#include "Scene.h"

Scene::Scene(bool clockReallocates, int roomCount):
	clockReallocates(clockReallocates), roomCount(roomCount)
{

}

int Scene::GetRoomCount()
{
	return roomCount;
}
