#include <iostream>

#include "Room.h"

Room::Room(int roomNumber): roomNumber(roomNumber)
{

}

void Room::AddActor(Node* actor)
{
	allActors.push_back(actor);

	if (actor->IsClearable())
	{
		clearableActors.push_back(actor);
	}

	if (actor->IsDeallocatable())
	{
		deallocatableActors.push_back(actor);
	}

	if (actor->StartCleared())
	{
		clearedActors.push_back(actor);
	}
}

void Room::AddCurrentlyLoadedActor(Node* actor)
{
	currentlyLoadedActors.push_back(actor);
}

std::vector<Node*> Room::GetAllActors() const
{
	return allActors;
}

std::vector<Node*> Room::GetCurrentlyLoadedActors() const
{
	return currentlyLoadedActors;
}

void Room::RemoveCurrentlyLoadedActor(Node* node)
{
	currentlyLoadedActors.erase(std::remove(currentlyLoadedActors.begin(), 
		currentlyLoadedActors.end(), node), currentlyLoadedActors.end());
}

void Room::ResetCurrentlyLoadedActors()
{
	currentlyLoadedActors.clear();
}

std::vector<Node*> Room::GetClearedActors() const
{
	return clearedActors;
}

std::vector<Node*> Room::GetClearableActors() const
{
	return clearableActors;
}

std::vector<Node*> Room::GetDeallocatableActors() const
{
	return deallocatableActors;
}

int Room::GetRoomNumber() const
{
	return roomNumber;
}

void Room::ClearActor(Node* actor)
{
	actor->SetCleared();
	clearedActors.push_back(actor);
}

void Room::AddRandomAllocatableActor(int timesCanAllocate, Node* actor)
{
	possibleTemporaryActors[actor->GetID()] = std::make_pair(timesCanAllocate, actor);
}



void Room::Dump()
{
	for (auto actor : possibleTemporaryActors)
	{
		std::cout << std::hex << actor.first << std::dec << std::endl;
	}
}

std::map<int, std::pair<int, Node*>> Room::GetPossibleTemporaryActors() const
{
	return possibleTemporaryActors;
}