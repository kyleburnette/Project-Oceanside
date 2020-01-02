#include "Room.h"

Room::Room()
{

}

void Room::AddActor(Node* actor)
{
	allActors.push_back(actor);
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

void Room::Memes()
{
	currentlyLoadedActors = allActors;
}

void Room::RemoveCurrentlyLoadedActor(Node* node)
{
	currentlyLoadedActors.erase(std::remove(currentlyLoadedActors.begin(), 
		currentlyLoadedActors.end(), node), currentlyLoadedActors.end());
}

void Room::PrintCurrentlyLoadedActors() const
{
	for (auto node : currentlyLoadedActors)
	{
		std::cout << node->GetID() << " " << node->GetPriority() << std::endl;
	}
}

void Room::PrintSize() const
{
	std::cout << currentlyLoadedActors.size() << std::endl;
}

void Room::ResetCurrentlyLoadedActors()
{
	currentlyLoadedActors.clear();
}