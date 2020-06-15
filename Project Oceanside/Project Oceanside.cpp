#include <time.h>
#include <chrono>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	//JP
	Scene* scene = new Scene(OoT_N2);
	Heap* heap = new Heap(scene, OoT_N2_START, OoT_N2_LINK_SIZE);

	//US (remember to switch back to the right scene and actors)
	//this is really hardcoded and bad sorry
	//Scene* scene = new Scene(MM_US);
	//Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->LoadInitialRoom(0);
	heap->ChangeRoom(1, 1, nullptr);
	heap->ChangeRoom(0, 1, nullptr);//matches Spectrum Poggers
	heap->PrintHeap(1);

	delete(scene);
	delete(heap);

	return 0;
}
