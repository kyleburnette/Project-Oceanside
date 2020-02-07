#include "Heap.h"



Heap::Heap(Scene* scene, int start, int end) : start_address(start), end_address(end), scene(scene)
{
	Node* headNode = new Node(start, LINK_SIZE, nullptr, nullptr, LINK_TYPE, LINK_ID);
	Node* tailNode = new Node(end, LINK_SIZE, nullptr, nullptr, LINK_TYPE, LINK_ID);
	headNode->SetNext(tailNode);
	tailNode->SetPrev(headNode);
	head = headNode;
	tail = tailNode;

	//placeholder and hardcoded
	auto thing = scene->GetRoom(1)->GetAllActors();

	for (auto actor : thing)
	{
		if (actor->GetID() == 0x0006)
		{
			chestOverlay = actor->GetOverlay();
		}
	}

	auto otherThing = scene->GetRoom(0)->GetAllActors();
	
	for (auto actor : otherThing)
	{
		if (actor->GetID() == 0x00B1)
		{
			flowerOverlay = actor->GetOverlay();
			break;
		}
	}

	auto finalThing = scene->GetRoom(0)->GetAllActors();

	for (auto actor : finalThing)
	{
		if (actor->GetID() == 0x00B1)
		{
			allFlowers.push_back(actor);
		}
	}

	currentActorCount[LINK_ID] = 2;

	//fix this later
	possibleTemporaryActors[0x0009] = new Node(0x0009, scene->GetActorJSON()["0009"], 0); //Bomb
	possibleTemporaryActors[0x00A2] = new Node(0x00A2, scene->GetActorJSON()["00A2"], 0); //Smoke
	possibleTemporaryActors[0x003D] = new Node(0x003D, scene->GetActorJSON()["003D"], 0); //HookShot
	possibleTemporaryActors[0x017B] = new Node(0x017B, scene->GetActorJSON()["017B"], 0); //Bugs
	possibleTemporaryActors[0x000F] = new Node(0x000F, scene->GetActorJSON()["000F"], 0); //Arrow
	possibleTemporaryActors[0x0035] = new Node(0x0035, scene->GetActorJSON()["0035"], 0); //Spin Attack1
	possibleTemporaryActors[0x007B] = new Node(0x007B, scene->GetActorJSON()["007B"], 0); //Spin Attack 2
	possibleTemporaryActors[0x006A] = new Node(0x006A, scene->GetActorJSON()["006A"], 0); //Chu
	possibleTemporaryActors[0x018c] = new Node(0x018C, scene->GetActorJSON()["018C"], 0); //ISoT
	possibleTemporaryActors[0xF001] = new Node(0xF001, scene->GetActorJSON()["F001"], 0); //ISoT Memory Leak
	possibleTemporaryActors[0xF002] = new Node(0xF002, scene->GetActorJSON()["F002"], 0); //SC Memory Leak
	

	possibleRandomAllocatableActorsRoom1[0] = 0x0009;
	//possibleRandomAllocatableActorsRoom1[1] = 0x006A;
	//possibleRandomAllocatableActorsRoom1[2] = 0x000F;

	possibleRandomAllocatableActorsRoom0[0] = 0x0009;
	//possibleRandomAllocatableActorsRoom0[1] = 0x006A;
	//possibleRandomAllocatableActorsRoom1[2] = 0x000F;
};

Heap::~Heap()
{
	DeleteHeap();
}

void Heap::FreezeRocksAndGrass() 
{
	for (auto rockOrGrass : rocksAndGrass)
	{
		std::tuple<int, int, int> thingToInsert;
		std::get<0>(thingToInsert) = rockOrGrass->GetID();
		std::get<1>(thingToInsert) = rockOrGrass->GetAddress();
		std::get<2>(thingToInsert) = rockOrGrass->GetPriority();
		frozenRocksAndGrass.push_back(thingToInsert);
	}
}

