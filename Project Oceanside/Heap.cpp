#include "Heap.h"



Heap::Heap(Scene* scene, int start, int end) : start_address(start), end_address(end), scene(scene)
{
	Node* headNode = new Node(start, LINK_SIZE, nullptr, nullptr, LINK_TYPE, LINK_ID);
	Node* tailNode = new Node(end, LINK_SIZE, nullptr, nullptr, LINK_TYPE, LINK_ID);
	headNode->SetNext(tailNode);
	tailNode->SetPrev(headNode);
	head = headNode;
	tail = tailNode;

	currentActorCount[LINK_ID] = 2;

	//fix this later
	possibleTemporaryActors[0x0009] = new Node(0x0009, scene->GetActorJSON()["0009"], 0);
	possibleTemporaryActors[0x00A2] = new Node(0x00A2, scene->GetActorJSON()["00A2"], 0);
	possibleTemporaryActors[0x003D] = new Node(0x003D, scene->GetActorJSON()["003D"], 0);
	possibleTemporaryActors[0x017B] = new Node(0x017B, scene->GetActorJSON()["017B"], 0);
	possibleTemporaryActors[0x000F] = new Node(0x000F, scene->GetActorJSON()["000F"], 0);
	possibleTemporaryActors[0x0035] = new Node(0x0035, scene->GetActorJSON()["0035"], 0);
	possibleTemporaryActors[0x007B] = new Node(0x007B, scene->GetActorJSON()["007B"], 0);
};

Heap::~Heap()
{
	DeleteHeap();
}

void Heap::AllocateTemporaryActor(int actorID)
{
	//TODO - implement ISoT leak and scarecrow leak and arrow animation thing

	Node* newTempActor = new Node(*possibleTemporaryActors[actorID]);
	temporaryActors.push_back(newTempActor);
	Allocate(newTempActor);
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
			Node* link = new Node(LINK_SIZE, LINK_ID, LINK_TYPE, nullptr);
			Insert(link, overlay);
			currentActorCount[LINK_ID]++;

			Node* actorGap = FindSuitableGap(node);

			//insert new actor and update count
			Insert(node, actorGap);
			currentActorCount[node->GetID()]++;

			//handle this better 
			if (node->GetNext()->GetType() == LINK_TYPE && node->GetNext()->GetAddress() - node->GetAddress() > node->GetSize() + LINK_SIZE) 
			{
				Node* actorLink = new Node(LINK_SIZE, LINK_ID, LINK_TYPE, nullptr);
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
		currentActorCount[node->GetID()]++;

		//check if a new LINK needs to be allocated
		if (node->GetNext()->GetType() == LINK_TYPE && node->GetNext()->GetAddress() - node->GetAddress() > node->GetSize() + LINK_SIZE)
		{
			Node* link = new Node(LINK_SIZE, LINK_ID, LINK_TYPE, nullptr);
			Insert(link, node);
			currentActorCount[LINK_ID]++;
		}
		
	}
}

void Heap::LoadRoom(int roomNumber)
{
	Room* room = scene->GetRoom(roomNumber);
	for (Node* actor : room->GetAllActors())
	{
		room->AddCurrentlyLoadedActor(actor);
		Allocate(actor);
	}
	this->currentRoomNumber = roomNumber;
}

void Heap::ChangeRoom(int newRoomNumber)
{
	if (newRoomNumber == currentRoomNumber)
	{
		std::cerr << "Room number {" << newRoomNumber << "} is already loaded!";
		return;
	}
	Room* oldRoom = scene->GetRoom(currentRoomNumber);
	Room* newRoom = scene->GetRoom(newRoomNumber);

	//allocate new room first
	for (Node* actor : newRoom->GetAllActors())
	{
		if (actor->GetID() == 0x015A && !scene->GetClockReallocates())
		{
			; //we do not want to allocate the new clock if it does not reallocate in this scene
		}
		else if (actor->GetID() == 0x0018)
		{
			; //TODO - handle not reallocating loading plane later
		}
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
		if (actor->GetID() == 0x015A && !scene->GetClockReallocates())
		{
			; //we do not want to allocate the new clock if it does not reallocate in this scene
		}
		else if (actor->GetID() == 0x0018)
		{
			; //TODO - handle not reallocating loading plane later
		}
		else
		{
			Deallocate(actor);
		}
	}

	//clear old room's currently loaded actors (since they in fact are not currently loaded anymore)
	oldRoom->GetCurrentlyLoadedActors().clear();

	//update room number to room number of room we're changing to
	this->currentRoomNumber = newRoomNumber;
}

void Heap::UnloadRoom(Room* room)
{
	for (Node* actor : room->GetCurrentlyLoadedActors())
	{
		Deallocate(actor);
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
		delete(node->GetPrev());
		currentActorCount[LINK_ID] -= 2;
	}
	else if (node->GetNext()->GetNext()->GetType() == LINK_TYPE)
	{
		node->GetPrev()->SetNext(node->GetNext()->GetNext());
		node->GetNext()->GetNext()->SetPrev(node->GetPrev());
		delete(node->GetNext());
		currentActorCount[LINK_ID] -= 1;
	}
	else if (node->GetPrev()->GetPrev()->GetType() == LINK_TYPE)
	{
		node->GetNext()->SetPrev(node->GetPrev()->GetPrev());
		node->GetPrev()->GetPrev()->SetNext(node->GetNext());
		delete(node->GetPrev());
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
	while (curr != nullptr)
	{
		if (setting == 0)
		{
			if (curr->GetID() != LINK_ID) 
			{
				std::cout << std::hex << curr->GetAddress() << ":" << curr->GetSize() << " " << curr->GetType() << " " << curr->GetID() << std::dec << std::endl;
			}
			
		}
		else if (setting == 1)
		{
			std::cout << std::hex << curr->GetAddress() << ":" << curr->GetSize() << " " << curr->GetType() << " " << curr->GetID() << std::dec << std::endl;
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
			int gapSize = curr->GetNext()->GetAddress() - curr->GetAddress();
			if (gapSize > size)
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
	UnloadRoom(scene->GetRoom(currentRoomNumber));

	Node* curr = head;

	//handle this better later
	while (curr->GetNext() != nullptr)
	{
		if (curr->GetID() == 0x15A && curr->GetType() == 'A' || curr->GetID() == 0x0018)
		{
			Deallocate(curr);
		}

		curr = curr->GetNext();
	}

	ClearTemporaryActors();
	currentRoomNumber = -1;

}
