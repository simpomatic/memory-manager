#include "MemoryManager.h"
#include <iomanip>
#include <iostream>
using namespace std;

namespace MemoryManager {
	const int	MM_POOL_SIZE = 65536;
	char		MM_pool[65536];

	void onOutofMemory()
	{
		cout << "Not enough usable memory to complete this action. Action was aborted.";
	}

	// A tool that assists in keeping track of memory
	void memView(int start, int end) {
		const unsigned int SHIFT = 8;
		const unsigned int MASK = 1 << SHIFT - 1;

		unsigned int value;																				// used to facilitate bit shifting and masking

		cout << "         Pool                     Unsignd  Unsigned " << endl;
		cout << "Mem Add  indx   bits   chr ASCII#  short      int    " << endl;
		cout << "-------- ---- -------- --- ------ ------- ------------" << endl;

		for (int i = start; i <= end; i++) {
			cout << (long*)(MM_pool + i) << ":";														// the actual address in hexidecimal
			cout << '[' << setw(2) << i << ']';															// the index into MM_pool

			value = MM_pool[i];
			cout << " ";
			for (int j = 1; j <= SHIFT; j++) {															// the bit sequence for this byte (8 bits)
				cout << ((value & MASK) ? '1' : '0');
				value <<= 1;
			}
			cout << " ";
			cout << '|' << *(char*)(MM_pool + i) << "|  (";												// the ASCII character of the 8 bits (1 byte)
			cout << " (" << setw(4) << ((int)(*((unsigned char*)(MM_pool + i)))) << ")";				// the ASCII number for the character
			cout << " (" << setw(5) << (*(unsigned short*)(MM_pool + i)) << ")";						// the unsigned short value of 16 bits (2 bytes)
			cout << " (" << setw(10) << (*(unsigned int*)(MM_pool + i)) << ")";							// the unsigned int value of 32 bit (4 bytes)
			cout << endl;
		} 
	}

	// Initialize any data needed to manage the memory pool
	void initializeMemoryManager()
	{

		int freeHead = 0;																				//starting index of the freelist
		int inUseHead = 2;																				//starting index of the inUselist
		int usedHead = 4;																				//starting index of the usedlist - deallocated memory

		int nextLink = 2;																				//offset index of the next link
		int prevLink = 4;																				//offset index for the prev link

		*(unsigned short*)(MM_pool + freeHead) = 6;														//freelist starts at byte 6
		*(unsigned short*)(MM_pool + 6) = MM_POOL_SIZE - 6;												//we used 6 bytes to get things started
		*(unsigned short*)(MM_pool + inUseHead) = 0;													//nothing in the inUse list yet
		*(unsigned short*)(MM_pool + usedHead) = 0;														//nothing in the used list yet

	}

	// return a pointer inside the memory pool
	// If no chunk can accomodate aSize call onOutofMemory() - still
	void * allocate(int aSize)
	{
		// Not enough usable memory exists
		if (aSize > freeMemory()) {
			onOutofMemory();
			return nullptr;
		}
		// Enough usable memory exists
		else {
			//cout << "allocation:" << size << endl;
			// define locations of the lists
			int freeHead = 0;																			// starting index of the freelist
			int inUseHead = 2;																			// starting index of the inUselist

			// retrieving locations of lists
			freeHead = *(unsigned short*)(MM_pool + freeHead);
			inUseHead = *(unsigned short*)(MM_pool + inUseHead);

			// writing allocation information to newly allocated memory
			*(unsigned short*)(MM_pool + freeHead) = aSize;
			*(unsigned short*)(MM_pool + freeHead + 2) = 0;												// new allocations do not have a next link as they are the most recent allocation
			if (*(unsigned short*)(MM_pool + 2) == 0) {													// if there is no memory allocations,a previous link is not possible
				*(unsigned short*)(MM_pool + freeHead + 4) = 0;
			}
			else {																						// some memory has already been allocated thus a previous link is possible
				*(unsigned short*)(MM_pool + freeHead + 4) = inUseHead;
			}

			// writing new nextLink information for previously allocated memory
			*(unsigned short*)(MM_pool + inUseHead + 2) = freeHead;

			// write new values for the pertinent lists
			*(unsigned short*)(MM_pool) = (freeHead + 6 + aSize);
			*(unsigned short*)(MM_pool + 2) = freeHead;

			// location of the allocated memory
			void *memory = (char*)(MM_pool + freeHead + 6);

			return memory;
		}
	}

