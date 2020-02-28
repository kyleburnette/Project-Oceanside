#include <iostream>
#include <fstream>
#include <iomanip>

#include "Heap.h"
#include "Room.h"
#include "Constants.h"

Heap::Heap(Scene* scene, int start, int linkSize) : start_address(start), scene(scene), linkSize(linkSize)
{
	Node* headNode = new Node(start, linkSize, nullptr, nullptr, LINK_TYPE, LINK_ID);
	Node* tailNode = new Node(END_ADDRESS, linkSize, nullptr, nullptr, LINK_TYPE, LINK_ID);
	headNode->SetNext(tailNode);
	tailNode->SetPrev(headNode);
	head = headNode;
	tail = tailNode;

	currentActorCount[LINK_ID] = 2;
};

Heap::~Heap()
{
	DeleteHeap();
}

void Heap::AllocateTemporaryActor(int actorID)
{
	//get list of all possible temporary actors in the currently loaded room
	auto possibleTempActors = scene->GetRoom(currentRoomNumber)->GetPossibleTemporaryActors();

	//constuct a copy of this temporary actor from the template given in the above map
	Node* newTempActor = new Node(*possibleTempActors[actorID].second);

	switch (actorID) {
	case 0x00A2:
		AllocateTemporaryActor(0x0009);
		Allocate(newTempActor);
		temporaryActors.push_back(newTempActor);
		DeallocateTemporaryActor(0x0009);
		break;
	case 0x18C:
	{
		Allocate(newTempActor);

		Node* isotLeak = new Node(0x300, 0x018C, 'E', nullptr);
		Allocate(isotLeak);
		leaks.push_back(isotLeak);

		Deallocate(newTempActor);
		delete(newTempActor);
		newTempActor = nullptr;
	}
	break;
	case 0x0035:
	{
		Allocate(newTempActor);
		temporaryActors.push_back(newTempActor);
		AllocateTemporaryActor(0x007B);
	}
	break;

	default:
		Allocate(newTempActor);
		temporaryActors.push_back(newTempActor);
		break;
	}
}

void Heap::DeallocateTemporaryActor(int actorID)
{
	for (auto node : temporaryActors)
	{
		if (node->GetID() == actorID)
		{
			temporaryActors.erase(std::remove(temporaryActors.begin(), temporaryActors.end(), node), temporaryActors.end());
			Deallocate(node);
			delete(node);
			node = nullptr;
			return;
		}
	}

	std::cerr << "Actor " << actorID << " is not loaded." << std::endl;
}

void Heap::ClearTemporaryActors()
{
	for (Node* actor : temporaryActors)
	{
		delete(actor);
		actor = nullptr;
	}

	temporaryActors.clear();
}

void Heap::Allocate(Node* node)
{
	//TODO - remove some repeated code

	//if actor has an overlay we care about and the overlay isn't already loaded
	if (node->GetOverlay() != nullptr && currentActorCount[node->GetID()] == 0)
	{
		Node* overlay = node->GetOverlay();
		Node* suitableGap = FindSuitableGap(overlay);
		if (suitableGap != nullptr)
		{
			Insert(overlay, suitableGap);
			Node* link = new Node(linkSize, LINK_ID, LINK_TYPE, nullptr);
			Insert(link, overlay);
			currentActorCount[LINK_ID]++;

			Node* actorGap = FindSuitableGap(node);

			//insert new actor and update count
			Insert(node, actorGap);
			currentActorCount[node->GetID()]++;

			//handle this better 
			if (node->GetNext()->GetType() == LINK_TYPE && node->GetNext()->GetAddress() - node->GetAddress() > node->GetSize() + linkSize)
			{
				Node* actorLink = new Node(linkSize, LINK_ID, LINK_TYPE, nullptr);
				Insert(actorLink, node);

				currentActorCount[LINK_ID]++;
			}
		}
	}

	//if actor does not have an overlay that matters (i.e. something allocated far past the part we care about)
	else
	{
		Node* suitableGap = FindSuitableGap(node);
		Insert(node, suitableGap);
		
		if (node->GetType() == 'A')
		{
			currentActorCount[node->GetID()]++;
		}

		//check if a new LINK needs to be allocated
		if (node->GetNext()->GetType() == LINK_TYPE && node->GetNext()->GetAddress() - node->GetAddress() > node->GetSize() + linkSize)
		{
			Node* link = new Node(linkSize, LINK_ID, LINK_TYPE, nullptr);
			Insert(link, node);
			currentActorCount[LINK_ID]++;
		}
	}

	if (node->IsSpawner())
	{
		for (Node* offspring : node->GetOffspring())
		{
			offspringToAllocate.push_back(offspring);
		}
	}
}

