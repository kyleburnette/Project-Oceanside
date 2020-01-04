#include "./Heap.h"
#include "./Scene.h"

/*
FIX - debug mode doesn't work, probably because of my memory leak fix
TODOs:
1. [DONE] implement allocating new items that aren't part of the room (smoke, chus, arrows, bugs, fish, etc)
2. implement checking system to compare addresses to a desired offset (i.e. 0x160 offset pot and chest)
3. implement permutation generation and running those permutations
    -random? something else?
4. implement ISoT and scarecrow memory leaks
5. [DONE] implement spawner behavior
6. implement enemies or whatever dying and dropping things (tokens, magic drops, etc.)
*/

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();
    Heap* heap = new Heap(scene, START, END);

	heap->LoadRoom(0);

	heap->PrintHeap(0);

    delete(scene);
    delete(heap);

    return 0;
}