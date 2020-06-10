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
	heap->ChangeRoom(1, 0);
	/*heap->ChangeRoom(0, 0);
	heap->ChangeRoom(1, 2);*/
	//heap->PrintHeap(1);
	//scene->DumpSceneInfo();

	delete(scene);
	delete(heap);

	return 0;
}
