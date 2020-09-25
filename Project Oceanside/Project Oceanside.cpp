#include <time.h>
#include <chrono>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	///////*JP*/
	//Scene* scene = new Scene(MM_JP, "scene_6.json");
	//Heap* heap = new Heap(scene, MM_JP0_START, MM_JP0_LINK_SIZE);

	////US
	Scene* scene = new Scene(MM_US, "scene_6.json");
	Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->LoadInitialRoom(0);
	heap->ChangeRoom(1, 0, true);
	heap->PrintHeap(0);
	//heap->SolveGraveyard();

	delete(scene);
	delete(heap);

	return 0;
}