void Heap::LoadInitialRoom(int roomNumber)
{
	this->initiallyLoadedRoomNumber = roomNumber;
	this->currentRoomNumber = roomNumber;
	this->currentRoom = scene->GetRoom(currentRoomNumber);

	Room* newRoom = scene->GetRoom(roomNumber);
	bool scarecrow = false;

	for (Node* actor : newRoom->GetAllActors())
	{
		if (actor->GetID() == 0xCA)
		{
			scarecrow = true;
		}

		if (actor->IsSingleton())
		{
			singletons.push_back(actor);
		}

		Allocate(actor);
		newRoom->AddCurrentlyLoadedActor(actor);
	}

	if (scarecrow)
	{
		Node* leak_1 = new Node(0xB0, 0x00CA, 'E', nullptr);
		Node* leak_2 = new Node(0xB0, 0x00CA, 'E', nullptr);
		Allocate(leak_1);
		Allocate(leak_2);
		leaks.push_back(leak_1);
		leaks.push_back(leak_2);
	}

	DeallocateClearedActors();
	AllocateSpawnerOffspring();

	initialLoad = false;

	//reimplement situations where initial scene load is different from subsequent loads of initial room
}

void Heap::ChangeRoom(int newRoomNumber)
{
	Room* oldRoom = scene->GetRoom(currentRoomNumber);
	Room* newRoom = scene->GetRoom(newRoomNumber);

	this->currentRoomNumber = newRoomNumber;
	this->currentRoom = scene->GetRoom(currentRoomNumber);
	
	AllocateNewRoom(*newRoom);
	UnloadRoom(*oldRoom);

	DeallocateClearedActors();
	AllocateSpawnerOffspring();
	DeallocateReallocatingActors();
}

void Heap::AllocateNewRoom(Room& newRoom)
{
	//allocate new room first
	for (Node* actor : newRoom.GetAllActors())
	{
		if (actor->IsSingleton() && actor->ReallocateOnRoomChange())
		{
			singletonsAttemptingToReallocate.push_back(actor);
			Allocate(actor);
			newRoom.AddCurrentlyLoadedActor(actor);
		}
		else if (!actor->IsSingleton())
		{
			int actorID = actor->GetID();

			Allocate(actor);
			newRoom.AddCurrentlyLoadedActor(actor);

			switch (actorID)
			{
				case 0x00CA:
				{
					Node* leak_1 = new Node(0xB0, 0x00CA, 'E', nullptr);
					Node* leak_2 = new Node(0xB0, 0x00CA, 'E', nullptr);
					Allocate(leak_1);
					Allocate(leak_2);
					leaks.push_back(leak_1);
					leaks.push_back(leak_2);
					break;
				}	
				default:
					break;
			}
		}
	}
}

void Heap::DeallocateClearedActors()
{
	Room* currentRoom = scene->GetRoom(currentRoomNumber);

	for (auto actor : currentRoom->GetClearedActors())
	{
		Deallocate(actor);
		currentRoom->RemoveCurrentlyLoadedActor(actor);
	}
}

void Heap::AllocateSpawnerOffspring()
{
	Room* room = scene->GetRoom(currentRoomNumber);

	for (Node* offspring : offspringToAllocate)
	{
		Allocate(offspring);
		room->AddCurrentlyLoadedActor(offspring);
	}

	offspringToAllocate.clear();
}

