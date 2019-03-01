#include "memory_management.h"
#include <stdio.h>
#include <math.h>

Block *head = NULL; //entry point for our list, begins as null to identify when the first block is added

void * firstAlloc(size_t size)
{
  float x = (float) (size + sizeof(Block)) / 4096; //works out how many pages of 4096 we need
  int pagesNeeded = ceil(x);
  Block *blockOne = sbrk(pagesNeeded * 4096);
  Block *blockOneProper = (Block *)(((void*)blockOne)+ sizeof(Block)); //proper block means the start of the data after the header.
  blockOne->size = size;
  blockOne->free = 0;
  blockOne->prev = NULL;

  int check = (pagesNeeded * 4096) - size; //works out the amount of space left for block 2
  if (check < (sizeof(Block)*2) + 1) //if not enough space for a block header for block 2 dont make one
  {
    blockOne->next=NULL;
    head = blockOne;
    return blockOneProper;
  }
  else
  {
    Block *blockTwo = (Block *) (((void*)blockOneProper) + size);
    blockTwo->size = (pagesNeeded * 4096) - (size + (sizeof(Block) * 2)); //the second block will fit into the remaining space minus its own block header
    blockTwo->free = 1;
    blockTwo->prev = blockOne;
    blockTwo->next = NULL;
    blockOne->next = blockTwo;
  }
  head = blockOne; //sets our linked list entry point
  return blockOneProper;
}

Block *findLastBlock(Block *block) /*find last block in linked list and links it to newest allocation*/
{
  Block *start = head;
  while(1==1)
  {
    if(start->next == NULL)
    {
      start->next = block;
      return start;
    }
    start=start->next;
  }
}

Block *findBlock(size_t size) //finds a block that will fit size
{
  Block *start = head;
  while (1==1)
  {
    if (start->size >= (size + sizeof(Block)) && start->free == 1) //need enough space for header
    {
      return start;
    }
    else if (start->next == NULL)
    {
      return NULL;
    }

    else
    {
      start = start->next;
    }
  }
}

void lastBlockCheck() //checks if last block can be freed
{
  Block *start = head;
  while (1==1)
  {
    if (start->next == NULL)
    {
      int modCheck = (int)(start->size + sizeof(Block)) % 4096;
      if (modCheck == 0) //if the size of the block is a multiple of 4096
      {
        if (start->prev != NULL)
        {
          Block *prev = start->prev;
          prev->next = NULL;
        }

        int shrink = ((start->size) + sizeof(Block)) * -1;
        sbrk(shrink);
        break;
      }

      else
      {
        break;
      }
    }

    else
    {
      start=start->next;
    }
  }
}

void * _malloc(size_t size)
{
  if (head == NULL) //assuming size is below 4096
  {
    return firstAlloc(size);
  }

  else if (size == 0)
  {
    return NULL;
  }

  else
  {
    Block *newBlock = findBlock(size);

    if (newBlock == NULL) //If no available blocks
    {
      float x = (float) (size+sizeof(Block)) / 4096;
      int pagesNeeded = ceil(x);
      Block *blockOne = sbrk(4096 * pagesNeeded); //add check of size vs block
      Block *blockOneProper = (Block *)(((void*)blockOne)+ sizeof(Block));
      blockOne->size = size;
      blockOne->free = 0;
      blockOne->prev = findLastBlock(blockOne);

      int check = (pagesNeeded * 4096) - size;
      if (check < (sizeof(Block)*2) + 1)
      {
        blockOne->next=NULL;
        return blockOneProper;
      }

      else
      {
        Block *blockTwo = (Block *) (((void*)blockOneProper) + size);
        blockTwo->size = (pagesNeeded * 4096) - (size + (sizeof(Block) * 2));
        blockTwo->prev = blockOne;
        blockTwo->next = NULL;
        blockOne->next = blockTwo;
      }

      return blockOneProper;

    }

    else
    {
      Block *blockTwo = (Block *)(((void*)newBlock) + (size + sizeof(Block))); //create the leftover block
      blockTwo->size = newBlock->size - (size + sizeof(Block));
      blockTwo->next = NULL;
      blockTwo->prev = newBlock;
      blockTwo->free = 1;

      newBlock->size = size;
      newBlock->free = 0;
      newBlock->next = blockTwo;

      Block *newBlockProper = (Block *) (((void*)newBlock) + sizeof(Block));
      return newBlockProper;
    }
  }
}


void _free(void * ptr)
{
  Block *ptrr = (Block*)ptr;
  ptrr = (Block *)(((void*)ptrr) - sizeof(Block)); //cast to block pointer type
  lastBlockCheck();
  if (ptrr == NULL)
  {
    return NULL;
  }


  else
  {
    if (ptrr->free == 0)
    {

      if (ptrr->next != NULL)
      {
        Block *next = ptrr->next;
        if (next->free == 1) //if next block is free join them
        {
          ptrr->size = (ptrr->size) + sizeof(Block) + next->size;
          ptrr->next = next->next;
          ptrr->free = 1;
        }

        else
        {
          ptrr->free=1;
        }
      }

      if (ptrr->prev != NULL) //if previous block is free join them
      {
        Block *prev = ptrr->prev;
        if (prev->free == 1)
        {
          prev->size = prev->size + sizeof(Block) + ptrr->size;
          prev->next = ptrr->next;
          prev->free = 1;
        }

        else
        {
          ptrr->free=1;
        }
      }

      else
      {
        ptrr->free = 1;
      }
    }
    else
    {
      return NULL;
    }
  }

}
