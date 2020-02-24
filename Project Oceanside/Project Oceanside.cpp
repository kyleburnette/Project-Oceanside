#include <fstream>

#include <chrono>
#include <algorithm>
#include <iostream>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	Scene* scene = new Scene(MM_US);
	Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	heap->LoadInitialRoom(0);
	heap->ChangeRoom(1);
	heap->ChangeRoom(0);

	for (auto actor : scene->GetRoom(0)->GetCurrentlyLoadedActors())
	{
		std::cout << std::hex << actor->GetID() << std::endl;
	}

	delete(scene);
	delete(heap);

	return 0;
}
