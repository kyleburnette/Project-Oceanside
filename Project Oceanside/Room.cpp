#include "Room.h"

Room::Room()
{

}

void Room::AddActor(Node* actor)
{
	actors.push_back(actor);
}

void Room::AddTemporaryActor(Node* actor)
{
	temporaryActors.push_back(actor);
}

void Room::ClearTemporaryActors()
{
	for (Node* actor : temporaryActors)
	{
		delete(actor);
	}

	temporaryActors.clear();
}

std::vector<Node*> Room::GetActors() const
{
	return actors;
}

std::vector<Node*> Room::GetTemporaryActors() const
{
	return temporaryActors;
}

void Room::PrintActors() const
{
	for (auto actor : actors)
	{
		std::cout << actor->GetID() << std::endl;
	}
}

void Room::PrintTemporaryActors() const
{
	for (auto actor : temporaryActors)
	{
		std::cout << actor->GetID() << std::endl;
	}
}