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
	void DeallocateTemporaryActor(int actorID);
	void LoadInitialRoom(int roomNumber);
	void UnloadRoom(Room& room);
	void ChangeRoom(int newRoomNumber);
	void PrintHeap(char setting) const;
	void PrintHeapInReverse() const;
	void DeleteHeap();
	Node* FindSuitableGap(Node* newNode) const;
	void Insert(Node* newNode, Node* oldNode);
	Node* GetHead() const;
	Node* GetTail() const;
	void PrintCurrentActorCount() const;
	void ClearTemporaryActors();
	void ResetHeap();
	std::pair<int, int> DeallocateRandomActor();
	int AllocateRandomActor();
	int GetRoomNumber() const;
	void Solve(int solverType);

private:
	void AllocateNewRoom(Room& newRoom);
	void DeallocateClearedActors();
	void AllocateSpawnerOffspring();
	void DeallocateReallocatingActors();

	Scene* scene = nullptr;
	Node* head = nullptr;
	Node* tail = nullptr;
	int currentRoomNumber = 0;
	int initiallyLoadedRoomNumber = 0;
	const int start_address = 0x0;
	const int END_ADDRESS = 0x5fffff;
	const int linkSize = 0x0;
	const int LINK_ID = 0xffff;
	const char LINK_TYPE = 'L';
	const char OVERLAY_TYPE = 'O';

	bool initialLoad = true;

	Node* clock = nullptr;

	std::map<int, int> currentActorCount;
	std::vector<Node*> temporaryActors;
	std::vector<Node*> offspringToAllocate;

	std::vector<Node*> leaks;

	std::vector<Node*> singletonsAttemptingToReallocate;
};

