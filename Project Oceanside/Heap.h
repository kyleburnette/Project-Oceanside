#pragma once

#include "Scene.h"
#include "Node.h"

class Heap
{
public:
	Heap(Scene* scene, int start, int linkSize);
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
	int GetRoomNumber() const;

private:
	Scene* scene;
	Node* head;
	Node* tail;
	int currentRoomNumber = -1;
	int initiallyLoadedRoomNumber = -1;
	const int start_address;
	const int END_ADDRESS = 0x5fffff;
	const int linkSize;
	const int LINK_ID = 0xffff;
	const char LINK_TYPE = 'L';
	const char OVERLAY_TYPE = 'O';

	std::map<int, int> currentActorCount;
	std::vector<Node*> temporaryActors;
	std::vector<Node*> offspringToAllocate;
};

