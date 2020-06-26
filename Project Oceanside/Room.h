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
	void ResetClearedActors();
	int GetRoomNumber() const;
	void AddRandomAllocatableActor(int timesCanAllocate, Node* actor);
	void AddDeallocatableActor(Node* actor);
	void AddTransitionActor(Node* actor);
	std::map<int, std::pair<int, Node*>> GetPossibleTemporaryActors() const;
	std::vector<int> GetPossibleTemporaryActorsIDs() const;
	void DeallocateActor(Node* actor);
	void ReplenishDeallocatableActors();
	void DumpRoomInfo() const;
	std::map<int, Node*> GetTransitionActors() const;

private:
	int roomNumber;
	std::vector<Node*> allActors;
	std::vector<Node*> currentlyLoadedActors;
	std::vector<Node*> deallocatableActors;
	std::vector<Node*> clearableActors;
	std::vector<Node*> clearedActors;
	std::map<int, Node*> transitionActors;
	std::map<int, std::pair<int, Node*>> possibleTemporaryActors;
	std::vector<int> possibleTemporaryActorsIDs;
	std::vector<Node*> currentlyDeallocatedActors;
};

