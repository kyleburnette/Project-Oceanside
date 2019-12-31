#pragma once

#include <string>
#include <map>
#include <string>

#include "./json.hpp"

class Node
{
public:
	Node();
	//going to handle this without a factory pattern for now
	Node(std::string actorID, nlohmann::json& actorInfo);
	Node(const Node& copy);
	Node(int size, std::string actorID, char type, Node* overlay);
	Node(int address, int size, Node* prev, Node* next, char type, std::string ID);
	void SetAddress(int address);
	void SetPrev(Node* prev);
	void SetNext(Node* next);
	void SetSize(int size);
	void SetID(std::string);
	int GetAddress() const;
	int GetSize() const;
	Node* GetNext() const;
	Node* GetPrev() const;
	Node* GetOverlay() const;
	std::string GetID() const;
	char GetType() const;
	void SetType(char type);

private:
	Node* next = nullptr;
	Node* prev = nullptr;
	int address = 0;
	int size = 0;
	std::string ID = "x";
	char type = 'x';
	Node* overlay = nullptr;
};



