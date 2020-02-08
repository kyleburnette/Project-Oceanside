#include <fstream>

#include <chrono>
#include <algorithm>
#include "windows.h"
#include "./Heap.h"
#include "./Scene.h"
#include "./ActorList.h"

//const int START = 0x40B670; //1.1
const int START = 0x40B3B0; //1.0
const int END = 0x5fffff;

enum SolverTypes {
	KyleSolver,
	KyleSolver2,
	PermSolver,
	nop,
};
uint64_t seed = GetTickCount64();

int main()
{
	srand(seed);
	std::cout << std::to_string(seed) << std::endl;
	std::cout << std::showbase;
	std::cout << std::setfill('0');
	std::cout << std::internal;
	Scene* scene = new Scene();
	Heap* heap = new Heap(scene, START, END);

	int roomLoads = 0;
	int deallocations = 0;
	int allocations = 0;
	uint64_t totalPermutations = 0;
	auto time1 = std::chrono::steady_clock::now();
	unsigned int totalSolutions = 0;
	char rng = 0;
	std::vector<std::pair<int, int>> solution;

	ActorList list;

	/*heap->LoadRoom(1);
	heap->ChangeRoom(0);
	heap->AllocateTemporaryActor(0x0009);
	heap->ChangeRoom(1);
	heap->ChangeRoom(0);
	heap->ChangeRoom(1);
	heap->Deallocate(0x0082, 2);
	heap->Deallocate(0x0082, 1);
	heap->ChangeRoom(0);
	heap->AllocateTemporaryActor(0x0009);
	heap->AllocateTemporaryActor(0x0009);
	heap->ChangeRoom(1);
	heap->ChangeRoom(0);

	heap->PrintHeap(1);*/

	SolverTypes Solver = KyleSolver;
	switch (Solver)
	{
	case KyleSolver2:
	
		while (true)
		{
			roomLoads = (2 * (rand() % list.roomLoads)) + 1;
			heap->LoadRoom(0);
			solution.push_back(std::make_pair(0xcccc, 0x0));

			for (int i = 1; i <= roomLoads; i++) {

				deallocations = rand() % heap->deallocatableActors.size();
				//std::cout << "number of deallocations set" << std::endl;

				for (int j = 0; j < deallocations; j++)
				{
					solution.push_back(heap->DeallocateRandomActor());
				}
				//Lets Generate perms
				if (heap->GetRoomNumber()) {
					for (auto i : list.actorList1)
					{
						rng = (rand() % (i.second + 1));
						for (int c = 0; c < rng; c++) {
							heap->AllocateTemporaryActor(i.first);
							solution.push_back(std::make_pair(0xFFFF, i.first));
						}
					}
				}
				else
				{
					for (auto i : list.actorList0)
					{
						rng = (rand() % (i.second + 1));
						for (int c = 0; c < rng; c++) {
							heap->AllocateTemporaryActor(i.first);
							solution.push_back(std::make_pair(0xFFFF, i.first));
						}
					}
				}
				heap->ChangeRoom(i % 2);
				solution.push_back(std::make_pair(0xcccc, i % 2));
			}

			heap->FreezeRocksAndGrass();
			heap->AllocateTemporaryActor(0x00A2);
			solution.push_back(std::make_pair(0xbbbb, 0xbbbb));
			if (heap->GetRoomNumber()) {
				heap->ChangeRoom(0);
				solution.push_back(std::make_pair(0xcccc, 0));
			}
			heap->ChangeRoom(1);
			solution.push_back(std::make_pair(0xcccc, 1));
			//std::cout << "Loaded room 1." << std::endl;
			//chest overlay will freeze when we change room again
			heap->ChangeRoom(0);
			solution.push_back(std::make_pair(0xcccc, 0));

			for (auto actor : scene->GetRoom(heap->GetRoomNumber())->GetCurrentlyLoadedActors())
			{
				if (actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x5480)
				{

					totalSolutions++;

					std::ofstream outputFile;
					std::string outputFileName = "solution" + std::to_string(totalSolutions) + "_seed_" + std::to_string(seed) + ".txt";
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
						else if (step.first == 0xdddd && step.second == 0x018C)
						{
							outputFile << std::hex << "Leak: ISoT" << std::endl;
						}
						else
						{
							outputFile << std::hex << "Deallocate: " << step.first << " | Priority: " << step.second << std::endl;
						}
					}
					//outputFile << std::hex << " Flower - Priority:" << flower->GetPriority() << std::endl;
					//outputFile << std::hex << "RB - Address: " << std::get<1>(roag) << " Priority: " << std::get<2>(roag) << std::endl;
					std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
					std::cout.rdbuf(outputFile.rdbuf());

					heap->PrintHeap(1);
					std::cout.rdbuf(coutbuf);
					outputFile.close();
				}
			}

				

			heap->ResetHeap();
			//std::cout << "Heap reset." << std::endl;
			solution.clear();
			totalPermutations++;

			if (totalPermutations % 50000 == 0)
			{
				std::chrono::duration<double> time_span = std::chrono::steady_clock::now() - time1;

				std::cout << std::dec << "Total permutations: " << totalPermutations << " | Total Solutions: " << totalSolutions << std::endl;
				std::printf("Permuations Per Second: %f\n", totalPermutations / ((time_span.count())));
			}
		}
		break;
	case KyleSolver:
		while (true)
		{
			roomLoads = (2 * (rand() % 3)) + 1; //max room loads = 5, always odd so we end up in chest room
			//std::cout << "Total number of room loads: " << roomLoads << std::endl;


			//std::cout << "Loaded room 0." << std::endl;
			heap->LoadRoom(1);
			solution.push_back(std::make_pair(0xcccc, 0x1));

			for (int i = 0; i <= roomLoads; i++)
			{
				deallocations = rand() % heap->deallocatableActors.size();
				//std::cout << "number of deallocations set" << std::endl;

				for (int j = 0; j < deallocations; j++)
				{
					solution.push_back(heap->DeallocateRandomActor());
				}

				if (rand() % 1)
				{

					heap->AllocateTemporaryActor(0x18C);

					solution.push_back(std::make_pair(0xdddd, 0x18C)); //ISoT

				}

				if (rand() % 1)
				{
					
					heap->AllocateTemporaryActor(0x00A2);
					
					solution.push_back(std::make_pair(0xdddd, 0x00A2)); //Smoke

				}

				if (rand() % 1)
				{

					heap->AllocateTemporaryActor(0x18C);

					solution.push_back(std::make_pair(0xdddd, 0x18C)); //ISoT

				}


				allocations = rand() % 4;

				for (int k = 0; k < allocations; k++)
				{
					solution.push_back(std::make_pair(0xffff, heap->AllocateRandomActor()));
				}

				if (heap->GetRoomNumber() && rand() % 1) 
				{ //If in room 1 AND futhington is not banned
						heap->AllocateTemporaryActor(0x0035);
						heap->AllocateTemporaryActor(0x007B);
						solution.push_back(std::make_pair(0xffff, 0x0035)); //Spin Attack 1
						solution.push_back(std::make_pair(0xdddd, 0x007B)); //Spin Attack 2

				}

				heap->ChangeRoom(i % 2);
				solution.push_back(std::make_pair(0xcccc, i % 2));
				//std::cout << "loaded room " << i % 2 << std::endl;
			}

			//we're now standing in room 1 waiting to superslide
			std::vector<int> pots;
			for (auto actor : scene->GetRoom(heap->GetRoomNumber())->GetCurrentlyLoadedActors())
			{
				if (actor->GetID() == 0x0082)
				{
					pots.push_back(actor->GetAddress());
				}
			}

			heap->AllocateTemporaryActor(0x0009);
			heap->AllocateTemporaryActor(0x00A2);
			heap->DeallocateTemporaryActor(0x0009);

			solution.push_back(std::make_pair(0xdddd, 0xA2)); 
			solution.push_back(std::make_pair(0xbbbb, 0xbbbb));
			heap->ChangeRoom(0);
			solution.push_back(std::make_pair(0xcccc, 0));

			//we're in room 0 now, but we need to go back into room 1 and then back into room 0 to have a chance of the chest lining up
			
			heap->ChangeRoom(1);
			solution.push_back(std::make_pair(0xcccc, 1));

			heap->ChangeRoom(0);
			solution.push_back(std::make_pair(0xcccc, 0));

			for (auto actor : scene->GetRoom(heap->GetRoomNumber())->GetCurrentlyLoadedActors())
			{
				/*if ((actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x4e40) ||
				(actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x5000) ||
				(actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x5140) ||
				(actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x5280) ||
				(actor->GetID() == 0x0082 && (actor->GetAddress() & 0xFFFF) == 0x5480))*/

				if (actor->GetID() == 0x0006 && actor->GetType() == 'A')
				{
					for (auto entry : pots)
					{
						if (entry - actor->GetAddress() == 0x160)
						{
							//std::cout << std::hex << entry << std::dec << std::endl;
							totalSolutions++;

							std::ofstream outputFile;
							std::string outputFileName = "solution" + std::to_string(totalSolutions) + "_seed_" + std::to_string(seed) + ".txt";
							outputFile.open(outputFileName);
							outputFile << std::hex << entry << std::endl << actor->GetAddress() << std::dec << std::endl;
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
									outputFile << "Pot address: " << actor->GetAddress() << " Priority: " << actor->GetPriority() << std::endl;
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
								else if (step.first == 0xdddd && step.second == 0x018C)
								{
									outputFile << std::hex << "Leak: ISoT" << std::endl;
								}
								else
								{
									outputFile << std::hex << "Deallocate: " << step.first << " | Priority: " << step.second << std::endl;
								}
							}
							std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
							std::cout.rdbuf(outputFile.rdbuf());

							heap->PrintHeap(1);
							std::cout.rdbuf(coutbuf);
							outputFile.close();
						}
					}
					
				}

			}

			heap->ChangeRoom(1);
			heap->ResetHeap();

			//std::cout << "Heap reset." << std::endl;
			solution.clear();
			pots.clear();
			totalPermutations++;

			if (totalPermutations % 50000 == 0)
			{
				std::chrono::duration<double> time_span = std::chrono::steady_clock::now() - time1;

				std::cout << std::dec << "Total permutations: " << totalPermutations << " | Total Solutions: " << totalSolutions << std::endl;
				std::printf("Permuations Per Second: %f\n", totalPermutations / ((time_span.count())));
			}
		}
		case nop:
			break;
	}
	delete(scene);
	delete(heap);

	return 0;
}
