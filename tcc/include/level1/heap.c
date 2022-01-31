#ifndef SYSTEM_HEAP_INCLUDED
#define SYSTEM_HEAP_INCLUDED


#include <types.c>


static Byte* allocate_memory   (Number size);
static void  free_memory       (Byte* memory_address);
static Byte* reallocate_memory (Byte* memory_address, Number new_size);


#ifdef __WIN32__

#include <Windows/kernel32.c>


/*
Bit8* allocate_memory(Bit32 size)
{
	return VirtualAlloc(0, size, COMMIT_MEMORY, EXECUTE_READ_AND_WRITE_MEMORY_ACCESS);
}


void free_memory(Bit8* memory_address)
{
	VirtualFree(memory_address, 0, RELEASE_MEMORY);
}
*/


static Byte* default_heap = 0;


static Byte* allocate_memory(Number size)
{
	if(!default_heap)
		default_heap = GetProcessHeap();
	
	Byte* memory_address = HeapAlloc(default_heap, 0, size);

	if(!memory_address)
	{
		log_error("out of memory")
	}

	//log("allocate ", write_N_32, size, " bytes: ", write_N_32, memory_address)

	return memory_address;
}


static void free_memory(Byte* memory_address)
{
	//log("free ", write_N_32, memory_address)

	if(!default_heap)
		default_heap = GetProcessHeap();

	HeapFree(default_heap, 0, memory_address);
}


static Byte* reallocate_memory(Byte* memory_address, Number new_size)
{
	if(!default_heap)
		default_heap = GetProcessHeap();

	return HeapReAlloc(default_heap, 0x00000008, memory_address, new_size);
}

#endif//__WIN32__


#endif//SYSTEM_HEAP_INCLUDED

/*
void main()
{
	Byte* memory = allocate_memory(32);
	memory[0] = 'H';
	memory[1] = 'i';
	memory[2] = '\0';
	printf(memory);
	free_memory(memory);
}*/