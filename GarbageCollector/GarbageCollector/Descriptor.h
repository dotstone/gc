#include <string>
#include <map>

#pragma once
class Descriptor
{
public:
	Descriptor(int objsize, std::map<std::string, int> offsets);

	int objsize;
	std::map<std::string, int> pointerOffsets;
};

