#include <map>
#include <list>

#pragma once
class Heap
{
	void* heap;
	std::map<std::string, void*> typeDescriptors;
	std::list<void*> freeList;

public:
	Heap();
	~Heap();
	void* alloc(std::string className);
	void registerType(std::string className, void* typeDescrAddr);
	void gc(void* roots);
	void dump();
};

