#include <time.h>
#include <chrono>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	/////*JP*/
	Scene* scene = new Scene(MM_JP1, "scene_6.json");
	Heap* heap = new Heap(scene, MM_JP1_START, MM_JP1_LINK_SIZE);

	//////US
	//Scene* scene = new Scene(MM_US, "scene_6.json");
	//Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->SolveGraveyard();

	delete(scene);
	delete(heap);

	return 0;
}
