#pragma once

#include <vector>
#include "Room.h"

class Scene
{
public:
	Scene(int roomNumber);
	void AddRoomToScene(Room* room);
private:
	int currentRoomNumber;
	std::vector<Room*> roomList;
};

