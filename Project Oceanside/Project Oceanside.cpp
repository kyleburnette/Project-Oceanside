#include "./Heap.h"
#include "./Scene.h"
#include <chrono>

/*
TODOs:
1. implement allocating new items that aren't part of the room (smoke, chus, arrows, bugs, fish, etc)
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

    heap->LoadRoom(0);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < 100000; i++)
    {
        heap->ChangeRoom(i % 2);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	heap->PrintHeap();

    std::cout << "Operation of 100000 loads ran in: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Average room load:  " << duration.count()/ 100000.0f << " milliseconds" << std::endl;

    delete(scene);
    delete(heap);

    return 0;
}