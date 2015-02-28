#ifndef BLOCKALLOCATOR_H
#define BLOCKALLOCATOR_H

#include "Singleton.h"

const int g_chunkSize = 16 * 1024;
const int g_maxBlockSize = 640;
const int g_blockSizes = 14;
const int g_chunkArrayIncrement = 128;

struct Block;
struct Chunk;

class BlockAllocator final : public Singleton < BlockAllocator >
{
	SINGLETON(BlockAllocator);

public:
	void* allocate(int size);

	void free(void *p, int size);

	void clear();

private:
	Chunk*			m_chunks;
	int				m_chunkCount;
	int				m_chunkSpace;
	Block*			m_freeLists[g_blockSizes];
	static int		s_blockSizes[g_blockSizes];
	static char		s_blockSizeLookup[g_maxBlockSize + 1];
	static bool		s_blockSizeLookupInitialized;
};

#endif