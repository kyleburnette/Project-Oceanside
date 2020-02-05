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
		std::cout << "Actor Testing List Loaded" << std::endl;

	}
	catch (json::parse_error & e)
	{
		std::cout << "message: " << e.what() << '\n'
			<< "exception id: " << e.id << '\n'
			<< "byte position of error: " << e.byte << std::endl;

	}

	for (auto a : actorJson["actorList0"]) {
		actorList0.push_back(std::make_pair(a[0],a[1]));
	}
	for (auto a : actorJson["actorList1"]) {
		actorList1.push_back(std::make_pair(a[0], a[1]));
	}
	roomLoads = actorJson["roomLoads"];
};
