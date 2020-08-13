#include <time.h>
#include <chrono>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	/////*JP*/
	Scene* scene = new Scene(MM_JP, "scene_69.json");
	Heap* heap = new Heap(scene, MM_JP1_START, MM_JP1_LINK_SIZE);

	//////US
	//Scene* scene = new Scene(MM_US, "scene_4.json");
	//Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->SolveObservatory();

	delete(scene);
	delete(heap);

	return 0;
}
