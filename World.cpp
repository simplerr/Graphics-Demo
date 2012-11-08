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
#include <limits>

World::World()
{
	mNumVisibleObjects = 0;
}

//! Init.
void World::Init()
{
	// Add test billboards.
	//mLightBillboard = GetGraphics()->AddBillboard(XMFLOAT3(0, 10, 0), XMFLOAT2(5, 5), "textures\\light_icon.png");

	// Create the sky box.
	mSkyBox = new Sky("textures/sky.dds", 5000.0f);

	// Create the terrain.
	mTerrain = new Terrain();
	InitInfo initInfo;
	initInfo.HeightMapFilename = "textures/heightmap17_257.raw";
	initInfo.LayerMapFilename0 = "textures/grass.dds";
	initInfo.LayerMapFilename1 = "textures/darkdirt.dds";
	initInfo.LayerMapFilename2 = "textures/stone.dds";
	initInfo.LayerMapFilename3 = "textures/lightdirt.dds";
	initInfo.LayerMapFilename4 = "textures/snow.dds";
	initInfo.BlendMapFilename = "textures/blend.raw";
	initInfo.HeightScale = 15.0f;
	initInfo.HeightmapWidth = 257;
	initInfo.HeightmapHeight = 257;
	initInfo.CellSpacing = 0.5f;
	mTerrain->Init(gGame->GetGraphics()->GetDevice(), gGame->GetGraphics()->GetContext(), initInfo);
}

//! Cleanup the object and light lists.
World::~World()
{
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
void World::Draw(Graphics* pGraphics)
{
	// Draw all objects to the shadow map.
	pGraphics->ActiveShadowMap();
	//for(int i = 0; i < mObjectList.size(); i++)
	//	mObjectList[i]->Draw(pGraphics);
	pGraphics->DeactiveShadowMap();

	// Draw the terrain.
	mTerrain->Draw(pGraphics);

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
	for(int i = 0; i < mLightList.size(); i++)
	{
		//mLightBillboard->SetPos(mLightList[i]->GetPosition());
		pGraphics->DrawBillboards();
	}

	// Draw the skybox.
	mSkyBox->Draw();
}

//! Returns the selected object, if any.
Object3D* World::GetSelectedObject()
{
	// Loop through all objects.
	float closestDist = numeric_limits<float>::infinity();
	Object3D* closestObject = nullptr;
	for(int i = 0; i < mObjectList.size(); i++)
	{
		float dist;
		XMFLOAT3 pos = gGame->GetGraphics()->GetCamera()->GetPosition();
		XMFLOAT3 dir = gInput->GetWorldPickingRay().direction;
		if(XNA::IntersectRayAxisAlignedBox(XMLoadFloat3(&pos), XMLoadFloat3(&dir), &mObjectList[i]->GetBoundingBox(), &dist)) {
			if(dist < closestDist) {
				closestObject = mObjectList[i];
				closestDist = dist;
			}
		}
	}

	return closestObject;
}

//! Returns the selected light, if any.
Light* World::GetSelectedLight()
{
	// Loop through all lights.
	Light* closestLight = nullptr;
	float closestDist = numeric_limits<float>::infinity();
	for(int i = 0; i < mLightList.size(); i++)
	{
		XMFLOAT3 pos = gGame->GetGraphics()->GetCamera()->GetPosition();
		XMFLOAT3 dir = gInput->GetWorldPickingRay().direction;
		float dist;
		AxisAlignedBox box;
		box.Center = mLightList[i]->GetPosition();
		box.Extents = XMFLOAT3(3, 3, 3);
		if(XNA::IntersectRayAxisAlignedBox(XMLoadFloat3(&pos), XMLoadFloat3(&dir), &box, &dist)) {
			if(dist < closestDist) {
				closestLight = mLightList[i];
				closestDist = dist;
			}
		}
	}
	
	return closestLight;
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
void World::AddLight(Light* light)
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
void World::RemoveLight(Light* pLight)
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
LightList* World::GetLights()
{
	return &mLightList;
}

//! Returns the address to the object list.
ObjectList* World::GetObjects()
{
	return &mObjectList;
}

//! Returns a pointer to the terrain.
Terrain* World::GetTerrain()
{
	return mTerrain;
}

//! Returns the number of visible objects (inside the camera frustum).
int World::GetVisibleObjects()
{
	return mNumVisibleObjects;
}

XMFLOAT3 World::GetTerrainIntersectPoint( Ray ray )
{
	return mTerrain->GetIntersectPoint(ray);
}
