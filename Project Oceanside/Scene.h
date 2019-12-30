#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "./json.hpp"

#include "Room.h"
#include "Node.h"

class Scene
{
public:
	Scene();
	int GetRoomCount() const;
	void PrintRooms() const;
	Room* GetRoom(int roomNumber) const;
	bool GetClockReallocates() const;

private:
	bool clockReallocates = false;
	int roomCount = 0;
	std::vector<Room*> rooms;
	nlohmann::json actorJson;

};

