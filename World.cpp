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
	// [TODO]!!
	//XMFLOAT3 pos = gGame->GetGraphics()->GetCamera()->GetPosition();
	//gGame->GetGraphics()->GetCamera()->SetPosition(XMFLOAT3(pos.x, mTerrain->GetHeight(pos.x, pos.z)+3, pos.z));
	//gGame->GetGraphics()->GetCamera()->SetTarget(gGame->GetGraphics()->GetCamera()->GetPosition() + gGame->GetGraphics()->GetCamera()->GetDirection());

	for(int i = 0; i < mObjectList.size(); i++)
	{
		mObjectList[i]->Update(dt);
	}
}
	
//! Draws all objects.
void World::Draw(Graphics* pGraphics)
{
	// Draw all objects to the shadow map.
	pGraphics->ActiveShadowMap();
	for(int i = 0; i < mObjectList.size(); i++)
		mObjectList[i]->Draw(pGraphics);
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

	//mSkyBox->Draw();
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

//! Returns the number of visible objects (inside the camera frustum).
int World::GetVisibleObjects()
{
	return mNumVisibleObjects;
}