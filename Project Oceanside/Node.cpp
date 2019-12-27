#include "Node.h"

Node::Node()
{

}

//ABSOLUTE DEGENERACY BELOW
//fix all this also TODO
Node* test = new Node(0x2660, "02A5", 'O', nullptr);
Node* test2 = new Node(0x1770, "015A", 'O', nullptr);
Node* test3 = new Node(0x2350, "0082", 'O', nullptr);
Node* test4 = new Node(0xB90, "0210", 'O', nullptr);
Node* test5 = new Node(0x3540, "0050", 'O', nullptr);
Node* test6 = new Node(0x1490, "0125", 'O', nullptr);
Node* test7 = new Node(0x450, "01E7", 'O', nullptr);
Node* test8 = new Node(0x1A80, "01F4", 'O', nullptr);
Node* test9 = new Node(0x780, "020F", 'O', nullptr);
Node* test10 = new Node(0x28E0, "0024", 'O', nullptr);
Node* test11 = new Node(0x21C0, "0006", 'O', nullptr);
Node* test12 = new Node(0x5F80, "00A2", 'O', nullptr);

std::map<const std::string, Node*> overlayMap = {
	{"02A5", test},
	{"015A", test2},
	{"0082", test3},
	{"0210", test4},
	{"0050", test5},
	{"0125", test6},
	{"01E7", test7},
	{"01F4", test8},
	{"020F", test9},
	{"0024", test10},
	{"0006", test11},
	{"00A2", test12}
};

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
	else if (actorID == "0009")
	{
		this->size = 0x210;
		this->ID = "0009";
		this->type = 'a';
		this->overlay = nullptr;
	}

	else if (actorID == "003D")
	{
		this->size = 0x210;
		this->ID = "003D";
		this->type = 'a';
		this->overlay = nullptr;
	}

	else if (actorID == "00A2")
	{
		this->size = 0x2E90;
		this->ID = "00A2";
		this->type = 'a';
		this->overlay = overlayMap["00A2"];
	}

	else if (actorID == "02A5")
	{
		this->size = 0x3F0;
		this->ID = "02A5";
		this->type = 'a';
		this->overlay = overlayMap["02A5"];
	}
	else if (actorID == "0082")
	{
		this->size = 0x1A0;
		this->ID = "0082";
		this->type = 'a';
		this->overlay = overlayMap["0082"];
	}
	else if (actorID == "0210")
	{
		this->size = 0x250;
		this->ID = "0210";
		this->type = 'a';
		this->overlay = overlayMap["0210"];
	}
	else if (actorID == "0050")
	{
		this->size = 0x4B0;
		this->ID = "0050";
		this->type = 'a';
		this->overlay = overlayMap["0050"];
	}
	else if (actorID == "0125")
	{
		this->size = 0x300;
		this->ID = "0125";
		this->type = 'a';
		this->overlay = overlayMap["0125"];
	}
	else if (actorID == "01E7")
	{
		this->size = 0x150;
		this->ID = "01E7";
		this->type = 'a';
		this->overlay = overlayMap["01E7"];
	}
	else if (actorID == "01F4")
	{
		this->size = 0x3D0;
		this->ID = "01F4";
		this->type = 'a';
		this->overlay = overlayMap["01F4"];
	}
	else if (actorID == "020F")
	{
		this->size = 0x290;
		this->ID = "020F";
		this->type = 'a';
		this->overlay = overlayMap["020F"];
	}
	else if (actorID == "0024")
	{
		this->size = 0x550;
		this->ID = "0024";
		this->type = 'a';
		this->overlay = overlayMap["0024"];
	}
	else if (actorID == "0006")
	{
		this->size = 0x230;
		this->ID = "0006";
		this->type = 'a';
		this->overlay = overlayMap["0006"];
	}
	else if (actorID == "0005")
	{
		this->size = 0x1D0;
		this->ID = "0005";
		this->type = 'a';
		this->overlay = nullptr;
	}
	else if (actorID == "015A")
	{
		this->size = 0x160;
		this->ID = "015A";
		this->type = 'a';
		this->overlay = overlayMap["015A"];
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

void Node::SetType(char type)
{
	this->type = type;
}
