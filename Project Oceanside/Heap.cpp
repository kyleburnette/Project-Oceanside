#include <iostream>
#include <iomanip>

#include "Heap.h"
#include "Room.h"

Heap::Heap(Scene* scene, int start, int linkSize) : start_address(start), scene(scene), linkSize(linkSize)
{
	Node* headNode = new Node(start, linkSize, nullptr, nullptr, LINK_TYPE, LINK_ID);
	Node* tailNode = new Node(END_ADDRESS, linkSize, nullptr, nullptr, LINK_TYPE, LINK_ID);
	headNode->SetNext(tailNode);
	tailNode->SetPrev(headNode);
	head = headNode;
	tail = tailNode;

	currentActorCount[LINK_ID] = 2;

	//reimplement temporary actors and possible temp actors per room
};

Heap::~Heap()
{
	DeleteHeap();
}

void Heap::AllocateTemporaryActor(int actorID)
{
	//TODO - implement ISoT leak and scarecrow leak and arrow animation thing

	//reimplement this function with potential randomly allocatable actors from ROOMs
}

void Heap::DeallocateTemporaryActor(int actorID)
{
	for (auto node : temporaryActors)
	{
		if (node->GetID() == actorID)
		{
			temporaryActors.erase(std::remove(temporaryActors.begin(), temporaryActors.end(), node), temporaryActors.end());
			Deallocate(node);
			//std::cout << std::hex << "Deallocated temporary actor: " << node->GetID() << std::endl;
			delete(node);
			node = nullptr;
			return;
		}
	}

	std::cerr << "Actor " << actorID << " is not loaded, deallocation failed." << std::endl;
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
				scene->GetRoom(currentRoomNumber)->AddCurrentlyLoadedActor(node);
				currentActorCount[LINK_ID]++;
			}
		}
	}

	//if actor does not have an overlay that matters (i.e. something allocated far past the part we care about)
	else
	{
		Node* suitableGap = FindSuitableGap(node); 
		Insert(node, suitableGap);
		currentActorCount[node->GetID()]++;

		//check if a new LINK needs to be allocated
		if (node->GetNext()->GetType() == LINK_TYPE && node->GetNext()->GetAddress() - node->GetAddress() > node->GetSize() + linkSize)
		{
			Node* link = new Node(linkSize, LINK_ID, LINK_TYPE, nullptr);
			Insert(link, node);
			currentActorCount[LINK_ID]++;
		}
		
	}
}

void Heap::LoadRoom(int roomNumber)
{
	//LoadRoom can only be used at the beginning of a permutation - currentRoomNumber should NEVER be 
	//anything other than -1 at the start of a permutation.

	if (currentRoomNumber != -1)
	{
		std::cerr << "This function can only be used before a room is initially loaded.";
		return;
	}

	this->initiallyLoadedRoomNumber = roomNumber;
	this->currentRoomNumber = roomNumber;

	Room* room = scene->GetRoom(roomNumber);

	//load all of room's actors
	for (Node* actor : room->GetAllActors())
	{
		Allocate(actor);

		if (actor->IsSpawner())
		{
			for (Node* offspring : actor->GetOffspring())
			{
				offspringToAllocate.push_back(offspring);
			}
		}
	}

	//allocate spawner offspring actors from spawners in this room
	for (Node* offspring : offspringToAllocate)
	{
		Allocate(offspring);
	}

	offspringToAllocate.clear();

	for (Node* clearedActor : room->GetClearedActors())
	{
		Deallocate(clearedActor);
	}

	//reimplement situations where initial scene load is different from subsequent loads of initial room
	//reimplement scarecrow (and other) memory leaks
}

