#pragma once

#include <iostream>
#include <map>
#include "Scene.h"
#include "Node.h"
#include "Room.h"

class Heap
{
public:
	Heap(int start, int end);
	~Heap();
	void Allocate(Node* node);
	void Deallocate(Node* node);
	void LoadRoom(Room* room);
	void UnloadRoom(Room* room);
	void NextRoom(Scene* scene, Room* oldRoom, Room* newRoom);
	void PrintHeap() const;
	void DeleteHeap();
	Node* FindSuitableGap(Node* newNode) const;
	void PrintHeapInReverse() const;
	void Insert(Node* newNode, Node* oldNode);
	Node* GetHead() const;
	Node* GetTail() const;
	void PrintCurrentActorCount() const;

private:
	Node* head;
	Node* tail;
	const int start_address;
	const int end_address;
	const int LINK_SIZE = 0x10;
	const std::string LINK_ID = "LINK";
	const char LINK_TYPE = 'L';
	const char OVERLAY_TYPE = 'O';
	std::map<std::string, int> currentActorCount;
};

