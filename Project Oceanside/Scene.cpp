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
               
            Node* newActor = new Node(actor, actorJson[actor], actorCount[actor]);
            newRoom->AddActor(newActor);

        }

        //newRoom->Memes();
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
