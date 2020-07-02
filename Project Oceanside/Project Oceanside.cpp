#include <time.h>
#include <chrono>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	//JP
	//Scene* scene = new Scene(MM_JP);
	//Heap* heap = new Heap(scene, MM_JP1_START, MM_JP1_LINK_SIZE);

	//US
	Scene* scene = new Scene(MM_US, "scene_4.json");
	Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->LoadInitialRoom(1);
	heap->Deallocate(0x82, 0);
	heap->AllocateTemporaryActor(0x9);
	heap->AllocateTemporaryActor(0x9);
	heap->AllocateTemporaryActor(0x9);
	heap->AllocateTemporaryActor(0x3D);
	heap->ChangeRoom(0, 0, nullptr, false);
	heap->Deallocate(0x82, 0);
	heap->Deallocate(0x82, 1);
	heap->Deallocate(0x82, 2);
	heap->Deallocate(0x82, 3);
	heap->AllocateTemporaryActor(0xA2);
	heap->AllocateTemporaryActor(0x9);
	heap->AllocateTemporaryActor(0xF);
	heap->AllocateTemporaryActor(0x9);
	heap->AllocateTemporaryActor(0x35);
	heap->ChangeRoom(1, 0, nullptr, false);
	heap->Deallocate(0x82, 0);
	heap->AllocateTemporaryActor(0xA2);
	heap->ChangeRoom(0, 0, nullptr, false);
	heap->ChangeRoom(1, 0, nullptr, false);
	heap->ChangeRoom(0, 0, nullptr, false);
	heap->PrintHeap(0);

	//heap->Solve();

	delete(scene);
	delete(heap);

	return 0;
}
