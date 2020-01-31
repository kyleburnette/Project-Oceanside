#pragma once
#include "./json.hpp"


#include <fstream>
#include <iostream>
#include <vector>
class ActorList
{
	public:
		ActorList();
		std::vector<std::pair<int,int>> actorList0;
		std::vector<std::pair<int,int>> actorList1;
		int roomLoads;
	private:
		nlohmann::json actorJson;
};

