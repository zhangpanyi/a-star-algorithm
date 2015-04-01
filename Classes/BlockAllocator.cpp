#include <limits.h>
#include <memory.h>
#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#include "BlockAllocator.h"

int BlockAllocator::sBlockSizes_[g_blockSizes] =
{
	16,		// 0
	32,		// 1
	64,		// 2
	96,		// 3
	128,	// 4
	160,	// 5
	192,	// 6
	224,	// 7
	256,	// 8
	320,	// 9
	384,	// 10
	448,	// 11
	512,	// 12
	640,	// 13
};
char BlockAllocator::sBlockSizeLookup_[g_maxBlockSize + 1];
bool BlockAllocator::sBlockSizeLookupInitialized_;

struct Chunk
{
	int blockSize;
	Block *blocks;
};

struct Block
{
	Block *next;
};

BlockAllocator::BlockAllocator()
{
	assert(g_blockSizes < UCHAR_MAX);

	nChunkSpace_ = g_chunkArrayIncrement;
	nChunkCount_ = 0;
	pChunks_ = (Chunk *)malloc(nChunkSpace_ * sizeof(Chunk));

	memset(pChunks_, 0, nChunkSpace_ * sizeof(Chunk));
	memset(pFreeLists_, 0, sizeof(pFreeLists_));

	if (sBlockSizeLookupInitialized_ == false)
	{
		int j = 0;
		for (int i = 1; i <= g_maxBlockSize; ++i)
		{
			assert(j < g_blockSizes);
			if (i <= sBlockSizes_[j])
			{
				sBlockSizeLookup_[i] = (char)j;
			}
			else
			{
				++j;
				sBlockSizeLookup_[i] = (char)j;
			}
		}

		sBlockSizeLookupInitialized_ = true;
	}
}

BlockAllocator::~BlockAllocator()
{
	for (int i = 0; i < nChunkCount_; ++i)
	{
		::free(pChunks_[i].blocks);
	}

	::free(pChunks_);
}

void* BlockAllocator::allocate(int size)
{
	if (size == 0)
		return nullptr;

	assert(0 < size);

	if (size > g_maxBlockSize)
	{
		return malloc(size);
	}

	int index = sBlockSizeLookup_[size];
	assert(0 <= index && index < g_blockSizes);

	if (pFreeLists_[index])
	{
		Block *block = pFreeLists_[index];
		pFreeLists_[index] = block->next;
		return block;
	}
	else
	{
		if (nChunkCount_ == nChunkSpace_)
		{
			Chunk *oldChunks = pChunks_;
			nChunkSpace_ += g_chunkArrayIncrement;
			pChunks_ = (Chunk *)malloc(nChunkSpace_ * sizeof(Chunk));
			memcpy(pChunks_, oldChunks, nChunkCount_ * sizeof(Chunk));
			memset(pChunks_ + nChunkCount_, 0, g_chunkArrayIncrement * sizeof(Chunk));
			::free(oldChunks);
		}

		Chunk *chunk = pChunks_ + nChunkCount_;
		chunk->blocks = (Block *)malloc(g_chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, g_chunkSize);
#endif
		int blockSize = sBlockSizes_[index];
		chunk->blockSize = blockSize;
		int blockCount = g_chunkSize / blockSize;
		assert(blockCount * blockSize <= g_chunkSize);
		for (int i = 0; i < blockCount - 1; ++i)
		{
			Block *block = (Block *)((char *)chunk->blocks + blockSize * i);
			Block *next = (Block *)((char *)chunk->blocks + blockSize * (i + 1));
			block->next = next;
		}
		Block *last = (Block *)((char *)chunk->blocks + blockSize * (blockCount - 1));
		last->next = nullptr;

		pFreeLists_[index] = chunk->blocks->next;
		++nChunkCount_;

		return chunk->blocks;
	}
}

void BlockAllocator::free(void *p, int size)
{
	if (size == 0)
	{
		return;
	}

	assert(0 < size);

	if (size > g_maxBlockSize)
	{
		::free(p);
		return;
	}

	int index = sBlockSizeLookup_[size];
	assert(0 <= index && index < g_blockSizes);

#ifdef _DEBUG
	int blockSize = sBlockSizes_[index];
	bool found = false;
	for (int i = 0; i < nChunkCount_; ++i)
	{
		Chunk *chunk = pChunks_ + i;
		if (chunk->blockSize != blockSize)
		{
			assert((char *)p + blockSize <= (char *)chunk->blocks ||
				(char *)chunk->blocks + g_chunkSize <= (char *)p);
		}
		else
		{
			if ((char *)chunk->blocks <= (char *)p && (char *)p + blockSize <= (char *)chunk->blocks + g_chunkSize)
			{
				found = true;
			}
		}
	}

	assert(found);

	memset(p, 0xfd, blockSize);
#endif

	Block *block = (Block *)p;
	block->next = pFreeLists_[index];
	pFreeLists_[index] = block;
}

void BlockAllocator::clear()
{
	for (int i = 0; i < nChunkCount_; ++i)
	{
		::free(pChunks_[i].blocks);
	}

	nChunkCount_ = 0;
	memset(pChunks_, 0, nChunkSpace_ * sizeof(Chunk));

	memset(pFreeLists_, 0, sizeof(pFreeLists_));
}