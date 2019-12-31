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
	Room* GetRoom(int roomNumber) const;
	bool GetClockReallocates() const;
	nlohmann::json GetActorJSON() const;

private:
	bool clockReallocates = false;
	int roomCount = 0;
	std::vector<Room*> rooms;
	nlohmann::json actorJson;
};