std::pair<int, int> Heap::DeallocateRandomActor()
{
	std::vector<Node*> currentDeallocatableActors = scene->GetRoom(currentRoomNumber)->GetDeallocatableActors();

	//if there are no deallocatable actors, we return since we obviously can't deallocate anything
	if (currentDeallocatableActors.empty())
	{
		return std::make_pair(0, 0);
	}

	//dealloc things ~50% of the time (this is probably a 0head way of implementing this, fix later)
	char allocateOrNotRNG = rand() % 2;

	if (allocateOrNotRNG == 1)
	{
		return std::make_pair(0, 0);
	}

	//choose a random actor from the vector
	char rng = rand() % currentDeallocatableActors.size();
	Node* actorToDeallocate = currentDeallocatableActors[rng];

	//deallocate the actor from the heap and handle 
	scene->GetRoom(currentRoomNumber)->DeallocateActor(actorToDeallocate);
	Deallocate(actorToDeallocate);
	
	return std::make_pair(actorToDeallocate->GetID(), actorToDeallocate->GetPriority());
}

int Heap::AllocateRandomActor()
{
	//Chooses a random actor from the entire list of possible random actors
	//for the currently loaded room.

	auto possibleActors = scene->GetRoom(currentRoomNumber)->GetPossibleTemporaryActorsIDs();

	if (possibleActors.size() == 0)
	{
		return 0;
	}

	char rng = rand() % possibleActors.size();

	auto newID = possibleActors[rng];

	if (newID == 0x7B || newID == 0xA2 || newID == 0x3D)
	{
		return 0;
	}

	AllocateTemporaryActor(newID);

	return possibleActors[rng];
}

std::pair<int, int> Heap::ClearRandomActor()
{
	std::vector<Node*> currentClearableActors = scene->GetRoom(currentRoomNumber)->GetClearableActors();

	if (currentClearableActors.size() == 0)
	{
		return std::make_pair(0, 0);
	}

	//clear things ~50% of the time (this is probably a 0head way of implementing this, fix later
	char allocateOrNotRNG = rand() % 2;

	if (allocateOrNotRNG == 0)
	{
		return std::make_pair(0, 0);
	}

	char rng = rand() % currentClearableActors.size();

	Node* clearableActorToDeallocate = currentClearableActors[rng];

	Deallocate(clearableActorToDeallocate);
	currentRoom->RemoveCurrentlyLoadedActor(clearableActorToDeallocate);
	currentRoom->ClearActor(clearableActorToDeallocate);

	return std::make_pair(clearableActorToDeallocate->GetID(), clearableActorToDeallocate->GetPriority());
}

void Heap::UnloadRoom(Room& room)
{
	//deallocate temporary actors from old room (bombs, bugs, etc.) and reset temp actor vector
	for (Node* actor : temporaryActors)
	{
		Deallocate(actor);
	}

	ClearTemporaryActors();

	for (Node* actor : room.GetCurrentlyLoadedActors())
	{
		if (!actor->IsSingleton())
		{
			Deallocate(actor);
			room.RemoveCurrentlyLoadedActor(actor);
		}	
	}

	room.ResetCurrentlyLoadedActors();
	room.ReplenishDeallocatableActors();
}

void Heap::DeallocateReallocatingActors()
{
	for (auto actor : singletonsAttemptingToReallocate)
	{
		Deallocate(actor);
		scene->GetRoom(currentRoomNumber)->RemoveCurrentlyLoadedActor(actor);
	}

	singletonsAttemptingToReallocate.clear();
}

void Heap::Deallocate(int actorID, int priority)
{
	Node* curr = head;
	while (curr != nullptr)
	{
		if (curr->GetID() == actorID && curr->GetType() == 'A' && curr->GetPriority() == priority)
		{
			Deallocate(curr);
			break;
		}

		curr = curr->GetNext();
	}
	
}

