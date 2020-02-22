#include <fstream>
#include <iostream>

#include "./Scene.h"
#include "./Constants.h"
#include "./Node.h"

Scene::Scene(char version)
{
	ParseSceneJson();
	ParseActorJson(version);
	LoadScene();
}

void Scene::LoadScene()
{
	std::map<int, char> actorCount; //used to assign priority (order in which things are loaded)
	std::cout << "Parsing actors..." << std::endl;

	//load actors, create nodes, create rooms, create node caches
	for (auto room : sceneJson["rooms"])
	{
		Room* newRoom = new Room();
		actorCount.clear();
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

			Node* newActor = new Node(actorID, actorJson[actorIDString], actor, actorCount[actorID]);
			newRoom->AddActor(newActor);
		}

		std::cout << "Room " << roomCount << " completed!" << std::endl;
		rooms.push_back(newRoom);
		roomCount++;
	}

	std::cout << "Parsing actors complete..." << std::endl;
}

void Scene::ParseSceneJson()
{
	std::string sceneFile = "scene.json";
	std::string successMessage = " loaded...";

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
	std::string successMessage = " loaded...";
	std::string actorFile;

	switch (version)
	{
	case MM_JP:
		actorFile = "mm_j_actors.json";
		break;
	case MM_US:
		actorFile = "mm_u_actors.json";
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

bool Scene::GetClockReallocates() const
{
    return clockReallocates;
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
			std::cout << std::hex << "Actor: " << actor->GetID() << " | " << "Priority: " << actor->GetPriority() << std::endl;
		}
		roomNumber++;
	}
}
