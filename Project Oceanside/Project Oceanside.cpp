#include "./Heap.h"
#include "./Scene.h"

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();

    Heap* heap = new Heap(START, END);

    //test stuff below
    heap->LoadRoom(scene->GetRoom(0));
    heap->NextRoom(scene, scene->GetRoom(0), scene->GetRoom(1));

	heap->PrintHeap();
    heap->PrintCurrentActorCount();
    delete(scene);
    delete(heap);

    return 0;
}