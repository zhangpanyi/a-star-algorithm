/**
 * 小对象管理器
 */

#ifndef __BLOCKALLOCATOR_H__
#define __BLOCKALLOCATOR_H__

#include <cstdint>
#include "singleton.h"

static const int g_chunk_size = 16 * 1024;
static const int g_max_block_size = 640;
static const int g_block_sizes = 14;
static const int g_chunk_array_increment = 128;

/// This is a small object allocator used for allocating small
/// objects that persist for more than one time step.
/// See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
class BlockAllocator
{
public:
	BlockAllocator();
	~BlockAllocator();

public:
	void* allocate(int size);
	void free(void *p, int size);
	void clear();

private:
	int				num_chunk_count_;
	int				num_chunk_space_;
	struct Chunk*	chunks_;
	struct Block*	free_lists_[g_block_sizes];
	static int		block_sizes_[g_block_sizes];
	static uint8_t	s_block_size_lookup_[g_max_block_size + 1];
	static bool		s_block_size_lookup_initialized_;
};

#endif