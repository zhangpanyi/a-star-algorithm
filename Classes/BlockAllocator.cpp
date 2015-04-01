#include <limits.h>
#include <memory.h>
#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#include "BlockAllocator.h"

int BlockAllocator::block_sizes_[g_blockSizes] =
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
char BlockAllocator::s_block_size_lookup_[g_maxBlockSize + 1];
bool BlockAllocator::s_block_size_lookup_initialized_;

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

	num_chunk_space_ = g_chunkArrayIncrement;
	num_chunk_count_ = 0;
	chunks_ = (Chunk *)malloc(num_chunk_space_ * sizeof(Chunk));

	memset(chunks_, 0, num_chunk_space_ * sizeof(Chunk));
	memset(free_lists_, 0, sizeof(free_lists_));

	if (s_block_size_lookup_initialized_ == false)
	{
		int j = 0;
		for (int i = 1; i <= g_maxBlockSize; ++i)
		{
			assert(j < g_blockSizes);
			if (i <= block_sizes_[j])
			{
				s_block_size_lookup_[i] = (char)j;
			}
			else
			{
				++j;
				s_block_size_lookup_[i] = (char)j;
			}
		}

		s_block_size_lookup_initialized_ = true;
	}
}

BlockAllocator::~BlockAllocator()
{
	for (int i = 0; i < num_chunk_count_; ++i)
	{
		::free(chunks_[i].blocks);
	}

	::free(chunks_);
}

void* BlockAllocator::Allocate(int size)
{
	if (size == 0)
		return nullptr;

	assert(0 < size);

	if (size > g_maxBlockSize)
	{
		return malloc(size);
	}

	int index = s_block_size_lookup_[size];
	assert(0 <= index && index < g_blockSizes);

	if (free_lists_[index])
	{
		Block *block = free_lists_[index];
		free_lists_[index] = block->next;
		return block;
	}
	else
	{
		if (num_chunk_count_ == num_chunk_space_)
		{
			Chunk *oldChunks = chunks_;
			num_chunk_space_ += g_chunkArrayIncrement;
			chunks_ = (Chunk *)malloc(num_chunk_space_ * sizeof(Chunk));
			memcpy(chunks_, oldChunks, num_chunk_count_ * sizeof(Chunk));
			memset(chunks_ + num_chunk_count_, 0, g_chunkArrayIncrement * sizeof(Chunk));
			::free(oldChunks);
		}

		Chunk *chunk = chunks_ + num_chunk_count_;
		chunk->blocks = (Block *)malloc(g_chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, g_chunkSize);
#endif
		int blockSize = block_sizes_[index];
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

		free_lists_[index] = chunk->blocks->next;
		++num_chunk_count_;

		return chunk->blocks;
	}
}

void BlockAllocator::Free(void *p, int size)
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

	int index = s_block_size_lookup_[size];
	assert(0 <= index && index < g_blockSizes);

#ifdef _DEBUG
	int blockSize = block_sizes_[index];
	bool found = false;
	for (int i = 0; i < num_chunk_count_; ++i)
	{
		Chunk *chunk = chunks_ + i;
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
	block->next = free_lists_[index];
	free_lists_[index] = block;
}

void BlockAllocator::Clear()
{
	for (int i = 0; i < num_chunk_count_; ++i)
	{
		::free(chunks_[i].blocks);
	}

	num_chunk_count_ = 0;
	memset(chunks_, 0, num_chunk_space_ * sizeof(Chunk));

	memset(free_lists_, 0, sizeof(free_lists_));
}