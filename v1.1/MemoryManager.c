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
  void *buffer = malloc(size);
  int blockSize = size - (_overheadSize + _pointerSize);
  (*free_head).block_size = blockSize;
  (*free_head).next_block = buffer;
  print((*free_head).block_size);
  print((*free_head).next_block);
}

void *my_alloc(int size)
{
  int allocated = 0;
  struct Block *currentBlock = free_head;
  void *allocatedBlock;
  while (allocated == 0)
  {
    // A pointer that points to nothing will be equal to zero, and !0 is always true.
    if (!(*currentBlock).next_block)
    {
      // Assuming we can honor this allocation, we are looking at the remaining space minus the
      // overhead and pointer of a theoretical block to get the ammount of meaningful space left.
      int remainingSpace = (*currentBlock).block_size - (_overheadSize + _pointerSize);

      // Needs to be divisible by four, so increment allocation size until it is.
      while (size % 4 != 0)
      {
        ++size;
      }

      // Make sure we have enough meaningful space to honor this allocation.
      if (remainingSpace >= size)
      {
      }
    }
  }
}

int main()
{
  my_initialize_heap(1000);
}