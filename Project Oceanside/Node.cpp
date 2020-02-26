#include "Node.h"
#include <iostream>

Node::Node()
{

}

//TODO - probably handle this a bit better. MUCH BETTER than before, but still room for improvement
std::map<int, Node*> overlayMap = {

};

Node::Node(int actorID, std::string s_actorID, nlohmann::json& actorJson, nlohmann::json& actorParameters, int priority)
{
	std::string actorIDString = actorJson[s_actorID]["instanceSize"];
	
	this->size = strtol(actorIDString.c_str(), nullptr, 16);
	this->ID = actorID;
	this->type = 'A';
	this->priority = priority;

	this->isDeallocatable = actorParameters["isDeallocatable"];
	this->isClearable = actorParameters["isClearable"];
	this->canStartCleared = actorParameters["canStartCleared"];
	this->startCleared = actorParameters["startCleared"];
	this->considerForSRM = actorParameters["considerForSRM"];
	this->reallocateOnRoomChange = actorParameters["reallocateOnRoomChange"];
	this->isSingleton = actorParameters["isSingleton"];
	
	if (actorParameters["numberOfOffspring"] > 0)
	{
		this->isSpawner = true;
		this->numberOfOffspring = actorParameters["numberOfOffspring"];
		std::string offspringIDString = actorParameters["offspringActorID"];
		this->offspringActorID = strtol(offspringIDString.c_str(), nullptr, 16);
		for (int i = 0; i < numberOfOffspring; i++)
		{
			Node* newOffspring = new Node(offspringActorID, actorJson[offspringIDString], (100 * (priority + 1)) + i);
			SetSpawnerOffspring(newOffspring);
		}
	}
	else
	{
		this->isSpawner = false;
	}
	
	//if this actor has an overlay we care about AND its overlay hasn't been created yet
	if (actorJson[s_actorID]["overlayType"] == 0 && overlayMap.count(actorID) == 0)
	{
		std::string overlaySize = actorJson[s_actorID]["overlaySize"];
		Node* newOverlay = new Node(strtol(overlaySize.c_str(), nullptr, 16), actorID, 'O', nullptr);

		this->overlay = newOverlay;
		overlayMap[actorID] = newOverlay;
	}

	//if this actor has an overlay we care about BUT its overlay exists already
	else if (actorJson[s_actorID]["overlayType"] == 0 && overlayMap.count(actorID) != 0)
	{
		this->overlay = overlayMap[actorID];
	}

	//finally, if this actor does not have an overlay we care about
	else
	{
		this->overlay = nullptr;
	}
	
}
Node::Node(int actorID, nlohmann::json& actorInfo, int priority)
{
	std::string actorIDString = actorInfo["instanceSize"];
	this->size = strtol(actorIDString.c_str(), nullptr, 16);
	this->ID = actorID;
	this->type = 'A';
	this->priority = priority;
	this->isSpawner = false;

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

	this->isDeallocatable = copy.IsDeallocatable();
	this->isClearable = copy.IsClearable();
	this->canStartCleared = copy.CanStartCleared();
	this->startCleared = copy.StartCleared();
	this->considerForSRM = copy.ConsiderForSRM();
	this->reallocateOnRoomChange = copy.ReallocateOnRoomChange();
	this->isSpawner = copy.IsSpawner();
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

void Node::SetSpawnerOffspring(Node* node)
{
	spawnerOffspring.push_back(node);
}

std::vector<Node*> Node::GetOffspring()
{
	return spawnerOffspring;
}

bool Node::IsDeallocatable() const
{
	return isDeallocatable;
}

bool Node::IsClearable() const
{
	return isClearable;
}

bool Node::CanStartCleared() const
{
	return canStartCleared;
}

bool Node::StartCleared() const
{
	return startCleared;
}

bool Node::ConsiderForSRM() const
{
	return considerForSRM;
}

bool Node::ReallocateOnRoomChange() const
{
	return reallocateOnRoomChange;
}

char Node::GetNumberOfOffspring() const
{
	return numberOfOffspring;
}

int Node::GetOffspringActorID() const
{
	return offspringActorID;
}

bool Node::IsSpawner() const
{
	return isSpawner;
}

void Node::SetCleared(bool clearStatus)
{
	this->cleared = clearStatus;
}

bool Node::IsSingleton() const
{
	return isSingleton;
}
