#include <time.h>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	srand(time(NULL));

	Scene* scene = new Scene(MM_US);
	Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);
	heap->LoadInitialRoom(0);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->ChangeRoom(1, 2);
	heap->ChangeRoom(0, 2);
	heap->AllocateTemporaryActor(0x0009);
	heap->ChangeRoom(1, 2);
	//standing in room 1 waiting to superslide
	heap->Deallocate(0x0082, 1); //second pot
	heap->AllocateTemporaryActor(0x0009);
	heap->AllocateTemporaryActor(0x00A2);
	heap->ChangeRoom(0, 0);
	//standing in room 0, just need to go into room 2 through plane 3 to complete setup
	heap->ChangeRoom(2, 3);

	heap->PrintHeap(1);

	delete(scene);
	delete(heap);

	return 0;
}