void Heap::ChangeRoom(int newRoomNumber)
{
	if (newRoomNumber == currentRoomNumber)
	{
		std::cerr << "Room number {" << newRoomNumber << "} is already loaded!" << std::endl;
		return;
	}

	else if (currentRoomNumber == -1)
	{
		std::cerr << "ChangeRoom(int newRoomNumber) can only be used if a room is already loaded." << std::endl;
		return;
	}

	Room* oldRoom = scene->GetRoom(currentRoomNumber);
	Room* newRoom = scene->GetRoom(newRoomNumber);
	Node* newClock = nullptr;
	Node* newDampe = nullptr;

	//allocate new room first
	for (Node* actor : newRoom->GetAllActors())
	{
		if (actor->GetID() == 0x015A && !scene->GetClockReallocates())
		{
			; //we do not want to allocate the new clock if it does not reallocate in this scene OR if it does reallocate and we're
				//going back into the first room that was loaded
		}

		else if (actor->GetID() == 0x015A && scene->GetClockReallocates())
		{
			newClock = new Node(*actor);
			Allocate(newClock);
		}
		else if (actor->GetID() == 0x01CA)
		{
			newDampe = new Node(*actor);
			Allocate(newDampe);
		}

		else if (actor->GetID() == 0x0018)
		{
			; //TODO - handle not reallocating loading planes later
		}

		else if (actor->GetID() == 0x0265 || actor->GetID() == 0x00ED || actor->GetID() == 0x0082 )
		{
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
		}
		
		//reimplement leaks (including scarecrow leak)

		else
		{
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
		}
	}
	
	//deallocate temporary actors from old room (bombs, bugs, etc.) and reset temp actor vector
	for (Node* actor : temporaryActors)
	{
		Deallocate(actor);
	}

	ClearTemporaryActors();

	//deallocate old room's base/default actors
	for (Node* actor : oldRoom->GetCurrentlyLoadedActors())
	{
		switch (actor->GetID()) {
		case 0x15A:  //Clock
			if (scene->GetClockReallocates() || !scene->GetClockReallocates())
			{
				break; //we do not want to allocate the new clock if it does not reallocate in this scene	 
			}
			else if (newRoomNumber != initiallyLoadedRoomNumber)
			{
				break; //we do not want to allocate the new clock if it does not reallocate in this scene
			}
			
		case 0x01CA: //Dampe
			if(newRoomNumber != initiallyLoadedRoomNumber)
			{
				break;
			}
		case 0x0018: //Loading plane
			break;
		case 0xF001: //ISot Memory Leak
			break;
		case 0xF002: //SC Memory Leak 
			break;
		default:

			Deallocate(actor);
		}
	}

	//clear old room's currently loaded actors (since they in fact are not currently loaded anymore)
	oldRoom->GetCurrentlyLoadedActors().clear();

	//allocate spawner stuff
	for (Node* offspring : offspringToAllocate)
	{
		newRoom->AddCurrentlyLoadedActor(offspring);
		Allocate(offspring);
	}

	offspringToAllocate.clear();



	//update room number to room number of room we're changing to

	this->currentRoomNumber = newRoomNumber;
}

std::pair<int, int> Heap::DeallocateRandomActor()
{
	return std::make_pair(0, 0);
	//REIMPLEMENT WITH DEALLOCATABLE ACTORS BEING HANDLED BY ROOM, NOT HEAP!
}

int Heap::AllocateRandomActor()
{
	return 0;
	//REIMPLEMENT WITH RANDOM ALLOCATABLE ACTORS BEING HANDLED BY ROOM, NOT HEAP!
}

void Heap::UnloadRoom(int roomNumber)
{
	if (roomNumber != currentRoomNumber)
	{
		std::cerr << "Room " << roomNumber << " is not loaded, so it cannot be unloaded." << std::endl;
		return;
	}

	for (Node* actor : scene->GetRoom(roomNumber)->GetCurrentlyLoadedActors())
	{
		Deallocate(actor);
	}
	scene->GetRoom(currentRoomNumber)->ResetCurrentlyLoadedActors();
	currentRoomNumber = -1;
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

	scene->GetRoom(currentRoomNumber)->RemoveCurrentlyLoadedActor(node);
}

//fix this later
void Heap::DeallocateClockAndPlane(Node* node)
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
		DeallocateClockAndPlane(node->GetOverlay());
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
					<< std::setw(6) << curr->GetSize() << " "
					<< std::setw(1) << curr->GetType() << " "
					<< std::setw(6) << curr->GetID() << " "
					<< std::setw(2) << std::dec << curr->GetPriority()
					<< std::endl;
			}
			
		}
		else if (setting == 1)
		{
			std::cout << std::hex << curr->GetAddress() << ":" 
				<< std::setw(6) << curr->GetSize() << " "
				<< std::setw(1) << curr->GetType() << " " 
				<< std::setw(6) << curr->GetID() << " "
				<< std::setw(2) << std::dec << curr->GetPriority()
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

void Heap::ResetHeap()
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

}

int Heap::GetRoomNumber() const
{
	return currentRoomNumber;
}
