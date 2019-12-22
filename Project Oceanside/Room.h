#pragma once

#include <vector>
#include <map>
#include <string>

class Room
{
public:
	Room();
private:
	std::vector<std::string> actors;
	std::map<int, Room*> connections;
};

