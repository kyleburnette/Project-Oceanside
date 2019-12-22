#include "Room.h"
#include <iostream>

Room::Room()
{

}

void Room::AddActor(Node* actor)
{
	actors.push_back(actor);
}

std::vector<Node*> Room::GetActors()
{
	return actors;
}

void Room::PrintActors() const
{
	for (auto actor : actors)
	{
		std::cout << actor->GetID() << std::endl;
	}
}