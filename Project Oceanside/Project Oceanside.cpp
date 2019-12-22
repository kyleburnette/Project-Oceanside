#include "./Heap.h"
#include "./Scene.h"

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();

	Heap* heap = new Heap(START, END);
    heap->LoadRoom(scene->GetRoom(0));

	heap->PrintHeap();

    delete(scene);
    delete(heap);

    return 0;
}