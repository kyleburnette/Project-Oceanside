#include "Node.h"

Node::Node()
{

}

Node::Node(std::string actorID)
{
	//fix this ABSOLUTE DEGENERACY later
	if (actorID == "0018")
	{
		this->size = 0x150;
		this->ID = "0018";
		this->type = 'a';
		this->overlay = nullptr;
	}
	else if (actorID == "02A5")
	{
		this->size = 0x3F0;
		this->ID = "02A5";
		this->type = 'a';
		this->overlay = new Node(0x2660, actorID, 'O', nullptr);
	}
	else if (actorID == "0082")
	{
		this->size = 0x1A0;
		this->ID = "0082";
		this->type = 'a';
		this->overlay = new Node(0x2350, actorID, 'O', nullptr);
	}
	else if (actorID == "0210")
	{
		this->size = 0x250;
		this->ID = "0210";
		this->type = 'a';
		this->overlay = new Node(0xB90, actorID, 'O', nullptr);
	}
	else if (actorID == "0050")
	{
		this->size = 0x4B0;
		this->ID = "0050";
		this->type = 'a';
		this->overlay = new Node(0x3540, actorID, 'O', nullptr);
	}
	else if (actorID == "0125")
	{
		this->size = 0x300;
		this->ID = "0125";
		this->type = 'a';
		this->overlay = new Node(0x1490, actorID, 'O', nullptr);
	}
	else if (actorID == "01E7")
	{
		this->size = 0x150;
		this->ID = "01E7";
		this->type = 'a';
		this->overlay = new Node(0x450, actorID, 'O', nullptr);
	}
	else if (actorID == "01F4")
	{
		this->size = 0x2B0;
		this->ID = "01F4";
		this->type = 'a';
		this->overlay = new Node(0x1EE0, actorID, 'O', nullptr);
	}
	else if (actorID == "020F")
	{
		this->size = 0x290;
		this->ID = "020F";
		this->type = 'a';
		this->overlay = new Node(0x780, actorID, 'O', nullptr);
	}
	else if (actorID == "0024")
	{
		this->size = 0x550;
		this->ID = "0024";
		this->type = 'a';
		this->overlay = new Node(0x28E0, actorID, 'O', nullptr);
	}
	else if (actorID == "0006")
	{
		this->size = 0x230;
		this->ID = "0024";
		this->type = 'a';
		this->overlay = new Node(0x21C0, actorID, 'O', nullptr);
	}
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
