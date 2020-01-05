#include <fstream>

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
    srand(time(NULL));
    Scene* scene = new Scene();
    Heap* heap = new Heap(scene, START, END);

    int roomLoads = 0;
    int deallocations = 0;
    int allocations = 0;
    uint64_t totalPermutations = 0;
    unsigned int totalSolutions = 0;
    
    std::vector<std::pair<int, int>> solution;

    while (true)
    {
        roomLoads = (2 * (rand() % 4)) + 1; //max room loads = 9, always odd so we end up in chest room
        //std::cout << "Total number of room loads: " << roomLoads << std::endl;
        char rng = 0;

        //std::cout << "Loaded room 0." << std::endl;
        heap->LoadRoom(0);
        solution.push_back(std::make_pair(0xcccc, 0x0));

        for (int i = 1; i <= roomLoads; i++)
        {
            deallocations = rand() % heap->deallocatableActors.size();
            //std::cout << "number of deallocations set" << std::endl;

            for (int j = 0; j < deallocations; j++)
            {
                solution.push_back(heap->DeallocateRandomActor());
            }

            rng = rand() % 2;

            if (rng == 0 && heap->GetRoomNumber() == 1)
            {
                heap->AllocateTemporaryActor(0x0009);
                heap->AllocateTemporaryActor(0x00A2);
                heap->DeallocateTemporaryActor(0x0009);
                solution.push_back(std::make_pair(0xdddd, 0x00A2));
                //std::cout << "(smoke)" << std::endl;
            }

            allocations = rand() % 4;
                
            for (int k = 0; k < allocations; k++)
            {
                solution.push_back(std::make_pair(0xffff, heap->AllocateRandomActor()));
            }

            rng = rand() % 3;

            if (rng == 0)
            {
                heap->AllocateTemporaryActor(0x003D);
                solution.push_back(std::make_pair(0xdddd, 0x003D));
            }

            else if (rng == 1)
            {
                heap->AllocateTemporaryActor(0x0035);
                heap->AllocateTemporaryActor(0x007B);
                solution.push_back(std::make_pair(0xdddd, 0x007B));
            }

            heap->ChangeRoom(i % 2);
            solution.push_back(std::make_pair(0xcccc, i % 2));
            //std::cout << "loaded room " << i % 2 << std::endl;
        }

        heap->FreezeRocksAndGrass();
        //std::cout << "Rocks and grass frozen. " << std::endl;
        
        heap->AllocateTemporaryActor(0x0009);
        heap->AllocateTemporaryActor(0x00A2);
        heap->DeallocateTemporaryActor(0x0009);
        solution.push_back(std::make_pair(0xbbbb, 0xbbbb));

        //std::cout << "Supersliding..." << std::endl;

        heap->ChangeRoom(0);
        solution.push_back(std::make_pair(0xcccc, 0));
        //std::cout << "Loaded room 0." << std::endl;
        heap->ChangeRoom(1);
        solution.push_back(std::make_pair(0xcccc, 1));
        //std::cout << "Loaded room 1." << std::endl;
        //chest overlay will freeze when we change room again
        heap->ChangeRoom(0);
        solution.push_back(std::make_pair(0xcccc, 0));
       // std::cout << "Loaded room 0." << std::endl;

        //std::cout << std::hex << heap->chestOverlay->GetAddress() << std::endl;
        //std::cout << std::hex << heap->flowerOverlay->GetAddress() << std::endl;

        if ((heap->chestOverlay->GetAddress() & 0xFF0000) == (heap->flowerOverlay->GetAddress() & 0xFF0000))
        {

           //std::cout << std::hex << heap->chestOverlay->GetAddress() << " " << heap->flowerOverlay->GetAddress() << std::dec << std::endl;

           //std::cout << "Overlays match." << std::endl;
           for (auto flower : heap->allFlowers)
           {
               for (auto roag : heap->frozenRocksAndGrass)
               {
                   //std::cout << std::hex << flower->GetAddress() << " " << std::get<1>(roag) << std::endl;
                   if (flower->GetAddress() == std::get<1>(roag))
                   {
                       std::cout << "SOLUTION FOUND" << std::endl;
                       totalSolutions++;

                       std::ofstream outputFile;
                       std::string outputFileName = "solution" + std::to_string(totalSolutions) + ".txt";
                       outputFile.open(outputFileName);

                       for (auto step : solution)
                       {
                           if (step.first == 0xcccc)
                           {
                               outputFile << std::hex << "Load room " << step.second << std::endl;
                           }
                           else if (step.first == 0xffff && step.second != 0xffff)
                           {
                               outputFile << std::hex << "Allocate: " << step.second << std::endl;
                           }
                           else if (step.first == 0xbbbb)
                           {
                               outputFile << "Superslide into room 0." << std::endl;
                           }
                           else if (step.first == 0xdddd && step.second == 0x003D)
                           {
                               outputFile << std::hex << "Allocate: hookshot" << std::endl;
                           }
                           else if (step.first == 0xdddd && step.second == 0x007B)
                           {
                               outputFile << std::hex << "Allocate: charged spin attack" << std::endl;
                           }
                           else if (step.first == 0xdddd && step.second == 0x00A2)
                           {
                               outputFile << std::hex << "Allocate: smoke (let bomb unload)" << std::endl;
                           }
                           else
                           {
                               outputFile << std::hex << "Deallocate: " << step.first << " | Priority: " << step.second << std::endl;
                           }
                       }

                       outputFile.close();
                   }
               }
               
           }
        }

        heap->ResetHeap();
        //std::cout << "Heap reset." << std::endl;
        solution.clear();
        totalPermutations++;
        if (totalPermutations % 50000 == 0)
        {
            std::cout << std::dec << "Total permutations: " << totalPermutations << " | Total Solutions: " << totalSolutions << std::endl;
        }
    }
    
    delete(scene);
    delete(heap);

    return 0;
}