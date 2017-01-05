#include <string>
#include <map>

#pragma once
class Descriptor
{
public:
	Descriptor(std::string name, int objsize, std::map<std::string, int> offsets);

	std::string name;
	int objsize;
	std::map<std::string, int> pointerOffsets;
};

