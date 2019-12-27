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
	void AddTemporaryActor(Node* actor);
	void ClearTemporaryActors();
	void PrintActors() const;
	void PrintTemporaryActors() const;
	std::vector<Node*> GetActors() const;
	std::vector<Node*> GetTemporaryActors() const;
private:
	std::vector<Node*> actors;
	std::vector<Node*> temporaryActors;
	std::map<int, Room*> connections;
};

