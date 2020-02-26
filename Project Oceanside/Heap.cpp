#include <iostream>
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
	case 0x18C:
	{
		Allocate(newTempActor);

		Node* isotLeak = new Node(0x300, 0x018C, 'E', nullptr);
		Allocate(isotLeak);
		leaks.push_back(isotLeak);

		Deallocate(newTempActor);
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

	Room* newRoom = scene->GetRoom(roomNumber);

	AllocateNewRoom(*newRoom);

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
	for (auto actor : scene->GetRoom(currentRoomNumber)->GetClearedActors())
	{
		Deallocate(actor);
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
	return std::make_pair(0, 0);
	//REIMPLEMENT WITH DEALLOCATABLE ACTORS BEING HANDLED BY ROOM, NOT HEAP!
}

int Heap::AllocateRandomActor()
{
	//Chooses a random actor from the entire list of possible random actors
	//for the currently loaded room.

	auto possibleActors = scene->GetRoom(currentRoomNumber)->GetPossibleTemporaryActors();

	if (possibleActors.size() == 0)
	{
		return 0;
	}

	char rng = rand() % possibleActors.size();
	AllocateTemporaryActor(possibleActors[rng].first);

	return possibleActors[rng].first;
}

void Heap::UnloadRoom(Room& room)
{
	//deallocate temporary actors from old room (bombs, bugs, etc.) and reset temp actor vector
	for (Node* actor : temporaryActors)
	{
		std::cout << std::hex << actor->GetID() << std::endl;
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

/*void Heap::ResetHeap()
{
	UnloadRoom(currentRoomNumber);

	Node* curr = head;

	//handle this better later
	while (curr != nullptr)
	{
		if (curr->GetID() == 0x15A && curr->GetType() == 'A' || curr->GetID() == 0x0018 || curr->GetID() == 0x1CA && curr->GetType() == 'A')
		{
			DeallocateClockAndPlane(curr);
			curr = head;
		}
		else if (curr->GetID() == 0xF002 || curr->GetID() == 0xF001) {
			DeallocateClockAndPlane(curr);
			delete(curr);
			curr = head;
		}

		curr = curr->GetNext();
	}

	//scene->GetRoom(currentRoomNumber)->ResetCurrentlyLoadedActors();

	ClearTemporaryActors();
	currentRoomNumber = -1;

}*/

int Heap::GetRoomNumber() const
{
	return currentRoomNumber;
}

void Heap::Solve(int solverType)
{
	switch (solverType)
	{
	case RandomAssortment:
		break;
	case SuccessiveActorSolver:
		break;
	case nop:
		break;
	default:
		break;
	}
}
