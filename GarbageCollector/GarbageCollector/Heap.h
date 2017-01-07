#include <map>
#include <list>

#include "Descriptor.h"

#pragma once
class Heap
{
	void* const heap = malloc(1024 * 32);
	int* freelist;
	std::map<std::string, int*> typeDescriptors;

	bool isInHeap(int* address);

	void mark(int* root);
	void sweep();

public:
	Heap();
	~Heap();
	void* alloc(std::string className);
	void registerType(std::string className, int* typeDescrAddr);
	void gc(void* root);
	void dump();
};

