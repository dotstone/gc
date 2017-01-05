// GarbageCollector.cpp : main project file.

#include "stdafx.h"
#include "Heap.h"
#include <iostream>

using namespace std;

void createTypeDescriptors(Heap* heap);

void* createLecture(Heap* heap, int id, char* name, int semester);
void* createStudent(Heap* heap, int id, char* name, void* lectureNode);
void* createLectureNode(Heap* heap, void* next, void* lecture);
void* createStudentNode(Heap* heap, void* next, void* student);
void* createStudentList(Heap* heap, void* first);

int main(array<System::String ^> ^args)
{
	Heap* heap = new Heap();

	createTypeDescriptors(heap);

	// Create sample instances
	void* syssoft = createLecture(heap, 1, "System Software", 8);
	void* compiler = createLecture(heap, 2, "Uebersetzerbau", 4);

	void* syssoftNode = createLectureNode(heap, NULL, syssoft);
	void* compilerNode = createLectureNode(heap, syssoftNode, compiler);

	void* gabriel = createStudent(heap, 1, "Gabriel Schoerghuber", compilerNode);

	void* gabrielNode = createStudentNode(heap, NULL, gabriel);

	void* studentList = createStudentList(heap, gabrielNode);

	// TODO: Do some action on the students and verify the gc with dump()
	heap->gc(studentList);
	heap->dump();

	system("pause");
	delete heap;
    return 0;
}

void createTypeDescriptors(Heap* heap) {
	map<string, int> offsets;
	offsets["first"] = 0;
	Descriptor* descriptor = new Descriptor("StudentList", 4, offsets);
	heap->registerType("StudentList", descriptor);

	offsets = map<string, int>();
	offsets["next"] = 0;
	offsets["stud"] = 4;
	descriptor = new Descriptor("StudNode", 8, offsets);
	heap->registerType("StudNode", descriptor);

	offsets = map<string, int>();
	offsets["id"] = 0;
	offsets["name"] = 4;
	offsets["lect"] = 36;
	descriptor = new Descriptor("Student", 40, offsets);
	heap->registerType("Student", descriptor);

	offsets = map<string, int>();
	offsets["next"] = 0;
	offsets["lect"] = 4;
	descriptor = new Descriptor("LectNode", 8, offsets);
	heap->registerType("LectNode", descriptor);

	offsets = map<string, int>();
	offsets["id"] = 0;
	offsets["name"] = 4;
	offsets["semester"] = 36;
	descriptor = new Descriptor("Lecture", 40, offsets);
	heap->registerType("Lecture", descriptor);
}

void* createLecture(Heap* heap, int id, char* name, int semester) {
	int* lecture = (int*) heap->alloc("Lecture");
	*lecture = id;
	strcpy((char*)(lecture + 1), name);
	*(lecture + 9) = semester;
	return lecture;
}

void* createStudent(Heap* heap, int id, char* name, void* lectureNode) {
	int* student = (int*)heap->alloc("Student");
	*student = id;
	strcpy((char*)(student + 1), name);
	*(student + 9) = (int) lectureNode;
	return student;
}

void* createLectureNode(Heap* heap, void* next, void* lecture) {
	int* lectureNode = (int*)heap->alloc("LectNode");
	*lectureNode = (int)next;
	*(lectureNode + 1) = (int)lecture;
	return lectureNode;
}

void* createStudentNode(Heap* heap, void* next, void* student) {
	int* studentNode = (int*)heap->alloc("StudNode");
	*studentNode = (int)next;
	*(studentNode + 1) = (int)student;
	return studentNode;
}

void* createStudentList(Heap* heap, void* first) {
	int* studentList = (int*)heap->alloc("StudentList");
	*studentList = (int)first;
	return studentList;
}