void Heap::Deallocate(Node* node)
{
	//first two are for deallocating things at the very beginning of the heap
	if (node->GetPrev()->GetPrev() == nullptr && node->GetNext()->GetNext()->GetType() != LINK_TYPE)
	{
		head->SetNext(node->GetNext());
		node->GetNext()->SetPrev(head);

	}

	else if (node->GetPrev()->GetPrev() == nullptr && node->GetNext()->GetNext()->GetType() == LINK_TYPE)
	{
		head->SetNext(node->GetNext()->GetNext());
		node->GetNext()->GetNext()->SetPrev(head);
		delete(node->GetNext());
		node->SetNext(nullptr);
		currentActorCount[LINK_ID] -= 1;
	}

	//these next two should almost never happen unless the heap is VERY full
	else if (node->GetNext()->GetNext() == nullptr && node->GetPrev()->GetPrev()->GetType() != LINK_TYPE)
	{
		tail->SetPrev(node->GetPrev());
		node->GetPrev()->SetNext(tail);
	}

	else if (node->GetNext()->GetNext() == nullptr && node->GetPrev()->GetPrev()->GetType() == LINK_TYPE)
	{
		tail->SetPrev(node->GetPrev()->GetPrev());
		node->GetPrev()->GetPrev()->SetNext(tail);
	}

	//this handles a situation where there are two nodes in front and two nodes behind
	else if (node->GetNext()->GetNext()->GetType() == LINK_TYPE && node->GetPrev()->GetPrev()->GetType() == LINK_TYPE)
	{
		node->GetPrev()->GetPrev()->SetNext(node->GetNext()->GetNext());
		node->GetNext()->GetNext()->SetPrev(node->GetPrev()->GetPrev());
		delete(node->GetNext());
		node->SetNext(nullptr);
		delete(node->GetPrev());
		node->SetPrev(nullptr);
		currentActorCount[LINK_ID] -= 2;
	}
	else if (node->GetNext()->GetNext()->GetType() == LINK_TYPE)
	{
		node->GetPrev()->SetNext(node->GetNext()->GetNext());
		node->GetNext()->GetNext()->SetPrev(node->GetPrev());
		delete(node->GetNext());
		node->SetNext(nullptr);
		currentActorCount[LINK_ID] -= 1;
	}
	else if (node->GetPrev()->GetPrev()->GetType() == LINK_TYPE)
	{
		node->GetNext()->SetPrev(node->GetPrev()->GetPrev());
		node->GetPrev()->GetPrev()->SetNext(node->GetNext());
		delete(node->GetPrev());
		node->SetPrev(nullptr);
		currentActorCount[LINK_ID] -= 1;
	}

	else
	{
		node->GetPrev()->SetNext(node->GetNext());
		node->GetNext()->SetPrev(node->GetPrev());
	}

	//deallocating the overlay should not decrease number of actors loaded
	if (node->GetType() != OVERLAY_TYPE)
	{
		currentActorCount[node->GetID()]--;
	}

	if (currentActorCount[node->GetID()] == 0 && node->GetOverlay() != nullptr)
	{
		Deallocate(node->GetOverlay());
	}
}

void Heap::Insert(Node* newNode, Node* oldNode)
{
	newNode->SetNext(oldNode->GetNext());
	newNode->SetPrev(oldNode);
	oldNode->GetNext()->SetPrev(newNode);
	newNode->SetAddress(oldNode->GetAddress() + oldNode->GetSize());
	oldNode->SetNext(newNode);
}

void Heap::PrintHeap(char setting) const
{
	Node* curr = head;
	if (!setting) {
		std::cout << "-----HEAP OUTPUT-----" << std::endl;
		
	}
	else {
		std::cout << "-----HEAP OUTPUT With Links-----" << std::endl;;
	}
	while (curr != nullptr)
	{
		if (setting == 0)
		{
			if (curr->GetID() != LINK_ID) 
			{
				std::cout << std::hex << curr->GetAddress() << ":"
					<< std::setw(6) << std::setfill('0') << curr->GetSize() << " "
					<< std::setw(1) << curr->GetType() << " "
					<< std::setw(4) << curr->GetID() << " "
					<< std::setw(1) << std::dec << curr->GetPriority()
					<< std::endl;
			}
			
		}
		else if (setting == 1)
		{
			std::cout << std::hex << curr->GetAddress() << ":"
				<< std::setw(6) << std::setfill('0') << curr->GetSize() << " "
				<< std::setw(1) << curr->GetType() << " "
				<< std::setw(4) << curr->GetID() << " "
				<< std::setw(1) << std::dec << curr->GetPriority()
				<< std::endl;
		}
		

		curr = curr->GetNext();
		
	}
}

