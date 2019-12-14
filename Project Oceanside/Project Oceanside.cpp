#include <iostream>

#include "Heap.h"

int main()
{
	const int START = 0x40b140;
	const int END = 0x5fffff;
	Heap* heap = new Heap(START, END);
	Node* smokeOverlay = new Node(0x5F80, "00A2", 'O', nullptr);
	Node* smoke = new Node(0x2E90, "00A2", 'A', smokeOverlay);
	Node* smoke2 = new Node(0x2E90, "00A2", 'A', smokeOverlay);
	Node* thing = new Node(0x210, "003D", 'A', nullptr);
	Node* otherThing = new Node(0x210, "0069", 'A', nullptr);

	heap->PrintHeap();

	delete(heap);
	heap = nullptr;
}