	// Free up a chunk previously allocated
	void deallocate(void * aPointer)
	{
		unsigned short usedHead = 4;																	// starting index of the usedlist - deallocated memory
		usedHead = *(unsigned short*)(MM_pool + usedHead);												// retrieve the index for the start of the usedlist

		// If you are deallocating the newest allocated memory
		if (((char*)aPointer - (char*)MM_pool - 6) == *(unsigned short*)(MM_pool + 2)) {
			*(unsigned short*)(MM_pool + 2) = *(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 2);
		}

		// memory has already been deallocated
		if (usedHead != 0) {

			//writing new next node to the most current deallocated memory and repairing the links between the given memory
			unsigned short prev = *(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 2);	// retrieves the index of the memory allocated before the given memory
			unsigned short next = *(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 4);	// retrieves the index of the memory allocated after the given memory
			*(unsigned short*)(MM_pool + usedHead + 2) = (char*)aPointer - (char*)MM_pool - 6;			// the index of the next node of the previous deallocated memory in the deallocated list will belong to the index of the given memory
			
			// IMPORTANT! If either the previous or next node is zero then you could access the headers used to point to the latest memory allocations or deallocations
			if (prev != 0) {
				*(unsigned short*)(MM_pool + prev + 2) = next;											// sets the new next node of the memory before the given memory to the index of the memory located after the given memory
			}
			if (next != 0) {
				*(unsigned short*)(MM_pool + next + 4) = prev;											// sets the new previous node of the memory after the given memory to the index of the memory located before the given memory
			}

			// rewriting the next node and previous node indexes for the newly deallocated memory
			*(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 2) = usedHead;			// setting the new previous node the latest deallocated memory
			*(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 4) = 0;					// This is newest deallocated memory thus the next node for the dedallocated memory should be zero

			// rewriting the usedHead index
			*(unsigned short*)(MM_pool + 4) = (char*)aPointer - (char*)MM_pool - 6;

		}

		// no deallocated memory exists
		else {

			// writing new next node to the most current deallocated memory and repairing the links between the given memory
			unsigned short prev = *(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 2);	// retrieves the index of the memory allocated before the given memory
			unsigned short next = *(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 4);	// retrieves the index of the memory allocated after the given memory
			*(unsigned short*)(MM_pool + 4) = (char*)aPointer - (char*)MM_pool - 6;						// usedHead will be modified to have the value of the index of the given memory
		
			// IMPORTANT! If either the previous or next node is zero then you could access the headers used to point to the latest memory allocations or deallocations
			if (prev != 0) {
				*(unsigned short*)(MM_pool + prev + 2) = next;											// sets the new next node of the memory before the given memory to the index of the memory located after the given memory
			}
			if (next != 0) {
				*(unsigned short*)(MM_pool + next + 4) = prev;											// sets the new previous node of the memory after the given memory to the index of the memory located before the given memory
			}

			// rewriting the next node and previous node indexes for the newly deallocated memory
			*(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 2) = 0;					// setting the new previous node the latest deallocated memory, zero because no such deallocated exists yet
			*(unsigned short*)(MM_pool + ((char*)aPointer - (char*)MM_pool) - 4) = 0;					// This is newest deallocated memory thus the next node for the dedallocated memory should be zero
		}
	}

	// Will scan the memory pool and return the total in use memory
	int inUseMemory()
	{
		unsigned short usedmem = 0;
		unsigned short inUseHead = 2;																	// starting index of the inUselist
		inUseHead = *(unsigned short*)(MM_pool + inUseHead);
		if (*(unsigned short*)(MM_pool + 2) != 0) {
			while (*(unsigned short*)(MM_pool + inUseHead + 4) != 0) {									// loops through all the used memory and adds up the total using the size located at beginning of the allocated memory
				usedmem += (*(unsigned short*)(MM_pool + inUseHead) + 6);
				inUseHead = *(unsigned short*)(MM_pool + inUseHead + 4);
			}
			usedmem += (*(unsigned short*)(MM_pool + inUseHead) + 6);									// to account for the memory that broke our while loop, as we did not add it's size to the total memory used
		}
		return usedmem;																					// used memory
	}

	// Will scan the memory pool and return the total free space remaining
	int freeMemory()
	{
		return MM_POOL_SIZE - inUseMemory() - usedMemory() - 6;
	}

	// Will scan the memory pool and return the total deallocated memory
	int usedMemory()
	{
		unsigned short usedmem = 0;
		unsigned short usedHead = 4;																	// starting index of the inUselist
		usedHead = *(unsigned short*)(MM_pool + usedHead);
		if (*(unsigned short*)(MM_pool + 4) != 0) {
			while (*(unsigned short*)(MM_pool + usedHead + 4) != 0) {									// loops through all the used memory and adds up the total using the size located at beginning of the allocated memory
				usedmem += (*(unsigned short*)(MM_pool + usedHead) + 6);
				usedHead = *(unsigned short*)(MM_pool + usedHead + 4);
			}
			usedmem += (*(unsigned short*)(MM_pool + usedHead) + 6);									// to account for the memory that broke our while loop, as we did not add it's size to the total memory used
		}
		return usedmem;																					// used memory
	}
}