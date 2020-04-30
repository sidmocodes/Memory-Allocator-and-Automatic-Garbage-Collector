#include <stdio.h>

typedef unsigned int U_INT; //Data type stored in Memory

U_INT *mem_Array;
U_INT memory_Size;
U_INT available; //1st index of the 1st free range

U_INT blockSize(U_INT x)
{ //returns the size of the block
    return mem_Array[x];
}


U_INT nextFreeBlock(U_INT x)
{   //returns next free block
    return mem_Array[x + mem_Array[x]];
}


U_INT linkTonextFreeBlock(U_INT x)
{
    //return index of pointer to next free block
    return x + mem_Array[x]; 
}


void initialize(void *ptr, unsigned size)
{
    //initialize memory
    mem_Array = (U_INT *)ptr;
    memory_Size = size / sizeof(U_INT);
    mem_Array[0] = memory_Size - 1;
    mem_Array[memory_Size - 1] = memory_Size;
    available = 0;
}

//allocate memory
void *my_malloc(unsigned size)
{

    if (size == 0)
    { //return NULL pointer after attempt to allocate 0-length memory
        return NULL;
    }

    U_INT num = size / sizeof(U_INT);
    if (size % sizeof(U_INT) > 0)
        num++;
    U_INT current, previous;                     //pointer to (actually index of) currentrent block, previousious block
    U_INT isFirstFreeBeingAllocated = 1;        //whether the first free block is being allocated

    previous = current = available;


checkForSpace:
    //check if have enough free space for allocation

    if (available == memory_Size)
    { //if we are on the end of the memory
        return NULL;
    }

    if (blockSize(current) < num)
    { //if the size of free block is lower than requested
        isFirstFreeBeingAllocated = 0;
        previous = current;

        if (nextFreeBlock(current) == memory_Size)
        { //if not enough memory
            return NULL;
        }
        else
            current = nextFreeBlock(current);
        goto checkForSpace;
    }

    if (blockSize(current) == num)
    { //if the size of free block is equal to requested

        if (isFirstFreeBeingAllocated)
            available = nextFreeBlock(current);
        else
            mem_Array[linkTonextFreeBlock(previous)] = nextFreeBlock(current);
    }

    else
    { //if the size of free block is greater than requested

        if (isFirstFreeBeingAllocated)
        {
            if ((blockSize(current) - num) == 1) //if there is only 1 free item left from this (previousiously) free block
                available = nextFreeBlock(current);
            else
                available = current + num + 1;
        }
        else
        {
            if ((blockSize(current) - num) == 1) //if there is only 1 free item left from this (previousiously) free block
                mem_Array[linkTonextFreeBlock(previous)] = nextFreeBlock(current);
            else
                mem_Array[linkTonextFreeBlock(previous)] = current + num + 1;
        }

        if ((blockSize(current) - num) == 1) //if there is only 1 free item left from this (previousiously) free block
            mem_Array[current] = num + 1;
        else
        {
            mem_Array[current + num + 1] = blockSize(current) - num - 1;
            mem_Array[current] = num;
        }
    }

    return (void *)&(mem_Array[current + 1]);
}

//free memory
void my_free(void *ptr)
{

    U_INT toFree; //pointer to block (to free)
    U_INT current, previous;

    toFree = ((U_INT *)ptr - (mem_Array + 1));

    if (toFree < available)
    { //if block, that is being freed is before the first free block

        if (((linkTonextFreeBlock(toFree) + 1) == available) && available < memory_Size) //if nextFreeBlock free block is immediately after block that is being freed
            mem_Array[toFree] += (mem_Array[available] + 1);                        //defragmentation of free space
        else
            mem_Array[linkTonextFreeBlock(toFree)] = available;

        available = toFree;
    }

    else
    { //if block, that is being freed isn't before the first free block

        previous = current = available;

        while (current < toFree)
        {
            previous = current;
            current = nextFreeBlock(current);
        }

        if ((linkTonextFreeBlock(previous) + 1) == toFree)
        { //if previousious free block is immediately before block that is being freed

            mem_Array[previous] += (mem_Array[toFree] + 1); //defragmentation of free space

            if (((linkTonextFreeBlock(toFree) + 1) == current) && current < memory_Size) //if nextFreeBlock free block is immediately after block that is being freed
                mem_Array[previous] += (mem_Array[current] + 1);                        //defragmentation of free space
            else
                mem_Array[linkTonextFreeBlock(toFree)] = current;
        }
        else
        {
            mem_Array[linkTonextFreeBlock(previous)] = toFree;
            mem_Array[linkTonextFreeBlock(toFree)] = current;
        }
    }
}

int main()
{
    char region[30000000];     //space for memory allocation
    initialize(region, 30000000); //memory initialization

    return 0;
}