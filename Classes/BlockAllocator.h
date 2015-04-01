#ifndef BLOCKALLOCATOR_H
#define BLOCKALLOCATOR_H

#include "Singleton.h"

const int g_chunkSize = 16 * 1024;
const int g_maxBlockSize = 640;
const int g_blockSizes = 14;
const int g_chunkArrayIncrement = 128;

class BlockAllocator final : public Singleton < BlockAllocator >
{
	SINGLETON(BlockAllocator);

public:
	void* allocate(int size);

	void free(void *p, int size);

	void clear();

private:
	int				nChunkCount_;
	int				nChunkSpace_;
	struct Chunk*	pChunks_;
	struct Block*	pFreeLists_[g_blockSizes];
	static int		sBlockSizes_[g_blockSizes];
	static char		sBlockSizeLookup_[g_maxBlockSize + 1];
	static bool		sBlockSizeLookupInitialized_;
};

#endif