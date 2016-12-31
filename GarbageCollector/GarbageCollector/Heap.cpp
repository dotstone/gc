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
	*heapInt = 1024 * 32;
	*(heapInt + 1) = 0;

	freelist = heapInt + 1;
}

void* Heap::alloc(std::string className) {
	if (typeDescriptors.find(className) == typeDescriptors.end()) {
		std::cout << "Type " << className << " not found!";
		return 0;
	}

	Descriptor* descriptor = (typeDescriptors[className]);

	// TODO: look for an empty slot in freeList (take the first block which fits the object size
	void* start = freelist;
	void* prev = freelist;
	
	int next = *((int*)freelist);
	freelist = *(&start + next);
	int length = *((int*)freelist - 1);

	// iterate through the free list to find a sufficient block
	while ( length < (descriptor->objsize + 4) && freelist != start) {
		prev = freelist;
		next = *((int*)freelist);
		freelist = *(&start + next);
		length = *((int*)freelist - 1);
	}

	if (length < (descriptor->objsize + 4)) {
		std::cout << "No free block with sufficient size found!";
		return 0;
	}
	else {
		char* block = (char*) freelist;
		int newLen = length - (descriptor->objsize + 4);

		if (newLen >= 8) {
			// split block
			block = block - 4 + length - descriptor->objsize;
			*((int*)block - 1) = (descriptor->objsize + 4);
			*((int*) freelist - 1) = newLen;
		}
		else if (freelist == prev) {
			// last free block
			freelist = NULL;
		}
		else {
			// remove block from list
			*((int*)prev) = *((int*)freelist);
		}
		// TODO: set all data bytes in the block to 0
		// memset(block, 0, *((int*) block - 1));

		// set the used bit to 1
		*((int*)block - 1) = *((int*)block - 1) | 0x80000000;
	} 
	
	// TODO: set the type descriptor accordingly (type tag + mark bit)
	// TODO: create an instance according to information in typeDescriptors[className]
	// TODO: return address of the object itself (type tag excluded?)
	return 0;
}

void Heap::registerType(std::string className, Descriptor* typeDescrAddr) {
	typeDescriptors[className] = typeDescrAddr;
}

void Heap::gc(void* roots) {
	// TODO
}

void Heap::dump() {
	int* heapInt = (int*)heap;

	std::cout << "\n" << "Used blocks: ";
	int blockAddr = 0;
	do {
		if (*(heapInt + blockAddr) & 0x80000000 != 0) {
			int size = *(heapInt + blockAddr) & 0x7FFFFFFF;
			std::string type = "Some Type";
			std::string hex = "0xBADEAFFE";
			std::cout << "\n\tAddress: " << (heapInt + blockAddr) << " (" << blockAddr << ")";
			std::cout << "\n\t\tSize: " << size;
			std::cout << "\n\t\tType: " << type;
			std::cout << "\n\t\tDump: " << hex;
			std::cout << "\n\t\tPointers:";
		}

		blockAddr = *(heapInt + blockAddr + 1);
	} while (blockAddr > 0);

	std::cout << "\n\n" << "Free blocks: ";
	blockAddr = 0;
	do {
		if ((*(heapInt + blockAddr) & 0x80000000) == 0) {
			int size = *(heapInt + blockAddr) & 0x7FFFFFFF;
			std::cout << "\tIndex: " << blockAddr << " (" << size << ")";
		}
		blockAddr = *(heapInt + blockAddr + 1);
	} while (blockAddr > 0);
}

Heap::~Heap() {
	free(heap);
}