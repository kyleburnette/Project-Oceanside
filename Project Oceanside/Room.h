#pragma once

#include <vector>
#include "Node.h"	

class Room
{
public:
	Room(int roomNumber);
	void AddActor(Node* actor);
	void AddCurrentlyLoadedActor(Node* actor);
	std::vector<Node*> GetAllActors() const;
	std::vector<Node*> GetCurrentlyLoadedActors() const;
	void RemoveCurrentlyLoadedActor(Node* node);
	void ResetCurrentlyLoadedActors();
	std::vector<Node*> GetClearedActors() const;
	std::vector<Node*> GetClearableActors() const;
	std::vector<Node*> GetDeallocatableActors() const;
	void ClearActor(Node* actor);
	int GetRoomNumber() const;
	
private:
	int roomNumber;
	std::vector<Node*> allActors;
	std::vector<Node*> currentlyLoadedActors;
	std::vector<Node*> deallocatableActors;
	std::vector<Node*> clearableActors;
	std::vector<Node*> clearedActors;
};

