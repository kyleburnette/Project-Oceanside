#pragma once

#include <vector>
#include "Room.h"

class Scene
{
public:
	Scene(bool clockReallocates, int roomCount);
	int GetRoomCount();
private:
	bool clockReallocates = false;
	int roomCount = 0;
	std::vector<Room*> rooms;

};

