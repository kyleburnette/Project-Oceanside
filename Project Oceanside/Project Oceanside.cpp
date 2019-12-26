#include "./Heap.h"
#include "./Scene.h"
#include <chrono>

/*
TODOs:
1. implement allocating new items that aren't part of the room (smoke, chus, arrows, bugs, fish, etc)
2. implement checking system to compare addresses to a desired offset (i.e. 0x160 offset pot and chest)
3. implement permutation generation and running those permutations
    -random? something else?
4.
*/

const int START = 0x40b140;
const int END = 0x5fffff;

int main()
{
    Scene* scene = new Scene();

    Heap* heap = new Heap(scene, START, END);

    //test stuff below
    auto start = std::chrono::high_resolution_clock::now();
    heap->LoadRoom(0);

    for (int i = 1; i < 10000; i++)
    {
        heap->ChangeRoom(i % 2);
    }

    auto end = std::chrono::high_resolution_clock::now();

	heap->PrintHeap();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Total operation of 10000 loads took: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Average room load took: " << duration.count() / 10000.0f << " milliseconds";
    delete(scene);
    delete(heap);

    return 0;
}