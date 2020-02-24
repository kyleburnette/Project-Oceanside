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
	Node(int actorID, std::string s_actorID, nlohmann::json& actorInfo, nlohmann::json& actorParameters, int priority);
	Node(int actorID, nlohmann::json& actorInfo, int priority);
	Node(const Node& copy);
	Node(int size, int ID, char type, Node* overlay);
	Node(int address, int size, Node* prev, Node* next, char type, int ID);
	void SetAddress(int address);
	void SetPrev(Node* prev);
	void SetNext(Node* next);
	void SetSize(int size);
	void SetID(int ID);
	int GetAddress() const;
	int GetSize() const;
	Node* GetNext() const;
	Node* GetPrev() const;
	Node* GetOverlay() const;
	int GetID() const;
	char GetType() const;
	void SetType(char type);
	int GetPriority() const;
	void SetSpawnerOffspring(Node* node);
	std::vector<Node*> GetOffspring();
	void SetCleared();

	bool IsDeallocatable() const;
	bool IsClearable() const;
	bool CanStartCleared() const;
	bool StartCleared() const;
	bool ConsiderForSRM() const;
	bool ReallocateOnRoomChange() const;
	char GetNumberOfOffspring() const;
	int GetOffspringActorID() const;
	bool IsSpawner() const;
	bool IsSingleton() const;

private:
	Node* next = nullptr;
	Node* prev = nullptr;
	int address = 0;
	int size = 0;
	int ID = 0xFFFF;
	char type = 'X';
	Node* overlay = nullptr;
	int priority = 0;
	std::vector<Node*> spawnerOffspring;
	bool isClearable;
	bool isDeallocatable;
	bool canStartCleared;
	bool startCleared;
	bool considerForSRM;
	bool reallocateOnRoomChange;
	bool isSingleton;
	int numberOfOffspring;
	int offspringActorID;

	bool cleared;

	bool isSpawner;
};



