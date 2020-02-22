#pragma once

#include <vector>
#include "Node.h"	

class Room
{
public:
	Room();
	void AddActor(Node* actor);
	void AddCurrentlyLoadedActor(Node* actor);
	std::vector<Node*> GetAllActors() const;
	std::vector<Node*> GetCurrentlyLoadedActors() const;
	void RemoveCurrentlyLoadedActor(Node* node);
	void ResetCurrentlyLoadedActors();
	std::vector<Node*> GetClearedActors();
	std::vector<Node*> GetClearableActors();
	std::vector<Node*> GetDeallocatableActors();
	void ClearActor(Node* actor);
	
private:
	std::vector<Node*> allActors;
	std::vector<Node*> currentlyLoadedActors;
	std::vector<Node*> deallocatableActors;
	std::vector<Node*> clearableActors;
	std::vector<Node*> clearedActors;
};

