/**
 * 小对象管理器
 */

#ifndef __BLOCKALLOCATOR_H__
#define __BLOCKALLOCATOR_H__

#include <cstdint>
#include "Singleton.h"

/// This is a small object allocator used for allocating small
/// objects that persist for more than one time step.
/// See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
class BlockAllocator
{
	static const int kChunkSize = 16 * 1024;
	static const int kMaxBlockSize = 640;
	static const int kBlockSizes = 14;
	static const int kChunkArrayIncrement = 128;

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
	struct Block*	free_lists_[kBlockSizes];
	static int		block_sizes_[kBlockSizes];
	static uint8_t	s_block_size_lookup_[kMaxBlockSize + 1];
	static bool		s_block_size_lookup_initialized_;
};

#endif