#include "stdafx.h"
#include "Descriptor.h"


Descriptor::Descriptor(std::string name, int objsize, std::map<std::string, int> offsets)
{
	Descriptor::name = name;
	Descriptor::objsize = objsize;
	Descriptor::pointerOffsets = offsets;
}
