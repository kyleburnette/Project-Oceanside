#include "Scene.h"

Scene::Scene()
{
	using json = nlohmann::json;
	json sceneJson;

    //read in the scene data
    try
    {
        std::ifstream f("scene.json");
        sceneJson = json::parse(f);
		std::cout << "Scene data loaded..." << std::endl;
    }
    catch (json::parse_error & e)
    {
        // output exception information
        std::cout << "message: " << e.what() << '\n'
            << "exception id: " << e.id << '\n'
            << "byte position of error: " << e.byte << std::endl;
    }

	//read in the actor data
	try
	{
		std::ifstream f("mm_u_actors.json");
		actorJson = json::parse(f);
		std::cout << "Actor data loaded..." << std::endl;
	}
	catch (json::parse_error & e)
	{
		// output exception information
		std::cout << "message: " << e.what() << '\n'
			<< "exception id: " << e.id << '\n'
			<< "byte position of error: " << e.byte << std::endl;
	}

    //set scene variables
    this->clockReallocates = sceneJson["clockReallocates"];

    std::map<std::string, char> actorCount; //use to assign priority

    //load actors, create nodes, create rooms, create node caches
    for (auto room : sceneJson["rooms"])
    {
        Room* newRoom = new Room();

        for (std::string actor : room["actorList"])
        {
            if (actorCount.count(actor) == 0)
            {
                actorCount[actor] = 0;
            }
            else
            {
                actorCount[actor]++;
            }

			//This is VERY spaghetti right now, TODO - fix
			if (actor == "00B3g")
			{
				if (actorCount.count("00B3") == 0)
				{
					actorCount["00B3"] = 0;
				}
				else
				{
					actorCount["00B3"]++;
				}

				std::string parentString = "00B3";
				std::string offspringString = "0090";

				Node* newActor = new Node(strtol(parentString.c_str(), nullptr, 16), actorJson[parentString], actorCount[parentString]);
				for (int i = 0; i <= 8; ++i)
				{
					Node* newOffspring = new Node(strtol(offspringString.c_str(), nullptr, 16), actorJson["0090"], 100 + i);
					newActor->SetSpawnerOffspring(newOffspring);
				}

				newRoom->AddActor(newActor);
			}

			else if (actor == "00B3r")
			{
				if (actorCount.count("00B3") == 0)
				{
					actorCount["00B3"] = 0;
				}
				else
				{
					actorCount["00B3"]++;
				}

				std::string parentString = "00B3";
				std::string offspringString = "00B0";
				Node* newActor = new Node(strtol(parentString.c_str(), nullptr, 16), actorJson[parentString], actorCount[parentString]);
				for (int i = 0; i < 8; ++i)
				{
					Node* newOffspring = new Node(strtol(offspringString.c_str(), nullptr, 16), actorJson[offspringString], 100 + i);
					newActor->SetSpawnerOffspring(newOffspring);
				}

				newRoom->AddActor(newActor);
			}
             
			else
			{
				Node* newActor = new Node(strtol(actor.c_str(), nullptr, 16), actorJson[actor], actorCount[actor]);
				newRoom->AddActor(newActor);
			}
            
        }

        rooms.push_back(newRoom);
        roomCount++;
    }

    actorCount.clear();
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
