#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "Node.h"	

class Room
{
public:
	Room();
	void AddActor(Node* actor);
	std::vector<Node*> GetActors() const;
	
private:
	std::vector<Node*> actors;
};

