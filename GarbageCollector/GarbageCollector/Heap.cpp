#include "stdafx.h"
#include "Heap.h"
#include "Descriptor.h"
#include <list>
#include <string>
#include <iostream>

using namespace System;
using namespace std;

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

void* Heap::alloc(string className) {
	if (freelist == NULL) {
		cout << "No more free blocks!";
	}
	if (typeDescriptors.find(className) == typeDescriptors.end()) {
		cout << "Type " << className << " not found!";
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
			cout << "No free block with sufficient size found!";
			return NULL;
		}
	}

	int* block = cur;
	int newLen = length - (descriptor->objsize + 12);

	if (newLen >= 12) {
		// split block
		block += (length - (descriptor->objsize + 12)) / 4;
		*block = (descriptor->objsize + 12);
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
	memset(block + 1, 0, descriptor->objsize + 8);

	// set the used bit to 1
	*block = *block | 0x80000000;

	// set type descriptor (mark bit = 0)
	*(block + 1) = (int) descriptor;

	// return address of the object itself (size and type tag excluded)
	return block + 3;
}

void Heap::registerType(string className, Descriptor* typeDescrAddr) {
	typeDescriptors[className] = typeDescrAddr;
}

void Heap::gc(void* root) {
	mark((int*)root);
	sweep();
}

void Heap::mark(int* root) {
	int* cur = (int*)root - 3;
	int* prev = NULL;

	if (cur == NULL) {
		return;
	}

	while (true) {
		*(cur + 2) += 1;	// mark

		int curi = *(cur + 2);
		Descriptor* descriptor = (Descriptor*)*(cur + 1);
		int curn = descriptor->pointerOffsets.size();

		cout << "Mark " << descriptor->name << endl;

		if (curi <= curn) {
			// advance
			map<string, int>::iterator iterator(descriptor->pointerOffsets.begin());
			advance(iterator, curi - 1);
			int* pAddr = cur + 3 + (iterator->second / 4);
			int* p = ((int*)*pAddr) - 3;
			if (isInHeap(p)) {
				int pi = *(p + 2);
				if (pi <= 0) {
					*pAddr = (int)prev;
					prev = cur;
					cur = p;
				}
			}

			cout << "Advance" << endl;
		}
		else {
			// retreat
			if (prev == NULL) {
				return;
			}
			int* p = cur;
			cur = prev;

			descriptor = (Descriptor*)*(cur + 1);
			map<string, int>::iterator iterator(descriptor->pointerOffsets.begin());
			advance(iterator, *(cur + 2) - 1);
			int* prevAddr = cur + 3 + (iterator->second / 4);
			prev = (int*)*prevAddr;
			*prevAddr = (int)p + 3;

			cout << "Retreat" << endl;
		}
	}
}

void Heap::sweep() {

}

bool Heap::isInHeap(int* address) {
	return address >= heap && address < (int*)heap + 8 * 1024;
}

void Heap::dump() {
	int* heapInt = (int*)heap;

	cout << endl << endl << "Used blocks: ";
	int* blockAddr = (int*) heap;
	void* startFreeList;
	int totalSize;
	// Iterate over all blocks, filter used blocks
	for (int* blockAddr = (int*)heap; (int)blockAddr < (int)heap + 32 * 1024; blockAddr += totalSize / 4) {
		totalSize = *blockAddr & 0x7FFFFFFF;
		int size = totalSize - 8;
		if ( (*blockAddr & 0x80000000) != 0) {
			Descriptor* descriptor = (Descriptor*) *(blockAddr + 1);
			string type = descriptor->name;
			cout << endl << "\tAddress: 0x" << hex << uppercase << blockAddr+2 << dec;
			cout << endl << "\t\tSize: " << size;
			cout << endl << "\t\tType: " << type;
			cout << endl << "\t\tDump: 0x" << hex << uppercase << *(blockAddr + 3) << dec;
			cout << endl << "\t\tPointers:";
			
			for (auto iter : descriptor->pointerOffsets) {
				int* address = blockAddr + 3 + (iter.second / 4);
				cout << endl << "\t\t\t" << iter.first << " = 0x" << hex << uppercase << address << " (0x" << *address << ")" << dec;
			}
		}
	}

	cout << endl << endl << "Free blocks: ";
	// Iterate over freelist
	if (freelist == NULL) {
		cout << "No free blocks!";
	}
	else {
		int* blockAddr = freelist;
		do {
			int size = *blockAddr;
			cout << "\tIndex: 0x" << blockAddr << " (" << size << ")" << endl;
			blockAddr = (int*) *(blockAddr + 1);
		} while (blockAddr != freelist);
	}
	cout << endl;
}

Heap::~Heap() {
	free(heap);
}