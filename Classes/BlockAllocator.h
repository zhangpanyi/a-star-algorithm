#pragma once

#include "Singleton.h"
#include "NonCopyable.h"

const int g_chunkSize = 16 * 1024;
const int g_maxBlockSize = 640;
const int g_blockSizes = 14;
const int g_chunkArrayIncrement = 128;

class BlockAllocator final : public Singleton < BlockAllocator >, public NonCopyable
{
	SINGLETON(BlockAllocator);

public:
	void* Allocate(int size);

	void Free(void *p, int size);

	void Clear();

private:
	int				num_chunk_count_;
	int				num_chunk_space_;
	struct Chunk*	chunks_;
	struct Block*	free_lists_[g_blockSizes];
	static int		block_sizes_[g_blockSizes];
	static char		s_block_size_lookup_[g_maxBlockSize + 1];
	static bool		s_block_size_lookup_initialized_;
};