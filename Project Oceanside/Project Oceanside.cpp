#include <fstream>

#include <chrono>
#include <algorithm>

#include "./Constants.h"
#include "./Heap.h"
#include "./Scene.h"

int main()
{
	Scene* scene = new Scene(MM_US);
	Heap* heap = new Heap(scene, MM_US0_START, MM_US0_LINK_SIZE);

	SolverTypes Solver = KyleSolver;
	switch (Solver)
	{
	case KyleSolver:
	scene->DumpSceneInfo();
	heap->LoadRoom(0);
	heap->PrintHeap(1);
	case nop:
		break;
	}
	delete(scene);
	delete(heap);

	return 0;
}
