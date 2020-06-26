#pragma once

#include <vector>

#include "./json.hpp"
#include "./Room.h"

class Scene
{
public:
	Scene(char version, const std::string& sceneFile);
	Room* GetRoom(int roomNumber) const;
	nlohmann::json GetActorJSON() const;
	void DumpSceneInfo() const;
	void ResetClearedActors();
	std::map<int, Node*> GetTransitionActors() const;
	int NumberOfTransitionActors() const;

private:
	void LoadScene();
	void ParseSceneJson(const std::string& filename);
	void ParseActorJson(char version);
	void OutputExceptionInformation(nlohmann::json::parse_error& error);
	int roomCount = 0;
	std::vector<Room*> rooms;
	std::map<int, Node*> transitionActors;
	nlohmann::json actorJson;
	nlohmann::json sceneJson;
};

