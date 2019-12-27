#include "./Heap.h"
#include "./Scene.h"

/*
TODOs:
1. implement allocating new items that aren't part of the room (smoke, chus, arrows, bugs, fish, etc)
2. implement checking system to compare addresses to a desired offset (i.e. 0x160 offset pot and chest)
3. implement permutation generation and running those permutations
    -random? something else?
*/

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();
    Heap* heap = new Heap(scene, START, END);

    heap->LoadRoom(0);
	heap->PrintHeap();

    delete(scene);
    delete(heap);

    return 0;
}