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
	offsets["first"] = 0;
	// TODO: objsize always multiple of 4?
	Descriptor* descriptor = new Descriptor(8, offsets);
	heap->registerType("StudentList", descriptor);

	offsets.clear();
	offsets["next"] = 0;
	offsets["stud"] = 4;
	descriptor = new Descriptor(12, offsets);
	heap->registerType("StudentNode", descriptor);

	offsets.clear();
	offsets["name"] = 4;
	offsets["lect"] = 8;
	descriptor = new Descriptor(16, offsets);
	heap->registerType("Student", descriptor);

	offsets.clear();
	offsets["next"] = 0;
	offsets["lect"] = 4;
	descriptor = new Descriptor(12, offsets);
	heap->registerType("LectNode", descriptor);

	offsets.clear();
	offsets["name"] = 4;
	descriptor = new Descriptor(16, offsets);
	heap->registerType("Lecture", descriptor);

	// TODO: Create sample instances
	void* lecture = heap->alloc("Lecture");

	// TODO: Do some action on the students and verify the gc with dump()
	heap->gc(NULL);
	heap->dump();

	system("pause");
	delete heap;
    return 0;
}
