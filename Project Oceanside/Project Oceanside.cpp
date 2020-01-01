#include <chrono>

#include "./Heap.h"
#include "./Scene.h"

/*
TODOs:
*** maybe make temporary actors (bombs, etc.) go into currentlyLoadedActors instead of their
own vector
1. [DONE] implement allocating new items that aren't part of the room (smoke, chus, arrows, bugs, fish, etc)
2. implement checking system to compare addresses to a desired offset (i.e. 0x160 offset pot and chest)
3. implement permutation generation and running those permutations
    -random? something else?
4. implement ISoT and scarecrow memory leaks
5. implement spawner behavior
6. implement enemies or whatever dying and dropping things (tokens, magic drops, etc.)
*/

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();
    Heap* heap = new Heap(scene, START, END);

	auto start = std::chrono::high_resolution_clock::now();

    heap->LoadRoom(0);
    heap->ChangeRoom(1);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    heap->PrintHeap(1);

    std::cout << "Operation ran in: " << duration.count() << " microseconds" << std::endl;
    //std::cout << "Average room load:  " << duration.count()/ static_cast<float>(roomLoadsToTest) << " milliseconds" << std::endl;

    delete(scene);
    delete(heap);

    return 0;
}