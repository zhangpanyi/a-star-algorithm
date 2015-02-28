#include <limits.h>
#include <memory.h>
#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#include "BlockAllocator.h"

int BlockAllocator::s_blockSizes[g_blockSizes] =
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
char BlockAllocator::s_blockSizeLookup[g_maxBlockSize + 1];
bool BlockAllocator::s_blockSizeLookupInitialized;

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

	m_chunkSpace = g_chunkArrayIncrement;
	m_chunkCount = 0;
	m_chunks = (Chunk *)malloc(m_chunkSpace * sizeof(Chunk));

	memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));

	if (s_blockSizeLookupInitialized == false)
	{
		int j = 0;
		for (int i = 1; i <= g_maxBlockSize; ++i)
		{
			assert(j < g_blockSizes);
			if (i <= s_blockSizes[j])
			{
				s_blockSizeLookup[i] = (char)j;
			}
			else
			{
				++j;
				s_blockSizeLookup[i] = (char)j;
			}
		}

		s_blockSizeLookupInitialized = true;
	}
}

BlockAllocator::~BlockAllocator()
{
	for (int i = 0; i < m_chunkCount; ++i)
	{
		::free(m_chunks[i].blocks);
	}

	::free(m_chunks);
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

	int index = s_blockSizeLookup[size];
	assert(0 <= index && index < g_blockSizes);

	if (m_freeLists[index])
	{
		Block *block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}
	else
	{
		if (m_chunkCount == m_chunkSpace)
		{
			Chunk *oldChunks = m_chunks;
			m_chunkSpace += g_chunkArrayIncrement;
			m_chunks = (Chunk *)malloc(m_chunkSpace * sizeof(Chunk));
			memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(Chunk));
			memset(m_chunks + m_chunkCount, 0, g_chunkArrayIncrement * sizeof(Chunk));
			::free(oldChunks);
		}

		Chunk *chunk = m_chunks + m_chunkCount;
		chunk->blocks = (Block *)malloc(g_chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, g_chunkSize);
#endif
		int blockSize = s_blockSizes[index];
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

		m_freeLists[index] = chunk->blocks->next;
		++m_chunkCount;

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

	int index = s_blockSizeLookup[size];
	assert(0 <= index && index < g_blockSizes);

#ifdef _DEBUG
	int blockSize = s_blockSizes[index];
	bool found = false;
	for (int i = 0; i < m_chunkCount; ++i)
	{
		Chunk *chunk = m_chunks + i;
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
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void BlockAllocator::clear()
{
	for (int i = 0; i < m_chunkCount; ++i)
	{
		::free(m_chunks[i].blocks);
	}

	m_chunkCount = 0;
	memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));

	memset(m_freeLists, 0, sizeof(m_freeLists));
}