#include "World.h"
#include "Object3D.h"
#include "Light.h"
#include "PrimitiveFactory.h"
#include "Runnable.h"
#include "Effects.h"
#include "Graphics.h"
#include "xnacollision.h"
#include "Camera.h"
#include "Sky.h"
#include "Terrain.h"
#include "StaticModel.h"
#include "BillboardManager.h"
#include "Input.h"
#include "Util.h"
#include "Effects.h"
#include "WorldLoader.h"
#include <limits>

//! Constructor.
World::World()
{
	mNumVisibleObjects = 0;
}

//! Init.
void World::Init(GLib::Graphics* pGraphics)
{
	// Add test billboards.
	//mLightBillboard = GetGraphics()->AddBillboard(XMFLOAT3(0, 10, 0), XMFLOAT2(5, 5), "textures\\light_icon.png");

	// Create the sky box.
	mSkyBox = new GLib::Sky(pGraphics, "textures/sunsetcube1024.dds", 1000.0f);

	// Create the terrain.
	mTerrain = new GLib::Terrain();

	mWorldLoader = new WorldLoader();
	mWorldLoader->LoadWorld(this, mTerrain, "world.wmap");
}

//! Cleanup the object and light lists.
World::~World()
{
	mWorldLoader->SaveWorld(this, mTerrain, "world.wmap");

	delete mWorldLoader;
	delete mSkyBox;
	delete mTerrain;

	// Delete all objects.
	for(int i = 0; i < mObjectList.size(); i++)
		delete mObjectList[i];

	// Delete all lights.
	for(int i = 0; i < mLightList.size(); i++)
		delete mLightList[i];
}

//! Updates all objects.
void World::Update(float dt)
{
	// Loop through all objects.
	for(int i = 0; i < mObjectList.size(); i++)
	{
		// Update the object if alive.
		if(mObjectList[i]->GetAlive())
			mObjectList[i]->Update(dt);
		else
			RemoveObject(mObjectList[i]);
	}
}
	
//! Draws all objects.
void World::Draw(GLib::Graphics* pGraphics)
{
	// Draw all objects to the shadow map.
	pGraphics->ActiveShadowMap();
	for(int i = 0; i < mObjectList.size(); i++)
		mObjectList[i]->Draw(pGraphics);
	pGraphics->DeactiveShadowMap();

	// Draw the terrain.
	mTerrain->Draw(pGraphics);

	// Draw the skybox.
	//mSkyBox->Draw(pGraphics);

	mNumVisibleObjects = 0;
	for(int i = 0; i < mObjectList.size(); i++)
	{
		// Don't draw the object if it's outside the cameras frustum.
		//Frustum frustum = pGraphics->GetCamera()->GetFrustum();
		//if(XNA::IntersectAxisAlignedBoxFrustum(&mObjectList[i]->GetBoundingBox(), &frustum)) {
			mObjectList[i]->Draw(pGraphics);
			mNumVisibleObjects++;
		//}
	}

	// Draw the lights with a billboard.
	pGraphics->DrawBillboards();
}

//! Returns the selected object, if any.
Object3D* World::GetSelectedObject(GLib::Ray ray)
{
	// Loop through all objects.
	float closestDist = numeric_limits<float>::infinity();
	Object3D* closestObject = nullptr;
	for(int i = 0; i < mObjectList.size(); i++)
	{
		float dist;
		XMFLOAT3 pos = GLib::GetCamera()->GetPosition();
		XMFLOAT3 dir = ray.direction;
		if(XNA::IntersectRayAxisAlignedBox(XMLoadFloat3(&pos), XMLoadFloat3(&dir), &mObjectList[i]->GetBoundingBox(), &dist)) 
		{
			if(mObjectList[i]->RayIntersect(XMLoadFloat3(&pos), XMLoadFloat3(&dir), dist))
			{
				if(dist < closestDist) {
					closestObject = mObjectList[i];
					closestDist = dist;
				}
			}
		}
	}

	return closestObject;
}

//! Adds an object to the object list.
void World::AddObject(Object3D* object)
{
	// [TODO] Set Id.
	static int id = 0;
	object->SetId(id++);
	object->SetWorld(this);
	object->Init();
	mObjectList.push_back(object);
}

//! Adds a light to the light list.
void World::AddLight(GLib::Light* light)
{
	static int id = 0;
	light->SetId(id++);
	mLightList.push_back(light);
}

//! Removes an object from the list.
void World::RemoveObject(Object3D* pObject)
{
	// Loop through all objects and find out which one to delete.
	int i = 0;
	auto itr =  mObjectList.begin();
	while(itr != mObjectList.end() && i < mObjectList.size())
	{
		if(mObjectList[i]->GetId() == pObject->GetId())
		{
			delete mObjectList[i];		
			mObjectList[i] = NULL;
			itr = mObjectList.erase(itr);	
			break;
		}
		else	{
			itr++;
			i++;
		}
	}
}

//! Removes a light from the light list.
void World::RemoveLight(GLib::Light* pLight)
{
	// Loop through all objects and find out which one to delete.
	int i = 0;
	auto itr =  mLightList.begin();
	while(itr != mLightList.end() && i < mLightList.size())
	{
		if(mLightList[i]->GetId() == pLight->GetId())
		{
			delete mLightList[i];		
			mLightList[i] = NULL;
			itr = mLightList.erase(itr);	
			break;
		}
		else	{
			itr++;
			i++;
		}
	}
}

//! Returns the address to the light list.
GLib::LightList* World::GetLights()
{
	return &mLightList;
}

//! Returns the address to the object list.
ObjectList* World::GetObjects()
{
	return &mObjectList;
}

//! Returns a pointer to the terrain.
GLib::Terrain* World::GetTerrain()
{
	return mTerrain;
}

//! Returns the number of visible objects (inside the camera frustum).
int World::GetVisibleObjects()
{
	return mNumVisibleObjects;
}

int World::GetNumLights()
{
	return mLightList.size();
}

XMFLOAT3 World::GetTerrainIntersectPoint(GLib::Ray ray)
{
	return mTerrain->GetIntersectPoint(ray);
}
