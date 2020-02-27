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

	heap->Solve(Test);
	
	delete(scene);
	delete(heap);

	return 0;
}
