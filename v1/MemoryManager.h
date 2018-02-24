#pragma once

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

namespace MemoryManager {
	void	onOutofMemory();
	void	memView(int start, int end);
	void	initializeMemoryManager();
	void*	allocate(int aSize);
	void	deallocate(void* aPointer);
	int		inUseMemory();
	int		freeMemory();
	int		usedMemory();
};

#endif // __MEMORY_MANAGER_H__