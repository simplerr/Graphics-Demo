#pragma once

#include <vector>
#include <map>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include "d3dUtil.h"
using namespace std;

class Graphics;
class Object3D;
class Light;
class Sky;
class BillboardManager;
class Terrain;
struct BillboardVertex;

//! Contains all objects, lights and the terrain.
class World
{
public:
	World();
	~World();

	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void Init();
	void AddObject(Object3D* object);
	void RemoveObject(Object3D* pObject);
	void AddLight(Light* light);

	LightList*	GetLights();
	ObjectList* GetObjects();
	Terrain*	GetTerrain();
	int			GetVisibleObjects();
	XMFLOAT3	GetTerrainIntersectPoint(Ray ray);
	Object3D*	GetSelectedObject();

	// Callback hookup.
	template <class T>
	void AddItemSelectedListender(void(T::*_callback)(void*, int), T* _object)	{
		OnItemSelected = boost::bind(_callback, _object, _1, _2);
	}
private:
	// Callback.
	boost::function<void(void*, int)>	OnItemSelected;
	Light*		GetSelectedLight();
private:
	ObjectList	mObjectList;
	LightList	mLightList;//LightList mLightList;	
	Sky*		mSkyBox;
	Terrain*	mTerrain;
	int			mNumVisibleObjects;
	BillboardVertex*	mLightBillboard;
};