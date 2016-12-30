// GarbageCollector.cpp : main project file.

#include "stdafx.h"
#include "Heap.h"
#include <iostream>

int main(array<System::String ^> ^args)
{
	std::cout << "Initializing ...";

	Heap* heap = new Heap();

	// TODO: Create and register types: StudentList, StudNode, Student, LectNode, Lecture
	heap->registerType("StudentList", (Descriptor*)1);

	// TODO: Create sample instances
	heap->alloc("StudentList");

	// TODO: Do some action on the students and verify the gc with dump()
	heap->gc(NULL);
	heap->dump();

	_sleep(2000);
	delete heap;
    return 0;
}
