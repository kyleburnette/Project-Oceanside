#pragma once

#include <vector>
#include <map>
#include <string>

#include "Node.h"	

class Room
{
public:
	Room();
	void AddActor(Node* actor);
	void PrintActors() const;
	std::vector<Node*> GetActors();
private:
	std::vector<Node*> actors;
	std::map<int, Room*> connections;
};

