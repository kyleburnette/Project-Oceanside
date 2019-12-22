#include <iostream>
#include <fstream>

#include "./Heap.h"
#include "./Scene.h"
#include "./json.hpp"

const int START = 0x40b140;
const int END = 0x5fffff;

Scene* ParseAndConstructScene()
{
    using json = nlohmann::json;
    json sceneJson;

    //read in the scene data
    try
    {
        std::ifstream f("scene.json");
        sceneJson = json::parse(f);
    }
    catch (json::parse_error & e)
    {
        // output exception information
        std::cout << "message: " << e.what() << '\n'
            << "exception id: " << e.id << '\n'
            << "byte position of error: " << e.byte << std::endl;
        return nullptr;
    }

    Scene* scene = new Scene(sceneJson["clockReallocates"], sceneJson["roomCount"]);
    return scene;

}

int main()
{
    Scene* scene = ParseAndConstructScene();

	Heap* heap = new Heap(START, END);
    std::cout << scene->GetRoomCount() << std::endl;

	heap->PrintHeap();

    delete(scene);
    delete(heap);
    return 0;
}