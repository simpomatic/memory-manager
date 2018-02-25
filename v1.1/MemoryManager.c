#include <stdio.h>
#include <stdlib.h> 

struct Block
{
  int block_size;           // # of bytes in the data section
  struct Block *next_block; // in C, you have to use "struct Block" as the type
};

const static int _overheadSize = sizeof(struct Block);
const static int _pointerSize = sizeof(void *);
struct Block *free_head;

void my_initialize_heap(int size)
{
  free_head = (struct Block*) malloc(size);
  (*free_head).block_size = size;
  //printf("%s\n", "Initialize Heap:");
  //printf("%p\n", free_head);
  //printf("%d\n", (*free_head).block_size);
}

void *my_alloc(int size)
{
  // Flag indicating that we have traversed at least one free block, we need this flag in
  // the event that memory is allocated in a later block. We will then have the previous block
  // point to the next available block of memory instead of pointing at memory that is now in use.
  int counter = 0;
  int freeSpace = (*free_head).block_size;
  //printf("%d\n", freeSpace);
  struct Block *currentBlock = free_head;
  //printf("%p\n", currentBlock);
  //printf("%p\n", (*currentBlock).next_block);
  struct Block *previousBlock;
  while (!0)
  {
    //printf("%d\n", (*currentBlock).block_size);
    int remainingSpace = (*currentBlock).block_size;

    // A pointer that points to nothing will be equal to zero, and !0 is always true.
    if (!(*currentBlock).next_block)
    {
      // Needs to be divisible by four, so increment allocation size until it is.
      while (size % 4 != 0)
      {
        ++size;
      }

      // Make sure we have enough meaningful space to honor this allocation.
      if (remainingSpace >= size)
      {
        // Assuming we can honor this allocation, we are looking at the remaining space minus the
        // overhead and pointer of a theoretical block to get the amount of meaningful space left.
        remainingSpace -= _overheadSize;
        //Make sure this block isn't the end of the free list
        if (counter == 0) {
          // If there is enough meaningful space to split the current block, split it.
          if((remainingSpace - size) >= sizeof(_pointerSize)) {
            //printf("%s\n","Got here. Next block is null and split");
            // Offset free head by size of overhead, pointer size, and the desired amount of memory
            free_head = currentBlock + (_overheadSize + size);
            (*free_head).block_size = freeSpace - (_overheadSize + size);
            (*free_head).next_block = 0;
            (*currentBlock).block_size = size;
          } 
          // Not enough meaningful space is available to justify splitting the block
          else {
            //printf("%s\n","Got here. Next block is null and no split");
            free_head = 0;
            (*free_head).block_size = freeSpace - remainingSpace;
            (*currentBlock).block_size = remainingSpace;
          }
        } 
        // Block is not at the front of the list
        else {
          // If there is enough meaningful space to split the current block, split it.
          if((remainingSpace - size) >= sizeof(_pointerSize)) {
            //printf("%s\n","Got here. Next block is null and split and is not the front of free list");
            // Offset free head by size of overhead, pointer size, and the desired amount of memory
            (*currentBlock).block_size = size;
            (*previousBlock).next_block = currentBlock + (_overheadSize + size);
            (*(*previousBlock).next_block).block_size = remainingSpace - size;
          } 
          // Not enough meaningful space is available to justify splitting the block
          else {
            //printf("%s\n","Got here. Next block is null and no split and is not the front of free list");
            (*currentBlock).block_size = remainingSpace;
            (*previousBlock).next_block = (*currentBlock).next_block;
          }
        }

        // Assign values to the allocated block
        (*currentBlock).next_block = free_head;
        return (void *) (currentBlock + _overheadSize);
      } else {
        printf("%s\n","Not enough memory is available to satisfy this request! Please ensure an acceptable amount is desired.");
        void *nullPtr;
        return nullPtr;
      }
    } 
    // This block is free but points to another free block
    else {
      // Needs to be divisible by four, so increment allocation size until it is.
      while (size % 4 != 0)
      {
        ++size;
      }

      // Make sure we have enough meaningful space to honor this allocation.
      if (remainingSpace >= size)
      {
        // The current block is the head of the free list; therefore, simply redirect the free head to
        // the next block in the chain.
        if (counter == 0) {
          // If there is enough meaningful space to split the current block, split it.
          if((remainingSpace - size) >= sizeof(_pointerSize)) {
            //printf("%s\n","Got here. First in free head, Next block is not null and split");
            // Offset free head by size of overhead, pointer size, and the desired amount of memory
            free_head = currentBlock + (_overheadSize + size);
            (*free_head).block_size = remainingSpace - size;
            (*currentBlock).block_size = size;
            (*currentBlock).next_block = currentBlock + (_overheadSize + size);
          } 
          // Not enough meaningful space is available to justify splitting the block
          else {
            //printf("%s\n","Got here. First in free head, Next block is not null and no split");
            free_head = (*currentBlock).next_block;
            (*currentBlock).block_size = remainingSpace;
          }
        } 
        // The current block is not the first block in the free list; therefore, redirect the previous
        // block in the chain to the next free block in the chain and do not change the free header.
        else {
          // If there is enough meaningful space to split the current block, split it.
          if((remainingSpace - size) >= sizeof(_pointerSize)) {
            //printf("%s\n","Got here. Next block is not null and split");
            // Offset free head by size of overhead, pointer size, and the desired amount of memory
            free_head = currentBlock + (_overheadSize + size);
            (*currentBlock).block_size = size;
            (*currentBlock).next_block = currentBlock + (_overheadSize + size);
          } 
          // Not enough meaningful space is available to justify splitting the block
          else {
            //printf("%s\n","Got here. Next block is not null and no split");
            (*currentBlock).block_size = remainingSpace;
          }

          (*previousBlock).next_block = (*currentBlock).next_block;
        }

        return (void *) (currentBlock + _overheadSize);
      } 
      // If there is not enough space, check the following block
      else {
        //printf("%s\n","Got here. Free block was too small get next block");
        previousBlock = currentBlock;
        currentBlock = (*currentBlock).next_block;
        ++counter;
      }
    }
  }
}

