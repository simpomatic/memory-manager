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
  printf("%s\n", "Initialize Heap:");
  printf("%p\n", free_head);
  printf("%d\n", (*free_head).block_size);
}

void *my_alloc(int size)
{
  // Flag indicating that we have traversed at least one free block, we need this flag in
  // the event that memory is allocated in a later block. We will then have the previous block
  // point to the next available block of memory instead of pointing at memory that is now in use.
  int counter = 0;
  int freeSpace = (*free_head).block_size;
  struct Block *currentBlock = free_head;
  struct Block *previousBlock;
  while (!0)
  {
    // Assuming we can honor this allocation, we are looking at the remaining space minus the
    // overhead and pointer of a theoretical block to get the ammount of meaningful space left.
    int remainingSpace = (*currentBlock).block_size - (_overheadSize);

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
        // Offset free head by size of overhead, pointer size, and the desired amount of memory
        free_head = currentBlock + (_overheadSize + size);
        (*free_head).block_size = freeSpace - (_overheadSize + size);

        // Assign values to the allocated block
        (*currentBlock).block_size = size;
        (*currentBlock).next_block = free_head;
        return (void *) (currentBlock + _overheadSize);
      } else {
        printf("%s","Not enough memory is available to satisfy this request! Please ensure an acceptable amount is desired.");
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
          free_head = (*currentBlock).next_block;
          (*free_head).block_size = freeSpace - (_overheadSize + size);
        } 
        // The current block is not the first block in the free list; therefore, redirect the previous
        // block in the change to the next free block in the chain and do not change the free header.
        else {
          previousBlock = (*currentBlock).next_block;
          (*free_head).block_size = freeSpace - (_overheadSize + size);
        }

        // Assign values to the allocated block
        (*currentBlock).block_size = size;
        (*currentBlock).next_block = free_head;
        return (void *) (currentBlock + _overheadSize);
      } 
      // If there is not enough space, check the following block
      else {
        previousBlock = currentBlock;
        currentBlock = (*currentBlock).next_block;
        ++counter;
      }
    }
  }
}

int main()
{
  printf("%s\n", "Overhead size:");
  printf("%d\n", _overheadSize);
  printf("%s\n", "Pointer size:");
  printf("%d\n", _pointerSize);
  my_initialize_heap(1000);
  printf("%s\n", "Allocated Memory location:");
  printf("%p\n", my_alloc(10));
  printf("%s\n", "Updated free head location and size:");
  printf("%p\n", free_head);
  printf("%d\n", (*free_head).block_size);
}