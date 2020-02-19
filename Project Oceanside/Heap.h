#pragma once

#include <iostream>
#include <iomanip>

#include <fstream>
#include <map>
#include <random>
#include <time.h>

#include "Scene.h"
#include "Node.h"
#include "Room.h"

class Heap
{
public:
	Heap(Scene* scene, int start, int end);
	~Heap();
	void Allocate(Node* node);
	void AllocateTemporaryActor(int actorID);
	void Deallocate(int actorID, int priority);
	void Deallocate(Node* node);
	void DeallocateClockAndPlane(Node* node);
	void DeallocateTemporaryActor(int actorID);
	void LoadRoom(int roomNumber);
	void UnloadRoom(int roomNumber);
	void ChangeRoom(int newRoomNumber);
	void PrintHeap(char setting) const;
	void DeleteHeap();
	Node* FindSuitableGap(Node* newNode) const;
	void PrintHeapInReverse() const;
	void Insert(Node* newNode, Node* oldNode);
	Node* GetHead() const;
	Node* GetTail() const;
	void PrintCurrentActorCount() const;
	void ClearTemporaryActors();
	void ResetHeap();
	std::pair<int, int> DeallocateRandomActor();
	int AllocateRandomActor();
	void FreezeRocksAndGrass();
	int GetRoomNumber() const;

	Node* chestOverlay = nullptr;
	Node* flowerOverlay = nullptr;
	std::vector<std::tuple<int, int, int>> frozenRocksAndGrass;
	std::vector<Node*> rocksAndGrass;
	std::vector<Node*> allFlowers;
	std::vector<Node*> deallocatableActors;
	std::map<int, int> possibleRandomAllocatableActorsRoom1;
	std::map<int, int> possibleRandomAllocatableActorsRoom0;

private:
	Scene* scene;
	Node* head;
	Node* tail;
	int currentRoomNumber = -1;
	int initiallyLoadedRoomNumber = -1;
	const int start_address;
	const int end_address;
	const int LINK_SIZE = 0x10;
	const int LINK_ID = 0xffff;
	const char LINK_TYPE = 'L';
	const char OVERLAY_TYPE = 'O';
	std::map<int, int> currentActorCount;
	std::vector<Node*> temporaryActors;
	std::map<int, Node*> possibleTemporaryActors;
	std::vector<Node*> offspringToAllocate;
	
	
};

