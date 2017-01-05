#include "stdafx.h"
#include "Heap.h"
#include "Descriptor.h"
#include <list>
#include <string>
#include <iostream>

using namespace System;

Heap::Heap() {
	// initialize heap
	memset(heap, 0, 1024 * 32);
	
	// initialize free list with one single free block
	int* heapInt = (int*)heap;

	// setting size of the block
	*heapInt = 1024 * 32;

	// setting next pointer of block to itself
	*(heapInt + 1) = (int) heapInt;

	// freelist points to the first block
	freelist = heapInt;
}

void* Heap::alloc(std::string className) {
	if (freelist == NULL) {
		std::cout << "No more free blocks!";
	}
	if (typeDescriptors.find(className) == typeDescriptors.end()) {
		std::cout << "Type " << className << " not found!";
		return 0;
	}

	Descriptor* descriptor = (typeDescriptors[className]);

	// look for an empty slot in freeList (take the first block which fits the object size
	int* start = freelist;
	int* prev = NULL;
	int* cur = freelist;

	// iterate through the free list to find a sufficiently large block
	int length = *cur;
	while (length < (descriptor->objsize + 8)) {
		prev = cur;
		cur = (int*)*(cur + 1);
		length = *cur;

		if (cur == start) {
			std::cout << "No free block with sufficient size found!";
			return NULL;
		}
	}

	int* block = cur;
	int newLen = length - (descriptor->objsize + 8);

	if (newLen >= 8) {
		// split block
		block += (length - (descriptor->objsize + 8)) / 4;
		*block = (descriptor->objsize + 8);
		*cur = newLen;
	}
	else if ( *(cur+1) == ((int)cur)+1) {
		// last free block
		freelist = NULL;
	}
	else {
		if (prev == NULL) {
			// First free block
			freelist = (int*) *(cur + 1);
			// Iterate over the whole freelist to find the last free block
			while (*(prev+1) != (int) freelist) {
				prev = (int*) *(prev + 1);
			}
		}
		// remove block from list
		*(prev + 1) = *(cur + 1);
	}
	// set all data bytes in the block to 0
	memset(block + 1, 0, descriptor->objsize + 4);

	// set the used bit to 1
	*block = *block | 0x80000000;

	// set type descriptor (mark bit = 0)
	*(block + 1) = (int) descriptor;

	// return address of the object itself (size and type tag excluded)
	return block + 2;
}

void Heap::registerType(std::string className, Descriptor* typeDescrAddr) {
	typeDescriptors[className] = typeDescrAddr;
}

void Heap::gc(void* roots) {
	// TODO
}

void Heap::dump() {
	int* heapInt = (int*)heap;

	std::cout << std::endl << std::endl << "Used blocks: ";
	int* blockAddr = (int*) heap;
	void* startFreeList;
	int totalSize;
	// Iterate over all blocks, filter used blocks
	for (int* blockAddr = (int*)heap; (int)blockAddr < (int)heap + 32 * 1024; blockAddr += totalSize / 4) {
		totalSize = *blockAddr & 0x7FFFFFFF;
		int size = totalSize - 8;
		if ( (*blockAddr & 0x80000000) != 0) {
			Descriptor* descriptor = (Descriptor*) *(blockAddr + 1);
			std::string type = descriptor->name;
			std::cout << std::endl << "\tAddress: 0x" << std::hex << std::uppercase << blockAddr << std::dec;
			std::cout << std::endl << "\t\tSize: " << size;
			std::cout << std::endl << "\t\tType: " << type;
			std::cout << std::endl << "\t\tDump: 0x" << std::hex << std::uppercase << *(blockAddr + 2) << std::dec;
			std::cout << std::endl << "\t\tPointers:";
		}
	}

	std::cout << std::endl << std::endl << "Free blocks: ";
	// Iterate over freelist
	if (freelist == NULL) {
		std::cout << "No free blocks!";
	}
	else {
		int* blockAddr = freelist;
		do {
			int size = *blockAddr;
			std::cout << "\tIndex: 0x" << blockAddr << " (" << size << ")" << std::endl;
			blockAddr = (int*) *(blockAddr + 1);
		} while (blockAddr != freelist);
	}
	std::cout << std::endl;
}

Heap::~Heap() {
	free(heap);
}