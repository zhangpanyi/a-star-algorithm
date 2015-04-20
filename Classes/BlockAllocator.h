#pragma once

#include "Singleton.h"
#include "NonCopyable.h"

const int g_chunk_size = 16 * 1024;
const int g_max_block_size = 640;
const int g_block_sizes = 14;
const int g_chunk_array_increment = 128;

/// This is a small object allocator used for allocating small
/// objects that persist for more than one time step.
/// See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
class BlockAllocator : public NonCopyable
{
public:
	BlockAllocator();
	~BlockAllocator();

public:
	void* Allocate(int size);
	void Free(void *p, int size);
	void Clear();

private:
	int				num_chunk_count_;
	int				num_chunk_space_;
	struct Chunk*	chunks_;
	struct Block*	free_lists_[g_block_sizes];
	static int		block_sizes_[g_block_sizes];
	static char		s_block_size_lookup_[g_max_block_size + 1];
	static bool		s_block_size_lookup_initialized_;
};

class SOA final : public Singleton < SOA >, public BlockAllocator
{
	SINGLETON_DEFAULT(SOA);
};