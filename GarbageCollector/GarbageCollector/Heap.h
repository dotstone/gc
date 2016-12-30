#include <map>
#include <list>

#include "Descriptor.h"

#pragma once
class Heap
{
	int* heap;
	std::map<std::string, Descriptor*> typeDescriptors;
	std::list<void*> freeList;

public:
	Heap();
	~Heap();
	void* alloc(std::string className);
	void registerType(std::string className, Descriptor* typeDescrAddr);
	void gc(void* roots);
	void dump();
};

