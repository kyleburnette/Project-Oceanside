#pragma once
#include "./json.hpp"


#include <fstream>
#include <iostream>
#include <vector>
class ActorList
{
	public:
		ActorList();
		std::vector<std::pair<int,int>> actorList;
		int roomLoads;
	private:
		nlohmann::json actorJson;
};

