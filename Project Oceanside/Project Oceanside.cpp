#include <chrono>

#include "./Heap.h"
#include "./Scene.h"

/*
TODOs:
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

	int operations = 10000;
	
	std::cout << "Running " << operations << " permutations..." << std::endl;

	for (int i = 0; i < operations; ++i)
	{
		heap->LoadRoom(0);
		heap->Deallocate(0x02A5, 0);
		heap->Deallocate(0x02A5, 3);
		heap->Deallocate(0x0082, 0);
		heap->Deallocate(0x0082, 1);
		heap->Deallocate(0x01E7, 0);
		heap->Deallocate(0x01E7, 1);
		heap->Deallocate(0x01E7, 2);
		heap->AllocateTemporaryActor(0x000F);
		heap->AllocateTemporaryActor(0x000F);
		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x0009);

		heap->ChangeRoom(1);

		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x00A2);
		heap->AllocateTemporaryActor(0x000F);
		heap->AllocateTemporaryActor(0x000F);
		heap->AllocateTemporaryActor(0x000F);
		heap->DeallocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x0035);
		heap->AllocateTemporaryActor(0x007B);

		heap->ChangeRoom(0);

		heap->Deallocate(0x02A5, 0);
		heap->Deallocate(0x02A5, 2);
		heap->Deallocate(0x01E7, 0);
		heap->Deallocate(0x01E7, 1);
		heap->Deallocate(0x01E7, 2);
		heap->Deallocate(0x01E7, 3);
		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x00A2);
		heap->DeallocateTemporaryActor(0x009);
		heap->AllocateTemporaryActor(0x000F);
		heap->AllocateTemporaryActor(0x000F);

		heap->ChangeRoom(1);

		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x00A2);
		heap->DeallocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x003D);

		heap->ChangeRoom(0);

		heap->AllocateTemporaryActor(0x0009);
		heap->AllocateTemporaryActor(0x00A2);
		heap->DeallocateTemporaryActor(0x0009);

		heap->ChangeRoom(1);
		heap->ChangeRoom(0);
		heap->ChangeRoom(1);

		heap->ResetHeap();
	}
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << operations << " Operations ran in: " << duration.count() << " microseconds" << std::endl;
	std::cout << "Time per permutation: " << duration.count() / static_cast<float>(operations) << " microseconds"  << std::endl;
    
    delete(scene);
    delete(heap);

    return 0;
}