#pragma once

#include <iostream>
#include <map>
#include "Scene.h"
#include "Node.h"
#include "Room.h"

class Heap
{
public:
	Heap(Scene* scene, int start, int end);
	~Heap();
	void Allocate(Node* node);
	void AllocateTemporaryActor(Node* node);
	void Deallocate(Node* node);
	void LoadRoom(int roomNumber);
	void UnloadRoom(Room* room);
	void ChangeRoom(int newRoomNumber);
	void PrintHeap() const;
	void DeleteHeap();
	Node* FindSuitableGap(Node* newNode) const;
	void PrintHeapInReverse() const;
	void Insert(Node* newNode, Node* oldNode);
	Node* GetHead() const;
	Node* GetTail() const;
	void PrintCurrentActorCount() const;

private:
	Scene* scene;
	Node* head;
	Node* tail;
	int currentRoomNumber = -1;
	const int start_address;
	const int end_address;
	const int LINK_SIZE = 0x10;
	const std::string LINK_ID = "LINK";
	const char LINK_TYPE = 'L';
	const char OVERLAY_TYPE = 'O';
	std::map<std::string, int> currentActorCount;
};

