#include "TerrainTool.h"
#include "Terrain.h"
#include "Input.h"
#include "Effects.h"
#include "Util.h"

using namespace GLib;

TerrainTool::TerrainTool() : mUpdateInterval(0.03f)
{
	SetTool(TOOL_HEIGHT);
	SetRadius(20.0f);
	SetStrength(1.0f);
	SetSelectedLayer(4);

	Effects::TerrainFX->SetToolCenter(XMFLOAT2(-999999, -999999));
	Effects::TerrainFX->Apply(GetD3DContext());
}

//! Cleanup.
TerrainTool::~TerrainTool()
{

}

//! Poll for input and perform actions.
void TerrainTool::Update(GLib::Input* pInput, float dt)
{
	static float timer = 0;
	timer += dt;

	// Don't use the tools every frame.
	XMFLOAT3 intersectPoint = mTerrain->GetIntersectPoint(pInput->GetWorldPickingRay());
	if(timer > mUpdateInterval)
	{
		// Which tool is selected and was any mouse button pressed?
		if(mCurrentTool == TOOL_HEIGHT)
		{
			if(pInput->KeyDown(VK_LBUTTON) && IsIn3DScreen(pInput))
				ChangeHeight(intersectPoint, true);
			else if(pInput->KeyDown(VK_RBUTTON) && IsIn3DScreen(pInput))
				ChangeHeight(intersectPoint, false);
		}
		else if(mCurrentTool == TOOL_TEXTURE)
		{
			if(pInput->KeyDown(VK_LBUTTON) && IsIn3DScreen(pInput))
				EditTextures(intersectPoint);
		}
		else if(mCurrentTool == TOOL_SMOTH)
		{
			if(pInput->KeyDown(VK_LBUTTON) && IsIn3DScreen(pInput))
				SmothTerrain(intersectPoint);
		}

		// Reset the timer.
		timer = 0.0f;
	}

	// Smooth the terrain if any mouse button is released when TOOL_HEIGHT.
	if(IsIn3DScreen(pInput) && mCurrentTool == TOOL_HEIGHT && (pInput->KeyReleased(VK_LBUTTON) || pInput->KeyReleased(VK_RBUTTON))) {
		mTerrain->Smooth();
		mTerrain->BuildHeightmapSRV(GetD3DDevice());
	}

	// Set the tool center position.
	Effects::TerrainFX->SetToolCenter(XMFLOAT2(intersectPoint.x, intersectPoint.z));
	Effects::TerrainFX->Apply(GetD3DContext());
}
	
//! Draw the terrain tool. [NOTE] Unused.
void TerrainTool::Draw(GLib::Graphics* pGraphics)
{

}

//! Changes the height in a circle around center.
void TerrainTool::ChangeHeight(XMFLOAT3 center, bool raise)
{
	// Did the ray hit the terrain?
	if(center.x != numeric_limits<float>::infinity())
	{
		int dir = raise ? 1 : -1;				// Lower or increase height?
		float halfSize = mRadius / 2.0f;
		for(int x = -mRadius; x < mRadius; x++) {
			for(int z = -mRadius; z < mRadius; z++) {
				float dist = sqrt(x*x + z*z);	// Distance from center.
				float newHeight = mTerrain->GetHeight(center.x + x, center.z + z) + max(0, mRadius - dist) / 10 * mStrength * dir;
				mTerrain->SetHeigt(center.x + x, center.z + z, newHeight);
			}
		}

		// Apply some smoothing.
		mTerrain->Smooth(center, mRadius*2);
		mTerrain->Smooth(center, mRadius*2);
		mTerrain->BuildHeightmapSRV(GetD3DDevice());
	}
}

//! Smoothen the terrain in a circle around center.
void TerrainTool::SmothTerrain(XMFLOAT3 center)
{
	// Did the ray hit the terrain?
	if(center.x != numeric_limits<float>::infinity())
	{
		mTerrain->Smooth(center, mRadius);
		mTerrain->Smooth(center, mRadius);
		mTerrain->BuildHeightmapSRV(GetD3DDevice());
	}
}

//! Changes the texture in a circle around center.
void TerrainTool::EditTextures(XMFLOAT3 center)
{
	// Did the ray hit the terrain?
	if(center.x != numeric_limits<float>::infinity())
	{
		float halfSize = mRadius / 2.0f;
		for(float x = -mRadius; x < mRadius; x+=0.5f) {
			for(float z = -mRadius; z < mRadius; z+=0.5f) {
				float dist = sqrt(x*x + z*z);	// Distance from center.
				float modifier = max(0, mRadius - dist) / 10 * mStrength/14;
				mTerrain->SetBlend(XMFLOAT3(center.x + x, 0, center.z + z), modifier, mSelectedLayer);
			}
		}

		// Apply some smoothing.
		mTerrain->BuildBlendMapSRV(GetD3DDevice());
	}
}

//! Sets the tool radius.
void TerrainTool::SetRadius(float radius)
{
	mRadius = radius;
	Effects::TerrainFX->SetToolRadius(radius);
	Effects::TerrainFX->Apply(GetD3DContext());
}

//! Sets the tool strength.
void TerrainTool::SetStrength(float strength)
{
	mStrength = strength;
}

//! Sets the terrain.
void TerrainTool::SetTerrain(GLib::Terrain* pTerrain)
{
	mTerrain = pTerrain;
}
	
//! Sets the active tool type.
void TerrainTool::SetTool(ToolType tool)
{
	mCurrentTool = tool;
}

//! Returns the tool radius.
float TerrainTool::GetRadius()
{
	return mRadius;
}
	
//! Returns the tool strength.
float TerrainTool::GetStrength()
{
	return mStrength;
}

//! Sets the selected layer.
void TerrainTool::SetSelectedLayer(int layer)
{
	mSelectedLayer = layer;
}

//! Returns the active tool.
ToolType TerrainTool::GetActiveTool()
{
	return mCurrentTool;
}

//! Returns the selected layer.
int TerrainTool::GetSelectedLayer()
{
	return mSelectedLayer;
}