// Seems to simple tbh
void my_free(void *data) {
  struct Block *currentBlock = (struct Block *)data - _overheadSize;
  (*currentBlock).next_block = free_head;
  free_head = currentBlock;
}


int main()
{
  my_initialize_heap(1000);
  int n = 2;
  switch (n)
  {
    case 0: // Allocate int, deallocate int, allocate another int
      {
        void *allocInt = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
        my_free(allocInt);
        void *allocInt2 = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
      }
      break;
    case 1:
      {
        void *allocInt = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
        void *allocInt2 = my_alloc(sizeof(int));
        printf("%p\n",allocInt2);
      }
      break;
    case 2:
      {
        void *allocInt = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
        void *allocInt2 = my_alloc(sizeof(int));
        printf("%p\n",allocInt2);
        void *allocInt3 = my_alloc(sizeof(int));
        printf("%p\n",allocInt3);
        my_free(allocInt2);
        void *allocDub = my_alloc(sizeof(double));
        printf("%p\n",allocDub);
        void *allocInt4 = my_alloc(sizeof(int));
        printf("%p\n",allocInt4);
      }
      break;
    case 3:
      {
        void *allocChar = my_alloc(sizeof(char));
        printf("%p\n",allocChar);
        void *allocInt = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
      }
      break;
    case 4:
      {
        void *allocIntArr = my_alloc(100*sizeof(int));
        printf("%p\n",allocIntArr);
        void *allocInt = my_alloc(sizeof(int));
        printf("%p\n",allocInt);
        my_free(allocIntArr);
        printf("%p\n",allocInt);
      }
      break;
    default:
      printf("%s\n","You done goofed boy.");
      break;
  }
}