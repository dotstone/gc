// GarbageCollector.cpp : main project file.

#include "stdafx.h"
#include "Heap.h"
#include <iostream>

int main(array<System::String ^> ^args)
{
	std::cout << "Initializing ...";

	Heap* heap = new Heap();

	// TODO: Create and register types: StudentList, StudNode, Student, LectNode, Lecture
	std::map<std::string, int> offsets;
	Descriptor* descriptor = new Descriptor(16, offsets);
	heap->registerType("StudentList", descriptor);

	// TODO: Create sample instances
	heap->alloc("StudentList");

	// TODO: Do some action on the students and verify the gc with dump()
	heap->gc(NULL);
	heap->dump();

	system("pause");
	delete heap;
    return 0;
}
