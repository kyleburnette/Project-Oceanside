#include "Room.h"

Room::Room()
{

}

void Room::AddActor(Node* actor)
{
	actors.push_back(actor);
}

std::vector<Node*> Room::GetActors() const
{
	return actors;
}