void Heap::PrintHeapInReverse() const
{
	Node* curr = tail;
	while (curr != nullptr)
	{
		std::cout << "Address: " << std::hex << "0x" << curr->GetAddress() << " " << curr->GetID() << " " << curr->GetType() << std::dec << std::endl;
		curr = curr->GetPrev();
	}
}

Node* Heap::FindSuitableGap(Node* newNode) const
{
	int size = newNode->GetSize();

	Node* curr = head;
	while (curr->GetNext() != nullptr)
	{
		if (curr->GetID() == LINK_ID && curr->GetNext()->GetID() == LINK_ID)
		{
			int gapSize = curr->GetNext()->GetAddress() - curr->GetAddress() - linkSize;
			if (gapSize >= size)
			{
				return curr;
			}
		}
		curr = curr->GetNext();
	}
	return nullptr;
}

void Heap::DeleteHeap()
{
	Node* curr = head;
	while (curr != nullptr)
	{
		Node* next = curr->GetNext();
		delete(curr);
		curr = next;
	}

	head = nullptr;
}

Node* Heap::GetHead() const
{
	return head;
}

Node* Heap::GetTail() const
{
	return tail;
}

void Heap::PrintCurrentActorCount() const
{
	for (auto i : currentActorCount)
	{
		std::cout << i.first << " " << i.second << std::endl;
	}
}

void Heap::ResetLeaks()
{
	for (auto leak : leaks)
	{
		Deallocate(leak);
		delete(leak);
		leak = nullptr;
	}

	for (auto singleton : singletons)
	{
		Deallocate(singleton);
	}

	singletons.clear();
	leaks.clear();
}

void Heap::ResetHeap()
{
	UnloadRoom(*scene->GetRoom(currentRoomNumber));
	ResetLeaks();
	scene->ResetClearedActors();

	currentRoomNumber = -1;
	currentRoom = nullptr;
}

int Heap::GetRoomNumber() const
{
	return currentRoomNumber;
}

std::vector<std::pair<int, int>> Heap::GetAddressesAndPrioritiesOfType(int actorID, char type)
{
	std::vector<std::pair<int, int>> results;
	Node* curr = head;

	while (curr != nullptr)
	{
		if (curr->GetID() == actorID && curr->GetType() == type)
		{
			results.push_back(std::make_pair(curr->GetAddress(), curr->GetPriority()));
		}

		curr = curr->GetNext();
	}

	return results;
}

