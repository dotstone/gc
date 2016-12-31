#include "stdafx.h"
#include "Heap.h"
#include "Descriptor.h"
#include <list>
#include <string>
#include <iostream>

using namespace System;

Heap::Heap() {
	// Allocate heap
	heap = (int *) malloc(1024 * 32);
	memset(heap, 0, 1024 * 32);

	// initialize free list with one single free block
	*heap = 1024 * 32;
	*(heap + 1) = 0;
}

void* Heap::alloc(std::string className) {
	if (typeDescriptors.find(className) == typeDescriptors.end()) {
		std::cout << "Type " << className << " not found!";
		return 0;
	}

	Descriptor* descriptor = (typeDescriptors[className]);

	// TODO: look for an empty slot in freeList (take the first block which fits the object size
	int* start = heap + 1;
	int* free = start;
	int* prev = start;

	std::cout << "Objsize: " << descriptor->objsize << "\n";
	
	while ( *(free - 1) > (descriptor->objsize + 4) && free != heap) {
		prev = free;
		free = (int*) (&heap + *free);
	}

	if (*(free - 1) < (descriptor->objsize + 4)) {
		std::cout << "No free block with sufficient size found!";
		return 0;
	}
	else {
		int* block = free;
		int newLen = *(free - 1) - (descriptor->objsize + 4);

		if (newLen >= 8) {
			
		}
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
	std::cout << "\n" << "Used blocks: ";
	int blockAddr = 0;
	do {
		if (*(heap + blockAddr) & 0x80000000 != 0) {
			int size = *(heap + blockAddr) & 0x7FFFFFFF;
			std::string type = "Some Type";
			std::string hex = "0xBADEAFFE";
			std::cout << "\n\tAddress: " << (heap + blockAddr) << " (" << blockAddr << ")";
			std::cout << "\n\t\tSize: " << size;
			std::cout << "\n\t\tType: " << type;
			std::cout << "\n\t\tDump: " << hex;
			std::cout << "\n\t\tPointers:";
		}

		blockAddr = *(heap + blockAddr + 1);
	} while (blockAddr > 0);

	std::cout << "\n\n" << "Free blocks: ";
	blockAddr = 0;
	do {
		if ((*(heap + blockAddr) & 0x80000000) == 0) {
			int size = *(heap + blockAddr) & 0x7FFFFFFF;
			std::cout << "\tIndex: " << blockAddr << " (" << size << ")";
		}
		blockAddr = *(heap + blockAddr + 1);
	} while (blockAddr > 0);
}

Heap::~Heap() {
	free(heap);
}