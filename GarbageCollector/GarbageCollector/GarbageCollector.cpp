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

	void* dominik = createStudent(heap, 2, "Dominik Steinbinder", compilerNode);
	void* dominikNode = createStudentNode(heap, NULL, dominik);

	void* gabriel = createStudent(heap, 1, "Gabriel Schoerghuber", compilerNode);
	void* gabrielNode = createStudentNode(heap, dominikNode, gabriel);

	void* studentList = createStudentList(heap, gabrielNode);

	heap->dump();
	cout << endl << "Removing Dominik node from Student List" << endl << endl;
	*(int*)gabrielNode = NULL;
	system("pause");
	cout << endl;
	heap->gc(studentList);	
	heap->dump();

	cout << endl << "Removing Gabriel node from Student List" << endl << endl;
	*(int*)studentList = NULL;
	system("pause");
	cout << endl;
	heap->gc(studentList);
	heap->dump();

	cout << endl << "Deleting StudentList" << endl << endl;
	system("pause");
	cout << endl;
	heap->gc(NULL);
	heap->dump();

	system("pause");
	delete heap;
    return 0;
}

void createTypeDescriptors(Heap* heap) {
	int* studentlist = (int*) malloc(4 + 4 + 4);
	*studentlist = 12;
	*(studentlist + 1) = 0;
	*(studentlist + 2) = -8;
	heap->registerType("StudentList", studentlist);

	int* studnode = (int*)malloc(4 + 8 + 4);
	*studnode = 16;
	*(studnode + 1) = 0;
	*(studnode + 2) = 4;
	*(studnode + 3) = -12;
	heap->registerType("StudNode", studnode);

	int* student = (int*)malloc(4 + 12 + 4);
	*student = 48;
	*(student + 1) = 0;
	*(student + 2) = 4;
	*(student + 3) = 36;
	*(student + 4) = -16;
	heap->registerType("Student", student);

	int* lectnode = (int*)malloc(4 + 8 + 4);
	*lectnode = 16;
	*(lectnode + 1) = 0;
	*(lectnode + 2) = 4;
	*(lectnode + 3) = -12;
	heap->registerType("LectNode", lectnode);

	int* lecture = (int*)malloc(4 + 12 + 4);
	*lecture = 48;
	*(lecture + 1) = 0;
	*(lecture + 2) = 4;
	*(lecture + 3) = 36;
	*(lecture + 4) = -16;
	heap->registerType("Lecture", lecture);
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