void Heap::Solve(int solverType)
{
	//TODO - implement
	switch (solverType)
	{
	case RandomAssortment:
		break;
	case SuccessiveActorSolver:
		break;
	case nop:
		break;
	case Test:
		for (int i = 0; i < 100000; i++)
		{
			LoadInitialRoom(0);
			ClearRandomActor();
			AllocateRandomActor();
			ChangeRoom(1);
			ClearRandomActor();
			AllocateRandomActor();
			ResetHeap();
		}
		PrintHeap(1);
	case DFSRM:
	{
		unsigned int seed = time(NULL);
		srand(seed);

		uint64_t totalPermutations = 0;
		unsigned int totalSolutions = 0;

		std::vector<std::pair<int, int>> solution;
		
		int MAX_ALLOCATIONS_PER_STEP = 5;
		int MAX_SMOKES_PER_STEP = 2;

		std::cout << "Solving..." << std::endl;
		while (true)
		{
			int roomLoads = (2 * (rand() % 5)) + 1;

			LoadInitialRoom(0);
			solution.push_back(std::make_pair(CHANGE_ROOM, 0x0));

			for (int i = 1; i <= roomLoads; i++)
			{
				int MAX_DEALLOCATIONS_PER_STEP = currentRoom->GetDeallocatableActors().size();

				for (int j = 0; j <= MAX_DEALLOCATIONS_PER_STEP; j++)
				{
					solution.push_back(DeallocateRandomActor());
				}

				for (int j = 0; j <= MAX_SMOKES_PER_STEP; j++)
				{
					AllocateTemporaryActor(0xA2);
					solution.push_back(std::make_pair(ALLOCATE, 0xA2));
				}

				//clear actors or not
				for (int j = 0; j <= MAX_ALLOCATIONS_PER_STEP; j++)
				{
					solution.push_back(std::make_pair(ALLOCATE, AllocateRandomActor()));
				}

				char rng = rand() % 3;
				switch (rng)
				{
					case 0:
						break;
					case 1:
						AllocateTemporaryActor(0x3D);
						solution.push_back(std::make_pair(ALLOCATE, 0x3D));
						break;
					case 2:
						AllocateTemporaryActor(0x35);
						solution.push_back(std::make_pair(ALLOCATE, 0x35));
						break;
				}

				ChangeRoom(i % 2);
				solution.push_back(std::make_pair(CHANGE_ROOM, i % 2));
			}

			//we're now standing in chest room

			int chestOverlayAddress = GetAddressesAndPrioritiesOfType(0x6, 'O')[0].first;
			
			std::vector<std::pair<int, int>> rocks = GetAddressesAndPrioritiesOfType(0xB0, 'A');
			std::vector<std::pair<int, int>> grass = GetAddressesAndPrioritiesOfType(0x90, 'A');
			
			AllocateTemporaryActor(0xA2);
			ChangeRoom(0);
			solution.push_back(std::make_pair(SUPERSLIDE, 0));

			int flowerOverlayAddress = GetAddressesAndPrioritiesOfType(0xB1, 'O')[0].first;
			
			if (chestOverlayAddress & 0xFF0000 == flowerOverlayAddress & 0xFF0000)
			{
				std::vector<std::pair<int,int>> flowers = GetAddressesAndPrioritiesOfType(0xB1, 'A');
				std::pair<std::pair<int, int>, std::pair<int, int>> solutionPair;
				bool solutionFound = false;

				for (auto flower : flowers)
				{
					for (auto rock : rocks)
					{
						if (rock.first - flower.first == 0x80)
						{
							solutionPair = std::make_pair(rock, flower);
							solutionFound = true;
						}
					}

					for (auto gras : grass)
					{
						if (gras.first - flower.first == 0x80)
						{
							solutionPair = std::make_pair(gras, flower);
							solutionFound = true;
						}
					}
				}

				if (solutionFound)
				{
					std::cout << "SOLUTION FOUND\n";
					totalSolutions++;

					std::ofstream outputFile;
					std::string outputFilename = "solution" + std::to_string(totalSolutions) + "_seed_" + std::to_string(seed) + ".txt";
					outputFile.open(outputFilename);

					for (auto step : solution)
					{
						if (step.first == CHANGE_ROOM)
						{
							outputFile << std::hex << "Load room: " << step.second << std::endl;
						}
						else if (step.first == ALLOCATE)
						{
							outputFile << std::hex << "Allocate: " << step.second << std::endl;
						}
						else if (step.first == SUPERSLIDE)
						{
							outputFile << std::hex << "Superslide into room " << step.second << " with smoke still loaded." << std::endl;
						}
						else
						{
							outputFile << std::hex << "Deallocate: " << step.first << " | Priority: " << step.second << std::endl;
						}
						
					}

					outputFile.close();
				}


			}

			ResetHeap();
			solution.clear();
			totalPermutations++;

			if (totalPermutations % 100000 == 0)
			{
				std::cout << std::dec << "Total permutations: " << totalPermutations << " | Total Solutions: " << totalSolutions << std::endl;
			}

		}
	}
	default:
		break;
	}
}