void Heap::AllocateTemporaryActor(int actorID)
{
	//TODO - implement ISoT leak and scarecrow leak and arrow animation thing

	Node* newTempActor = new Node(*possibleTemporaryActors[actorID]);
	
	temporaryActors.push_back(newTempActor);
	switch (actorID) {
	/*case 0x00A2: 
	{
		Node* t = new Node(*possibleTemporaryActors[0x0009]);
		Allocate(t);
		Allocate(newTempActor);
		Deallocate(t);
		delete t;
	}
		break;
		*/
	case 0x18C:
	{
		Allocate(newTempActor);
		Node* newLeak = new Node(*possibleTemporaryActors[0xF001]);
		Allocate(newLeak);
		leaks.push_back(newLeak);
		Deallocate(newTempActor);
	}
		break;
	/*case 0x0035:
	{

		Allocate(newTempActor);
		AllocateTemporaryActor(0x007B);
		
	}
		break;*/

	default:
		Allocate(newTempActor);
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

	if (!node->GetOffspring().empty())
	{
		for (Node* offspring : node->GetOffspring())
		{
			offspringToAllocate.push_back(offspring);
		}
	}
}

void Heap::LoadRoom(int roomNumber)
{
	/*HARDCODING THIS SPECIFICALLY FOR NIGHT 1 GRAVEYARD FOR NOW!*/

	if (currentRoomNumber != -1)
	{
		std::cerr << "This function can only be used before a room is initially loaded.";
		return;
	}

	Room* room = scene->GetRoom(roomNumber);

	char stalchildCount = 0;
	std::vector<Node*> extraStalchildren;
	//load room's actors
	for (Node* actor : room->GetAllActors())
	{
		if (actor->GetID() == 0x0212 && stalchildCount < 1)
		{
			room->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			stalchildCount++;
		}
		else if (actor->GetID() == 0x212 && stalchildCount >= 1)
		{
			extraStalchildren.push_back(actor);
		}
		else if (actor->GetID() == 0x00ED)
		{
			room->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			deallocatableActors.push_back(actor);
		}
		else if (actor->GetID() == 0x0265)
		{
			room->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			deallocatableActors.push_back(actor);
		} 
		else if (actor->GetID() == 0x0082)
		{
			room->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			deallocatableActors.push_back(actor);
		}
		else
		{
			room->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
		}
		actor->SetRemoved(0);
	}

	for (Node* stalchild : extraStalchildren)
	{
		room->AddCurrentlyLoadedActor(stalchild);
		Allocate(stalchild);
	}

	//allocate spawner offspring actors
	for (Node* offspring : offspringToAllocate)
	{
		room->AddCurrentlyLoadedActor(offspring);
		rocksAndGrass.push_back(offspring);
		Allocate(offspring);
	}


	offspringToAllocate.clear();
	//If we have a Scarecrow After room load allocate Leaks
	for (Node* hunt : room->GetCurrentlyLoadedActors()) {
		if (hunt->GetID() == 0x00CA) {
			Node* newLeak1 = new Node(*possibleTemporaryActors[0xF002]);
			Node* newLeak2 = new Node(*possibleTemporaryActors[0xF002]);
			Allocate(newLeak1);
			Allocate(newLeak2);
			leaks.push_back(newLeak1);
			leaks.push_back(newLeak2);

		}
	}

	this->initiallyLoadedRoomNumber = roomNumber;
	this->currentRoomNumber = roomNumber;
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
	Node* wall = nullptr;

	deallocatableActors.clear();

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
		else if (actor->GetID() == 0x028D)
		{
			wall = actor;
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);

		}
		else if (actor->GetID() == 0x0018)
		{
			; //TODO - handle not reallocating loading planes later
		}

		else if (actor->GetID() == 0x0265 || actor->GetID() == 0x00ED || actor->GetID() == 0x0082 )
		{
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			deallocatableActors.push_back(actor);
		}
		/* Allocate Leaks on scarecrow load during room change
		 * Scarecrow is loaded into execution immeditily Load leaks soon as actor is instantiated 
		 */
		
		else if (actor->GetID() == 0x0CA) {  
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
			Node* newLeak1 = new Node(*possibleTemporaryActors[0xF002]);
			Node* newLeak2 = new Node(*possibleTemporaryActors[0xF002]);
			Allocate(newLeak1);
			Allocate(newLeak2);
			leaks.push_back(newLeak1);
			leaks.push_back(newLeak2);
		}
		/*else if (actor->GetID() == 0x005F)
		{
			
				newRoom->AddCurrentlyLoadedActor(actor);
				Allocate(actor);
				deallocatableActors.push_back(actor);
			
		}*/
		else
		{
			newRoom->AddCurrentlyLoadedActor(actor);
			Allocate(actor);
		}
	}


	//deallocate new clock 
	if (newClock != nullptr)
	{
		Deallocate(newClock);
		delete(newClock);
		newClock = nullptr;
	}

	//deallocate new dampe 
	if (newDampe != nullptr)
	{
		Deallocate(newDampe);
		delete(newDampe);
		newDampe = nullptr;
	}

	for (Node* actor : newRoom->GetAllActors()) {
		if (actor->GetRemoved()) {
			Deallocate(actor);
			deallocatableActors.erase(std::remove(deallocatableActors.begin(), deallocatableActors.end(),actor), deallocatableActors.end());
			newRoom->RemoveCurrentlyLoadedActor(actor);
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

	rocksAndGrass.clear();

	//allocate spawner stuff
	for (Node* offspring : offspringToAllocate)
	{
		newRoom->AddCurrentlyLoadedActor(offspring);
		Allocate(offspring);
		deallocatableActors.push_back(offspring);
		rocksAndGrass.push_back(offspring);
	}

	offspringToAllocate.clear();

	//update room number to room number of room we're changing to

	this->currentRoomNumber = newRoomNumber;

	if (wall != nullptr)
	{
		Deallocate(wall);
	}
}

std::pair<int, int> Heap::DeallocateRandomActor()
{
	if (deallocatableActors.empty())
	{
		std::pair<int, int> yep;
		yep.first = 0;
		yep.second = 0;
		return yep;
	}

	char rng = rand() % deallocatableActors.size();

	Node* nodeToDeallocate = deallocatableActors[rng];
	/*if (nodeToDeallocate->GetID() == 0x005F) {
		nodeToDeallocate->SetRemoved(1);
	}*/
	Deallocate(deallocatableActors[rng]);
	//std::cout << std::hex << "Deallocated random actor: " << deallocatableActors[rng]->GetID() << std::endl;
	deallocatableActors.erase(std::remove(deallocatableActors.begin(), deallocatableActors.end(), nodeToDeallocate), deallocatableActors.end());

	std::pair<int, int> pair;
	pair.first = nodeToDeallocate->GetID();
	pair.second = nodeToDeallocate->GetPriority();
	return pair;
}

int Heap::AllocateRandomActor()
{
	if (currentRoomNumber == 1)
	{
		int rng = rand() % possibleRandomAllocatableActorsRoom1.size();
		//	std::cout << std::hex << "Allocated random actor: " << possibleRandomAllocatableActors[rng] << std::endl;
		AllocateTemporaryActor(possibleRandomAllocatableActorsRoom1[rng]);

		return possibleRandomAllocatableActorsRoom1[rng];
	}

	else if (currentRoomNumber == 0)
	{
		int rng = rand() % possibleRandomAllocatableActorsRoom0.size();
		//	std::cout << std::hex << "Allocated random actor: " << possibleRandomAllocatableActors[rng] << std::endl;
		AllocateTemporaryActor(possibleRandomAllocatableActorsRoom0[rng]);

		return possibleRandomAllocatableActorsRoom0[rng];
	}
	
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
			int gapSize = curr->GetNext()->GetAddress() - curr->GetAddress() - LINK_SIZE;
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

		curr = curr->GetNext();
	}

	//scene->GetRoom(currentRoomNumber)->ResetCurrentlyLoadedActors();
	
	for (auto leak : leaks)
	{
		DeallocateClockAndPlane(leak);
		delete(leak);
		leak = nullptr;
	}

	leaks.clear();
	frozenRocksAndGrass.clear();

	ClearTemporaryActors();
	currentRoomNumber = -1;

}

int Heap::GetRoomNumber() const
{
	return currentRoomNumber;
}
