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

	//DOES NOT WORK!
	heap->LoadInitialRoom(1);
	heap->ChangeRoom(0);
	heap->ChangeRoom(1);
	heap->ChangeRoom(0);
	
	scene->GetRoom(heap->GetCurrentRoomNumber())->DumpRoomInfo();
	std::cout << std::endl;
	heap->PrintHeap(1);

	delete(scene);
	//delete(heap);

	return 0;
}
