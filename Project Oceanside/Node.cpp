#include "Node.h"

Node::Node()
{

}

Node::Node(int size, std::string ID, char type, Node* overlay)
{
	this->size = size;
	this->ID = ID;
	this->type = type;
	this->overlay = overlay;
}

Node::Node(int address, int size, Node* prev, Node* next, char type, std::string ID)
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

void Node::SetID(std::string ID)
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

std::string Node::GetID() const
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
