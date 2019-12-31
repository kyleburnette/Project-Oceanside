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
	void AddCurrentlyLoadedActor(Node* actor);
	std::vector<Node*> GetAllActors() const;
	std::vector<Node*> GetCurrentlyLoadedActors() const;
	void Memes();
	void RemoveCurrentlyLoadedActor(Node* node);
	void PrintCurrentlyLoadedActors() const;
	
private:
	std::vector<Node*> allActors;
	std::vector<Node*> currentlyLoadedActors;
};

