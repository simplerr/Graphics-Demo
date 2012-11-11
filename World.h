#pragma once
#include <vector>
#include <map>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include "d3dUtil.h"

using namespace std;

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Graphics;
	class Light;
	class Sky;
	class BillboardManager;
	class Terrain;
	struct BillboardVertex;
}

class Object3D;
#pragma endregion

typedef std::vector<Object3D*> ObjectList;

//! Contains all objects, lights and the terrain.
class World
{
public:
	World();
	~World();

	void Init(GLib::Graphics* pGraphics);
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);
	void AddObject(Object3D* object);
	void AddLight(GLib::Light* pLight);
	void RemoveObject(Object3D* pObject);
	void RemoveLight(GLib::Light* pLight);
	

	GLib::LightList*	GetLights();
	ObjectList* GetObjects();
	GLib::Terrain*	GetTerrain();
	int			GetVisibleObjects();
	XMFLOAT3	GetTerrainIntersectPoint(GLib::Ray ray);
	Object3D*	GetSelectedObject(GLib::Ray ray);

	// Callback hookup.
	template <class T>
	void AddItemSelectedListender(void(T::*_callback)(void*, int), T* _object)	{
		OnItemSelected = boost::bind(_callback, _object, _1, _2);
	}
private:
	// Callback.
	boost::function<void(void*, int)>	OnItemSelected;
private:
	ObjectList	mObjectList;
	GLib::LightList	mLightList;//LightList mLightList;	
	GLib::Sky*		mSkyBox;
	GLib::Terrain*	mTerrain;
	int			mNumVisibleObjects;
	GLib::BillboardVertex*	mLightBillboard;
};