#include "stdafx.h"
#include "Descriptor.h"


Descriptor::Descriptor(int objsize, std::map<std::string, int> offsets)
{
	Descriptor::objsize = objsize;
	Descriptor::pointerOffsets = offsets;
}
