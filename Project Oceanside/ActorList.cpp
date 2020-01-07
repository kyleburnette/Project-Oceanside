#include "ActorList.h"
ActorList::ActorList()
{
	using json = nlohmann::json;
	json actorJson;
	//Try to read actorFile
	try
	{
		std::ifstream f("actorList.json");
		actorJson = json::parse(f);
		printf("Actor Testing List Loaded");

	}
	catch (json::parse_error & e)
	{
		std::cout << "message: " << e.what() << '\n'
			<< "exception id: " << e.id << '\n'
			<< "byte position of error: " << e.byte << std::endl;

	}

	for (auto a : actorJson["actorList"]) {
		actorList.push_back(std::make_pair(a[0],a[1]));
	}
	roomLoads = actorJson["roomLoads"];
};
