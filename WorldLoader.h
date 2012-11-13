#pragma once

#include <string>
using namespace std;

namespace GLib {
	class Terrain;
}

class World;
class ModelLoaderXML;

class WorldLoader
{
public:
	WorldLoader();
	~WorldLoader();

	void LoadWorld(World* pWorld, GLib::Terrain* pTerrain, string filename);
	void SaveWorld(World* pWorld, GLib::Terrain* pTerrain, string filename);
private:
	ModelLoaderXML* mModelLoader;
};