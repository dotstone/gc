#include <map>
#include <list>

#include "Descriptor.h"

#pragma once
class Heap
{
	void* const heap = malloc(1024 * 32);
	void* freelist;
	std::map<std::string, Descriptor*> typeDescriptors;

public:
	Heap();
	~Heap();
	void* alloc(std::string className);
	void registerType(std::string className, Descriptor* typeDescrAddr);
	void gc(void* roots);
	void dump();
};

