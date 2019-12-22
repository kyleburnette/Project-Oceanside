#pragma once

#include <vector>
#include "Room.h"
#include "Node.h"

class Scene
{
public:
	Scene();
	int GetRoomCount() const;
	void PrintRooms() const;
	Room* GetRoom(int roomNumber);
private:
	bool clockReallocates = false;
	int roomCount = 0;
	std::vector<Room*> rooms;

};

