#include <iostream>
#include <fstream>
#include <iomanip>
#include <direct.h>

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

Node* Heap::AllocateTemporaryActor(int actorID)
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

	return newTempActor;
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

	bool stalchildLoaded = false;
	std::vector<Node*> extraStalchildren;

	bool badbat = false;
	std::vector<Node*> extraBats;

	for (Node* actor : newRoom->GetAllActors())
	{
		if (actor->IsTransitionActor())
		{
			Allocate(scene->GetTransitionActors()[actor->GetSceneTransitionID()]);
			//newRoom->AddCurrentlyLoadedActor(actor);
			continue;
		}
		if (actor->GetID() == 0xCA)
		{
			scarecrow = true;
		}

		if (actor->GetID() == 0x212 && stalchildLoaded == false)
		{
			stalchildLoaded = true;
		}

		else if (actor->GetID() == 0x212 && stalchildLoaded == true)
		{
			extraStalchildren.push_back(actor);
			continue;
		}

		if (actor->GetID() == 0x15B && badbat == false)
		{
			badbat = true;
		}

		else if (actor->GetID() == 0x15B && badbat == true)
		{
			extraBats.push_back(actor);
			continue;
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

	if (!extraStalchildren.empty())
	{
		for (auto stalchild : extraStalchildren)
		{
			Allocate(stalchild);
			newRoom->AddCurrentlyLoadedActor(stalchild);
		}
	}


	if (!extraBats.empty())
	{
		for (auto bat : extraBats)
		{
			Allocate(bat);
			newRoom->AddCurrentlyLoadedActor(bat);
		}
	}

	DeallocateClearedActors();
	AllocateSpawnerOffspring();
}

void Heap::ChangeRoom(int newRoomNumber, int transitionActorSceneID, Node* carryActor, bool spawners)
{
	this->carryActor = carryActor;

	if (newRoomNumber == currentRoomNumber)
	{
		std::cerr << "Room number " << newRoomNumber << " is already loaded!\n";
		return;
	}

	Room* oldRoom = scene->GetRoom(currentRoomNumber);
	Room* newRoom = scene->GetRoom(newRoomNumber);

	this->currentRoomNumber = newRoomNumber;
	this->currentRoom = scene->GetRoom(currentRoomNumber);

	if (carryActor)
	{
		AllocateNewRoom(*newRoom, *oldRoom, transitionActorSceneID);
		UnloadRoom(*oldRoom, transitionActorSceneID, carryActor);

		DeallocateClearedActors();
		DeallocateReallocatingActors();
		if (spawners)
		{
			AllocateSpawnerOffspring();
		}
		offspringToAllocate.clear();

		Deallocate(this->carryActor);
		this->carryActor = nullptr;
		ClearTemporaryActors();
	}
	else
	{
		AllocateNewRoom(*newRoom, *oldRoom, transitionActorSceneID);
		UnloadRoom(*oldRoom, transitionActorSceneID, nullptr);

		DeallocateClearedActors();
		DeallocateReallocatingActors();
		if (spawners)
		{
			AllocateSpawnerOffspring();
		}
		offspringToAllocate.clear();
	}

	allocatedExplosiveCount = 0;
}

void Heap::AllocateNewRoom(Room& newRoom, Room& oldRoom, int transitionActorSceneID)
{
	if (transitionActorSceneID > scene->NumberOfTransitionActors() - 1)
	{
		std::cerr << "TransitionActorID not in list of transition actors (most likely a number higher than the number of total transition actors\n";
		return;
	}
	
	//allocate new room first
	for (Node* actor : newRoom.GetAllActors())
	{
		if (actor->IsTransitionActor())
		{
			int currentActorSceneID = actor->GetSceneTransitionID();

			//Is its scene ID the one being used to switch rooms?
			if (currentActorSceneID == transitionActorSceneID)
			{
				/*if yes: do nothing(needs to stay allocated since you're going through it)
				also, since this is the corresponding loading plane in the new room, you need
				to not reallocate it AND you don't need to keep track of it. Do nothing.*/
				;
			}
			else
			{
				//if no:
				//is this scene ID in the last room as well(i.e.shared by the rooms, but you're not going through it?)
				auto transitionActorsOldRoom = oldRoom.GetTransitionActors();
				if (transitionActorsOldRoom.find(currentActorSceneID) != transitionActorsOldRoom.end())
				{
					/*if yes*/
					//hold onto it(don't allocate until end)
					reallocatingTransitionActors.push_back(scene->GetTransitionActors()[currentActorSceneID]);
				}
				else
				{
					/*if no:
					//allocate normally (but allocate the copy held in the scene (this is probably stupid, but...))*/
					Allocate(scene->GetTransitionActors()[currentActorSceneID]);
				}
			}		

			//none of the rest of this applies if this is a transition actor, so continue to next actor in list
			continue;
		}
		if (actor->IsSingleton() && actor->ReallocateOnRoomChange())
		{
			Node* newSingleton = new Node(*actor);
			singletonsAttemptingToReallocate.push_back(newSingleton);
			Allocate(newSingleton);
		}
		else if (!actor->IsSingleton())
		{
			int actorID = actor->GetID();

			//std::cout << std::hex << "Allocating: " << actor->GetID() << std::endl;
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
}

std::pair<int, int> Heap::DeallocateRandomActor()
{
	std::vector<Node*> currentDeallocatableActors = scene->GetRoom(currentRoomNumber)->GetDeallocatableActors();

	//if there are no deallocatable actors, we return since we obviously can't deallocate anything
	if (currentDeallocatableActors.empty())
	{
		return std::make_pair(0, 0);
	}

	/*//dealloc things ~50% of the time (this is probably a 0head way of implementing this, fix later)
	char deallocateOrNotRNG = rand() % 2;

	if (deallocateOrNotRNG == 1)
	{
		return std::make_pair(0, 0);
	}*/

	//choose a random actor from the vector
	char rng = rand() % currentDeallocatableActors.size();
	Node* actorToDeallocate = currentDeallocatableActors[rng];

	//deallocate the actor from the heap and handle 
	Deallocate(actorToDeallocate);
	scene->GetRoom(currentRoomNumber)->DeallocateActor(actorToDeallocate);
	
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

	while ((newID == 0x7B || newID == 0x35 || newID == 0xA2 || newID == 0x3D || newID == 0x20 || newID == 0xDA || newID == 0x0183))
	{
		rng = rand() % possibleActors.size();
		newID = possibleActors[rng];
	}

	if ((newID == 0x0009 || newID == 0x006A) && allocatedExplosiveCount >= MAX_EXPLOSIVES_PER_ROOM)
	{
		return 0;
	}
	else
	{
		AllocateTemporaryActor(newID);
		if (newID == 0x0009 || newID == 0x006A)
		{
			allocatedExplosiveCount++;
		}
	}

	if ((newID == 0x18C) && allocatedISOTCount >= MAX_ISOT)
	{
		return 0;
	}
	else
	{
		AllocateTemporaryActor(newID);
		if (newID == 0x018C)
		{
			allocatedISOTCount++;
		}
	}

	

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

void Heap::UnloadRoom(Room& room, int transitionActorSceneID, Node* carryActor)
{
	//deallocate temporary actors from old room (bombs, bugs, etc.) and reset temp actor vector

	if (carryActor)
	{
		for (Node* actor : temporaryActors)
		{
			if (actor != carryActor)
			{
				Deallocate(actor);
			}
		}
	}

	else
	{
		for (Node* actor : temporaryActors)
		{
			Deallocate(actor);

		}
		ClearTemporaryActors();
	}

	for (auto actor : room.GetTransitionActors())
	{
		if (actor.second->IsTransitionActor() && actor.second->GetSceneTransitionID() != transitionActorSceneID)
		{
			auto actortmp= scene->GetTransitionActors()[actor.first];
			Deallocate(actortmp);
		}
	}

	for (Node* actor : room.GetCurrentlyLoadedActors())
	{
		if (!actor->IsSingleton())
		{
			Deallocate(actor);
			room.RemoveCurrentlyLoadedActor(actor);
		}	
	}

	for (auto actor : reallocatingTransitionActors)
	{
		Allocate(scene->GetTransitionActors()[actor->GetSceneTransitionID()]);
	}

	reallocatingTransitionActors.clear();

	room.ResetCurrentlyLoadedActors();
	room.ReplenishDeallocatableActors();
}

void Heap::DeallocateReallocatingActors()
{
	for (auto singleton : singletonsAttemptingToReallocate)
	{
		Deallocate(singleton);
		delete(singleton);
		singleton = nullptr;
	}

	singletonsAttemptingToReallocate.clear();

	for (auto singleton : singletons)
	{
		scene->GetRoom(currentRoomNumber)->AddCurrentlyLoadedActor(singleton);
	}
}

void Heap::Deallocate(int actorID, int priority)
{
	Node* curr = head;
	while (curr != nullptr)
	{
		if (curr->GetID() == actorID && curr->GetType() == 'A' && curr->GetPriority() == priority)
		{
			Deallocate(curr);
			scene->GetRoom(currentRoomNumber)->RemoveCurrentlyLoadedActor(curr);
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
				<< std::setw(1) << std::dec << curr->GetPriority() << " "
				<< std::setw(2) << std::dec << curr->GetSceneTransitionID() << " "
				<< std::endl;
		}
		

		curr = curr->GetNext();
		
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
	UnloadRoom(*scene->GetRoom(currentRoomNumber), 0, nullptr);
	ResetLeaks();
	scene->ResetClearedActors();
	
	/*sloppy way to deallocate scene-level actors (i.e. transition actors)*/
	Node* curr = head;
	while (curr != nullptr)
	{
		if (curr->IsTransitionActor())
		{
			Deallocate(curr);
		}

		curr = curr->GetNext();
	}

	currentRoomNumber = -1;
	currentRoom = nullptr;
	allocatedExplosiveCount = 0;
	allocatedISOTCount = 0;
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

void Heap::Solve()
{
	unsigned int seed = time(NULL);
	srand(seed);

	uint64_t totalPermutations = 0;
	unsigned int totalSolutions = 0;

	bool endAllocationStep = true;

	std::vector<std::pair<int, int>> solution;

	int MAX_ALLOCATIONS_PER_STEP = 9;

	std::cout << "Seed: " << seed << std::endl;
	std::cout << "Solving..." << std::endl;

	auto newContainerFolder = "C:\\Users\\Kyle\\Desktop\\Heap_Manip_Outputs\\";
	auto newSubFolder = newContainerFolder + std::to_string(seed) + "\\";
	_mkdir(newContainerFolder);
	_mkdir(newSubFolder.c_str());

	while (true)
	{
		int roomLoads = (2 * (rand() % 8)) + 1;

		LoadInitialRoom(0);
		solution.push_back(std::make_pair(LOAD_INITIAL_ROOM, 0));

		for (int i = 1; i <= roomLoads; i++)
		{
			int deallocations = 0;
			int currentRoomDeallocations = currentRoom->GetDeallocatableActors().size();
			if (currentRoomDeallocations)
			{
				 deallocations = rand() % currentRoomDeallocations;
			}
			else
			{
				deallocations = 0;
			}

			for (int j = 0; j <= deallocations; j++)
			{
				solution.push_back(DeallocateRandomActor());
			}

			int allocations = 0;
			if (MAX_ALLOCATIONS_PER_STEP == 0)
			{
				allocations = 0;
			}
			else
			{
				allocations = rand() % MAX_ALLOCATIONS_PER_STEP;
			}
			 
			for (int j = 0; j < allocations; j++)
			{
				solution.push_back(std::make_pair(ALLOCATE, AllocateRandomActor()));
			}
			
			bool spinattack = false;
			if (endAllocationStep)
			{
				char rng = rand() % 2;

				switch (rng)
				{
				case 0:
					break;
				case 1:
					solution.push_back(std::make_pair(ALLOCATE, 0x57));
					spinattack = true;
					break;
				default:
					break;
				}
			}
			
			//actually perform room change using chosen room and plane
			if (spinattack)
			{
				ChangeRoom(i % 2, 0, AllocateTemporaryActor(0x57), false);
			}
			else
			{
				ChangeRoom(i % 2, 0, nullptr, false);
			}
			
			solution.push_back(std::make_pair(CHANGE_ROOM, i));
			solution.push_back(std::make_pair(USE_PLANE, 0));
		}

		//in room 1 right now - let's go to main room with spawner active
		ChangeRoom(0, 0, nullptr, true);

		std::vector<std::pair<int, int>> rocks = GetAddressesAndPrioritiesOfType(0x14E, 'A');

		ChangeRoom(1, 0, nullptr, false);
		ChangeRoom(0, 0, nullptr, false);

		std::vector<std::pair<int, int>> all = GetAllAddresses('A');

		for (auto rock : rocks)
		{
			for (auto thing : all)
			{
				if (rock.first - thing.first == 0x80)
				{
					if (((GetOverlayAddress(0x0141) + 0x2F34) & 0xFF0000) == 0xACA4)
					{
						std::cout << "SUCCESS ";
					}
				}
			}
		}
			
		bool solutionFound = false;
		std::pair<std::pair<int, int>, std::pair<int, int>> solutionPair;



		if (solutionFound)
		{
			std::cout << "SOLUTION FOUND\n";
			totalSolutions++;

			std::ofstream outputFile;
			std::string outputFilename = newSubFolder + "\\solution" + std::to_string(totalSolutions) + "_seed_" + std::to_string(seed) + ".txt";
			outputFile.open(outputFilename);

			outputFile << std::hex << "Pot Address | Priority: " << solutionPair.first.first << " | " << solutionPair.first.second <<
				" Guard Address | Priority: " << solutionPair.second.first << " | " << solutionPair.second.second << std::dec << std::endl;

			for (auto step : solution)
			{
				if (step.first == LOAD_INITIAL_ROOM)
				{
					outputFile << std::hex << "Load initial room: " << step.second << std::endl;
				}
				else if (step.first == CHANGE_ROOM)
				{
					outputFile << std::hex << "Load room: " << step.second;
				}
				else if (step.first == USE_PLANE)
				{
					outputFile << " | Use plane: " << step.second << std::endl;
				}
				else if (step.first == ALLOCATE)
				{
					if (step.second != 0x0)
					{
						outputFile << std::hex << "Allocate: " << step.second << std::endl;
					}	
				}
				else if (step.first == SUPERSLIDE)
				{
					outputFile << std::hex << "Superslide into room " << step.second << " with smoke still loaded using plane 0." << std::endl;
				}
				else if (step.first == 0)
				{
					;
				}
				else
				{
					outputFile << std::hex << "Deallocate: " << step.first << " | Priority: " << step.second << std::endl;
				}

			}
			outputFile.close();
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

int Heap::GetCurrentRoomNumber() const
{
	return currentRoomNumber;
}

std::vector<std::pair<int, int>> Heap::GetAllAddresses(char type)
{
	std::vector<std::pair<int, int>> results;
	Node* curr = head;

	while (curr != nullptr)
	{
		if (curr->GetType() == type)
		{
			results.push_back(std::make_pair(curr->GetAddress(), curr->GetPriority()));
		}

		curr = curr->GetNext();
	}

	return results;
}

int Heap::GetOverlayAddress(int actorID)
{
	int result;
	Node* curr = head;

	while (curr != nullptr)
	{
		if (curr->GetID() == actorID && curr->GetType() == 'O')
		{
			result = curr->GetAddress();
			return result;
		}

		curr = curr->GetNext();
	}

	
}