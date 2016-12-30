#include "stdafx.h"
#include "Heap.h"
#include <list>
#include <string>
#include <iostream>

using namespace System;

Heap::Heap() {
	// Allocate heap
	heap = malloc(1024 * 32);
	memset(heap, 0, 1024 * 32);
}

void* Heap::alloc(std::string className) {
	if (typeDescriptors[className] == NULL) {
		std::cout << "Type " << className << " not found!";
		return 0;
	}
	// TODO: look for an empty slot in freeList (take the first block which fits the object size
	// TODO: set the type descriptor accordingly (type tag + mark bit)
	// TODO: create an instance according to information in typeDescriptors[className]
	// TODO: return address of the object itself (type tag excluded?)
	return 0;
}

void Heap::registerType(std::string className, void* typeDescrAddr) {
	typeDescriptors[className] = typeDescrAddr;
}

void Heap::gc(void* roots) {
	// TODO
}

void Heap::dump() {
	// TODO
}

Heap::~Heap() {
	free(heap);
}