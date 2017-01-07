#include "stdafx.h"
#include "Heap.h"
#include <list>
#include <string>
#include <iostream>

using namespace System;
using namespace std;

bool isUsed(int* block);
int* tag2Address(int tag);
bool isMarked(int tag);
int tag(int* block);
int objSizeOfDescriptor(int* descriptor);

/* Block Layout
Used Block:
Bytes 0-3: 0x80000000 + blocksize
Bytes 4-7: descriptorAddr + marked (0/1)

Free Block:
Bytes 0-3: blocksize
Bytes 4-7: next pointer
*/

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

	int* descriptor = (typeDescriptors[className]);
	int requiredSize = objSizeOfDescriptor(descriptor);

	// look for an empty slot in freeList (take the first block which fits the object size
	int* start = freelist;
	int* prev = NULL;
	int* cur = freelist;

	// iterate through the free list to find a sufficiently large block
	int length = *cur;
	while (length < requiredSize) {
		prev = cur;
		cur = (int*)*(cur + 1);
		length = *cur;

		if (cur == start) {
			cout << "No free block with sufficient size found!";
			return NULL;
		}
	}

	int* block = cur;
	int newLen = length - requiredSize;

	if (newLen >= 8) {
		// split block
		block += (length - requiredSize) / 4;
		*block = requiredSize;
		*cur = newLen;
	}
	else if ( *(cur+1) == (int)cur) {
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
	memset(block + 2, 0, *block - 8);

	// set the used bit to 1
	*block |= 0x80000000;

	// set type descriptor (mark bit = 0)
	*(block + 1) = (int) descriptor;

	// return address of the object itself (size and type tag excluded)
	return block + 2;
}

void Heap::registerType(string className, int* typeDescrAddr) {
	typeDescriptors[className] = typeDescrAddr;
}

void Heap::gc(void* root) {
	if (root != NULL) {
		mark((int*)root);
	}
	sweep();
}

void Heap::mark(int* root) {
	int* cur = (int*)root - 2;
	int* prev = NULL;

	if (cur == NULL) {
		return;
	}

	while (true) {
		// Mark
		*(cur + 1) |= 1;		// Mark flag
		*(cur + 1) += 4;		// increment cur.i

		int* descrAddr = tag2Address(tag(cur));

		if (*descrAddr >= 0) {
			// advance
			// check if the field holds a pointer that's inside the heap
			int* pAddr = cur + 2 + (*descrAddr / 4);
			int* p = ((int*)*pAddr) - 2;
			if (isInHeap(p)) {
				if (!isMarked(tag(p))) {
					// Set the prev and dig in (DSW)
					*pAddr = (int)prev;
					prev = cur;
					cur = p;
				}
			}
		}
		else {
			// retreat
			// we reached the end of the descriptor fields -> reset descriptor address
			*(cur + 1) = *(cur + 1) + *(tag2Address(tag(cur)));

			if (prev == NULL) {
				return;
			}
			int* p = cur;
			cur = prev;

			// reset pointer fields (DSW)
			descrAddr = tag2Address(tag(cur));
			int* prevAddr = cur + 2 + (*descrAddr / 4);
			prev = (int*)*prevAddr;
			*prevAddr = (int)p + 2;
		}
	}
}

bool isUsed(int* block) {
	return (*block & 0x80000000) > 0;
}

int tag(int* block) {
	return *(block + 1);
}

int* tag2Address(int tag) {
	return (int*)(tag & 0xFFFFFFFC);
}

bool isMarked(int tag) {
	return tag & 1 > 0;
}

int sizeOfBlock(int* block) {
	return *block & 0x7FFFFFFF;
}

int objSizeOfDescriptor(int* descriptor) {
	return *descriptor;
}

void Heap::sweep() {
	int blockSize;
	int* free = NULL;
	for (int* blockAddr = (int*)heap; (int)blockAddr < (int)heap + 32 * 1024; blockAddr += blockSize / 4) {
		blockSize = sizeOfBlock(blockAddr);

		if (isMarked(tag(blockAddr))) {
			// Unset the marked bit
			*(blockAddr + 1) &= 0xFFFFFFFE;
		}
		else {
			// collect
			// Iterate through the heap until we find a block that's used and marked
			int* q = blockAddr + (blockSize / 4);
			int size = blockSize;
			while (q < (int*)heap + 8 * 1024 && (!isUsed(q) || !isMarked(tag(q)))) {
				size += sizeOfBlock(q);
				q = q + (sizeOfBlock(q) / 4);
			}
			// Merge blocks
			*blockAddr = size;
			blockSize = size;
			*(blockAddr + 1) = (int)free;
			free = blockAddr;

			// Freelist gets set to the last free block
			freelist = blockAddr;
		}
	}
}

bool Heap::isInHeap(int* address) {
	return address >= heap && address < (int*)heap + 8 * 1024;
}

void Heap::dump() {
	int* heapInt = (int*)heap;

	cout << "Used blocks: " << endl;
	int* blockAddr = (int*) heap;
	void* startFreeList;
	int blockSize;
	int totalSize = 0;
	int totalUsed = 0;
	// Iterate over all blocks, filter used blocks
	for (int* blockAddr = (int*)heap; (int)blockAddr < (int)heap + 32 * 1024; blockAddr += blockSize / 4) {
		blockSize = sizeOfBlock(blockAddr);
		int size = blockSize - 8;
		if (isUsed(blockAddr)) {
			totalSize += blockSize;
			totalUsed++;

			int* descriptor = tag2Address(tag(blockAddr));
			string type = "Type not found";
			for (auto iter : typeDescriptors) {
				if (iter.second == descriptor) {
					type = iter.first;
				}
			}
			cout << endl << "\tAddress: 0x" << hex << uppercase << blockAddr+2 << dec;
			cout << endl << "\t\tSize: " << size;
			cout << endl << "\t\tType: " << type;
			cout << endl << "\t\tDump: 0x" << hex << uppercase << *(blockAddr + 2) << dec;
			cout << endl << "\t\tPointers:";
			
			for (int i = 1; *(descriptor + i) >= 0; i++) {
				int* address = blockAddr + 2 + (*(descriptor+i) / 4);
				cout << endl << "\t\t\t" << "0x" << hex << uppercase << address << " (0x" << *address << ")" << dec;
			}
		}
	}

	cout << endl << totalUsed << " objects use " << totalSize << " bytes in heap" << endl << endl;

	cout << "Free blocks: " << endl;
	// Iterate over freelist
	if (freelist == NULL) {
		cout << "No free blocks!";
	}
	else {
		int* blockAddr = freelist;
		do {
			int size = *blockAddr;
			int offset = (int)blockAddr - (int)heap;
			if (offset >= 16 * 1024) {
				offset -= 32 * 1024;
			}
			cout << "\tIndex: 0x" << blockAddr << " (Offset: " << offset << ")" << ": " << size << " bytes" << endl;
			blockAddr = (int*) *(blockAddr + 1);
		} while (blockAddr != freelist && blockAddr != NULL);
	}
	cout << endl;
}

Heap::~Heap() {
	free(heap);
}