#include <fstream>
#include <iostream>

#include "./Scene.h"
#include "./Constants.h"
#include "./Node.h"

Scene::Scene(char version, const std::string& sceneFile)
{
	ParseSceneJson(sceneFile);
	ParseActorJson(version);
	LoadScene();
}

void Scene::LoadScene()
{
	std::map<int, char> actorCount; //used to assign priority (order in which things are loaded)
	std::cout << "Parsing actors" << std::endl;

	//load actors, create nodes, create rooms, create node caches
	for (auto room : sceneJson["rooms"])
	{
		Room* newRoom = new Room(roomCount);
		actorCount.clear();
		for (auto randAllocActor : room["possibleAllocatableActors"].items())
		{
			std::string actorIDString = randAllocActor.key();
			int actorID = strtol(actorIDString.c_str(), nullptr, 16);
			Node* newActor = new Node(actorID, actorJson[actorIDString], 0);
			newRoom->AddRandomAllocatableActor(randAllocActor.value(), newActor);
		}

		for (auto actor : room["actorList"])
		{
			std::string actorIDString = actor["actorID"];
			int actorID = strtol(actorIDString.c_str(), nullptr, 16);

			if (actorCount.count(actorID) == 0)
			{
				actorCount[actorID] = 0;
			}
			else
			{
				actorCount[actorID]++;
			}

			Node* newActor = new Node(actorID, actorIDString, actorJson, actor, actorCount[actorID]);
			newRoom->AddActor(newActor);

			//we need to add scene level transition actors to the scene transition actor map
			if (newActor->IsTransitionActor())
			{
				//but only if there isn't a copy in the map with this ID already
				//this prevents adding corresponding actors to the map more than once (i.e. from more than 1 room)
				if (transitionActors.find(newActor->GetSceneTransitionID()) == transitionActors.end())
				{
					//TODO - this shouldn't leak since this only runs once, but clean up at end of program
					Node* transitionActorSceneLevelCopy = new Node(*newActor);
					transitionActors[newActor->GetSceneTransitionID()] = transitionActorSceneLevelCopy;
				}
			}
		}

		std::cout << "Room " << roomCount << " completed!" << std::endl;
		rooms.push_back(newRoom);
		roomCount++;
	}

	std::cout << "Parsing actors complete" << std::endl;
}

void Scene::ParseSceneJson(const std::string& filename)
{
	std::string sceneFile = filename;
	std::string successMessage = " loaded";

	try
	{
		std::ifstream f(sceneFile);
		sceneJson = nlohmann::json::parse(f);
		std::cout << sceneFile << successMessage << std::endl;
	}
	catch (nlohmann::json::parse_error& e)
	{
		OutputExceptionInformation(e);
	}
}

void Scene::ParseActorJson(char version)
{
	std::string successMessage = " loaded";
	std::string actorFile;

	switch (version)
	{
	case MM_JP:
		actorFile = "mm_j_actors.json";
		break;
	case MM_US:
		actorFile = "mm_u_actors.json";
		break;
	case MM_JP_GC:
		actorFile = "mm_j_gc_actors.json";
		break;
	default:
		std::cerr << "Invalid version" << std::endl;
		break;
	}

	//read in the actor data
	try
	{
		std::ifstream f(actorFile);
		actorJson = nlohmann::json::parse(f);
		std::cout << actorFile << successMessage << std::endl;
	}
	catch (nlohmann::json::parse_error& e)
	{
		OutputExceptionInformation(e);
	}
}

void Scene::OutputExceptionInformation(nlohmann::json::parse_error& error)
{
	// output exception information
	std::cout << "message: " << error.what() << '\n'
		<< "exception id: " << error.id << '\n'
		<< "byte position of error: " << error.byte << std::endl;
}

Room* Scene::GetRoom(int roomNumber) const
{
    return rooms[roomNumber];
}

nlohmann::json Scene::GetActorJSON() const
{
	return actorJson;
}

void Scene::DumpSceneInfo() const
{
	int roomNumber = 0;
	for (auto room : rooms)
	{
		std::cout << "Room " << roomNumber << std::endl;
		for (auto actor : room->GetAllActors())
		{
			std::cout << std::hex << "Actor: " << actor->GetID();
				if (actor->IsTransitionActor())
				{
					std::cout << " | " << "SceneID: " << actor->GetSceneTransitionID() << std::endl;
				}
				else
				{
					std::cout << " | " << "Priority: " << actor->GetPriority() << std::endl;
				}
		}
		roomNumber++;
	}

	std::cout << "Scene Level Transition Actors:\n";
	std::cout << "Total Transition Actors: " << transitionActors.size() << "\n";
	for (auto actor : transitionActors)
	{
		std::cout << std::hex << "Transition Actor: " << actor.second->GetID() << " | " << "ID: " << actor.second->GetSceneTransitionID() << std::endl;
	}
}

void Scene::ResetClearedActors()
{
	for (auto room : rooms)
	{
		room->ResetClearedActors();
	}
}

std::map<int, Node*> Scene::GetTransitionActors() const
{
	return transitionActors;
}

int Scene::NumberOfTransitionActors() const
{
	return transitionActors.size();
}
