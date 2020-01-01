#include "Node.h"

Node::Node()
{

}

//TODO - probably handle this a bit better. MUCH BETTER than before, but still room for improvement
std::map<int, Node*> overlayMap = {

};

Node::Node(int actorID, nlohmann::json& actorInfo, int priority)
{
	std::string instanceSize = actorInfo["instanceSize"];
	this->size = strtol(instanceSize.c_str(), nullptr, 16);
	this->ID = actorID;
	this->type = 'a';
	this->priority = priority;

	//if this actor has an overlay we care about AND its overlay hasn't been created yet
	if (actorInfo["overlayType"] == 0 && overlayMap.count(actorID) == 0)
	{
		std::string overlaySize = actorInfo["overlaySize"];
		Node* newOverlay = new Node(strtol(overlaySize.c_str(), nullptr, 16), actorID, 'O', nullptr);

		this->overlay = newOverlay;
		overlayMap[actorID] = newOverlay;
	}

	//if this actor has an overlay we care about BUT its overlay exists already
	else if (actorInfo["overlayType"] == 0 && overlayMap.count(actorID) != 0)
	{
		this->overlay = overlayMap[actorID];
	}

	//finally, if this actor does not have an overlay we care about
	else
	{
		this->overlay = nullptr;
	}
	
}

Node::Node(const Node& copy)
{
	this->size = copy.GetSize();
	this->ID = copy.GetID();
	this->type = copy.GetType();
	this->overlay = copy.GetOverlay();
}

Node::Node(int size, int ID, char type, Node* overlay)
{
	this->size = size;
	this->ID = ID;
	this->type = type;
	this->overlay = overlay;
}

Node::Node(int address, int size, Node* prev, Node* next, char type, int ID)
	: address(address), size(size), prev(prev), next(next), type(type), ID(ID)
{

};

void Node::SetAddress(int address)
{
	this->address = address;
}

void Node::SetPrev(Node* prev)
{
	this->prev = prev;
}

void Node::SetNext(Node* next)
{
	this->next = next;
}

void Node::SetSize(int size)
{
	this->size = size;
}

void Node::SetID(int ID)
{
	this->ID = ID;
}

int Node::GetAddress() const
{
	return address;
}

int Node::GetSize() const
{
	return size;
}

Node* Node::GetNext() const
{
	return next;
}

Node* Node::GetPrev() const
{
	return prev;
}

int Node::GetID() const
{
	return ID;
}

Node* Node::GetOverlay() const
{
	return overlay;
}

char Node::GetType() const
{
	return type;
}

void Node::SetType(char type)
{
	this->type = type;
}

int Node::GetPriority() const
{
	return priority;
}
