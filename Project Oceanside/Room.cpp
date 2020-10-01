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
	//if (actor->IsSpawner())
	//{
	//	for (auto offspring : actor->GetOffspring())
	//	{
	//		deallocatableActors.push_back(offspring);
	//	}
	//}
	if (actor->IsTransitionActor())
	{
		AddTransitionActor(actor);
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
	actor->SetCleared(true);
	clearedActors.push_back(actor);
	RemoveCurrentlyLoadedActor(actor);
	clearableActors.erase(std::remove(clearableActors.begin(), clearableActors.end(), actor), clearableActors.end());
	deallocatableActors.erase(std::remove(deallocatableActors.begin(), deallocatableActors.end(), actor), deallocatableActors.end());
}

void Room::ResetClearedActors()
{
	for (auto actor : clearedActors)
	{
		if (!actor->StartCleared())
		{
			actor->SetCleared(false);
			clearedActors.erase(std::remove(clearedActors.begin(), clearedActors.end(), actor), clearedActors.end());
			clearableActors.push_back(actor);
			deallocatableActors.push_back(actor);
		}
	}
}

void Room::AddRandomAllocatableActor(int timesCanAllocate, Node* actor)
{
	possibleTemporaryActors[actor->GetID()] = std::make_pair(timesCanAllocate, actor);
	possibleTemporaryActorsIDs.push_back(actor->GetID());
}

void Room::AddTransitionActor(Node* actor)
{
	transitionActors[actor->GetSceneTransitionID()] = actor;
}

std::map<int, std::pair<int, Node*>> Room::GetPossibleTemporaryActors() const
{
	return possibleTemporaryActors;
}

void Room::DeallocateActor(Node* actor)
{
	if (actor->IsClearable())
	{
		ClearActor(actor);
		return;
	}

	currentlyDeallocatedActors.push_back(actor);
	deallocatableActors.erase(std::remove(deallocatableActors.begin(), deallocatableActors.end(), actor), deallocatableActors.end());
	RemoveCurrentlyLoadedActor(actor);
}

std::vector<int> Room::GetPossibleTemporaryActorsIDs() const
{
	return possibleTemporaryActorsIDs;
}

void Room::ReplenishDeallocatableActors()
{
	for (auto actor : currentlyDeallocatedActors)
	{
		deallocatableActors.push_back(actor);
	}

	currentlyDeallocatedActors.clear();
}

void Room::DumpRoomInfo() const
{
	std::cout << std::hex;
	std::cout << "---All Actors---\n";
	for (auto actor : allActors)
	{
		std::cout << actor->GetID() << " " << std::endl;
	}

	std::cout << "---Currently Loaded Actors---\n";
	for (auto actor : currentlyLoadedActors)
	{
		std::cout << actor->GetID() << std::endl;
	}

	std::cout << "---Deallocatable Actors---\n";
	for (auto actor : deallocatableActors)
	{
		std::cout << actor->GetID() << std::endl;
	}

	std::cout << "---Clearable Actors---\n";
	for (auto actor : clearableActors)
	{
		std::cout << actor->GetID() << std::endl;
	}

	std::cout << "---Cleared Actors---\n";
	for (auto actor : clearedActors)
	{
		std::cout << actor->GetID() << std::endl;
	}

	std::cout << "---Possible Temporary Actors---\n";
	for (auto actor : possibleTemporaryActorsIDs)
	{
		std::cout << actor << std::endl;
	}

	std::cout << "---Currently Deallocated Actors---\n";
	for (auto actor : currentlyDeallocatedActors)
	{
		std::cout << actor->GetID() << std::endl;
	}

	std::cout << "---Current Transition Actors---\n";
	for (auto actor : transitionActors)
	{
		std::cout << actor.second->GetID() << std::endl;
	}

	std::cout << std::dec;
}

void Room::AddDeallocatableActor(Node* actor)
{
	deallocatableActors.push_back(actor);
}

std::map<int, Node*> Room::GetTransitionActors() const
{
	return transitionActors;
}