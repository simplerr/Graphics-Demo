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

class World
{
public:
	World();
	~World();

	void Update(float dt);
	void Draw(Graphics* pGraphics);
	void Init();
	void AddObject(Object3D* object);
	void AddLight(Light* light);

	LightList* GetLights();
	ObjectList* GetObjects();
	int GetVisibleObjects();

	// Callback hookups.
	template <class T>
	void AddObjectSelectedListender(void(T::*_callback)(Object3D*), T* _object)	{
		OnObjectSelected = boost::bind(_callback, _object, _1);
	}

	template <class T>
	void AddLightSelectedListender(void(T::*_callback)(Light*), T* _object)	{
		OnLightSelected = boost::bind(_callback, _object, _1);
	}
private:
	// Callbacks.
	boost::function<void(Object3D*)>	OnObjectSelected;
	boost::function<void(Light*)>		OnLightSelected;
private:
	ObjectList	mObjectList;
	LightList	mLightList;//LightList mLightList;	
	Sky*		mSkyBox;
	Terrain*	mTerrain;
	int			mNumVisibleObjects;
	BillboardVertex*	mLightBillboard;
};