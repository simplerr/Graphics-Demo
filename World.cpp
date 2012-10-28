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
#include <limits>

float toolDelta = 0;
float toolDelta2 = 0;
float toolDelta3 = 0;

World::World()
{
	mNumVisibleObjects = 0;
}

void World::Init()
{
	// Add test billboards.
	mLightBillboard = GetGraphics()->AddBillboard(XMFLOAT3(0, 10, 0), XMFLOAT2(5, 5), "textures\\light_icon.png");

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
	initInfo.BlendMapFilename = "textures/blend.dds";
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
	//delete mDebugObject;

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
	bool objectSelected = false;

	// Loop through all objects.
	float closestDist = numeric_limits<float>::infinity();
	Object3D* closestObject = nullptr;
	for(int i = 0; i < mObjectList.size(); i++)
	{
		// Left mouse button pressed?
		if(gInput->KeyPressed(VK_LBUTTON))
		{
			XMFLOAT3 pos = gGame->GetGraphics()->GetCamera()->GetPosition();
			XMFLOAT3 dir = gInput->GetWorldPickingRay().direction;
			float dist;
			if(XNA::IntersectRayAxisAlignedBox(XMLoadFloat3(&pos), XMLoadFloat3(&dir), &mObjectList[i]->GetBoundingBox(), &dist)) 
			{
				if(dist < closestDist) {
					closestObject = mObjectList[i];
					closestDist = dist;
				}
				objectSelected = true;
			}
		}

		mObjectList[i]->Update(dt);
	}
	
	// An object was selected.
	if(closestObject != nullptr) 
		OnObjectSelected(closestObject);

	//
	// [NOTE] An object can be pressed and then a light could be in the ray as well, the closest of the Light and Object3D should be selected.
	//

	// Was a light selected?
	Light* closestLight = nullptr;
	if(gInput->KeyPressed(VK_LBUTTON)) 
	{
		// Loop through all lights.
		closestDist = numeric_limits<float>::infinity();
		closestLight = nullptr;
		for(int i = 0; i < mLightList.size(); i++)
		{
			XMFLOAT3 pos = gGame->GetGraphics()->GetCamera()->GetPosition();
			XMFLOAT3 dir = gInput->GetWorldPickingRay().direction;
			float dist;
			AxisAlignedBox box;
			box.Center = mLightList[i]->GetPosition();
			box.Extents = XMFLOAT3(3, 3, 3);
			if(XNA::IntersectRayAxisAlignedBox(XMLoadFloat3(&pos), XMLoadFloat3(&dir), &box, &dist)) 
			{
				if(dist < closestDist) {
					closestLight = mLightList[i];
					closestDist = dist;
				}
				objectSelected = true;
			}
		}
	}

	// Was the terrain selected?
	// [NOTE] Only check if an object weren't selected.
	if(gInput->KeyPressed(VK_LBUTTON) && !objectSelected)
	{
		XMFLOAT3 intersectPoint = mTerrain->GetIntersectPoint(gInput->GetWorldPickingRay());
		if(intersectPoint.x != numeric_limits<float>::infinity())
			OnTerrainSelected(mTerrain);
	}

	// A light was selected.
	if(closestLight != nullptr)
		OnLightSelected(closestLight);
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

	for(int i = 0; i < mLightList.size(); i++)
	{
		mLightBillboard->SetPos(mLightList[i]->GetPosition());
		pGraphics->DrawBillboards();
	}

	mSkyBox->Draw();
}

//! Adds a object to the object list.
void World::AddObject(Object3D* object)
{
	// [TODO] Set Id.
	mObjectList.push_back(object);
}
	
//! Adds a light to the light list.
void World::AddLight(Light* light)
{
	mLightList.push_back(light);
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