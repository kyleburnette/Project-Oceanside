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
	//in room 1
	heap->AllocateTemporaryActor(0x0009);
	heap->Deallocate(0x0082, 1); 
	heap->AllocateTemporaryActor(0x0009);
	
	heap->AllocateTemporaryActor(0x00A2);
	heap->DeallocateTemporaryActor(0x0009);
	heap->ChangeRoom(0, 0);
	heap->ChangeRoom(2, 3);
	
	delete(scene);
	delete(heap);

	return 0;
}
