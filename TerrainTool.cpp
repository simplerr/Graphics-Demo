#include "TerrainTool.h"
#include "Terrain.h"
#include "Input.h"
#include "Effects.h"
#include "Util.h"

TerrainTool::TerrainTool() : mUpdateInterval(0.03f)
{
	SetTool(TOOL_HEIGHT);
	SetRadius(20.0f);
	SetStrength(1.0f);
	SetSelectedTexture(4);

	Effects::TerrainFX->SetToolCenter(XMFLOAT2(-999999, -999999));
	Effects::TerrainFX->Apply();
}

TerrainTool::~TerrainTool()
{

}

void TerrainTool::Update(float dt)
{
	static float timer = 0;
	timer += dt;

	// Don't use the tools every frame.
	XMFLOAT3 intersectPoint = GetIntersectPoint();
	if(timer > mUpdateInterval)
	{
		// Which tool is selected and was any mouse button pressed?
		if(mCurrentTool == TOOL_HEIGHT)
		{
			if(gInput->KeyDown(VK_LBUTTON) && IsIn3DScreen())
				ChangeHeight(intersectPoint, true);
			else if(gInput->KeyDown(VK_RBUTTON) && IsIn3DScreen())
				ChangeHeight(intersectPoint, false);
		}
		else if(mCurrentTool == TOOL_TEXTURE)
		{
			if(gInput->KeyDown(VK_LBUTTON) && IsIn3DScreen())
				EditTextures(intersectPoint);
		}
		else if(mCurrentTool == TOOL_SMOTH)
		{
			if(gInput->KeyDown(VK_LBUTTON) && IsIn3DScreen())
				SmothTerrain(intersectPoint);
		}

		// Reset the timer.
		timer = 0.0f;
	}

	if(mCurrentTool == TOOL_HEIGHT)
	{
		if(gInput->KeyReleased(VK_LBUTTON) || gInput->KeyReleased(VK_RBUTTON)) {
			mTerrain->Smooth();
			mTerrain->BuildHeightmapSRV(GetD3DDevice());
		}
	}

	if(intersectPoint.x != numeric_limits<float>::infinity()) {
		Effects::TerrainFX->SetToolCenter(XMFLOAT2(intersectPoint.x, intersectPoint.z));
		Effects::TerrainFX->Apply();
	}
	else {
		Effects::TerrainFX->SetToolCenter(XMFLOAT2(-999999, -999999));
		Effects::TerrainFX->Apply();
	}
}
	
void TerrainTool::Draw(Graphics* pGraphics)
{

}

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
				mTerrain->SetBlend(XMFLOAT3(center.x + x, 0, center.z + z), modifier, mSelectedTexture);
			}
		}

		// Apply some smoothing.
		mTerrain->BuildBlendMapSRV(GetD3DDevice());
	}
}

XMFLOAT3 TerrainTool::GetIntersectPoint()
{
	return mTerrain->GetIntersectPoint(gInput->GetWorldPickingRay());
}

void TerrainTool::SetRadius(float radius)
{
	mRadius = radius;
	Effects::TerrainFX->SetToolRadius(radius);
	Effects::TerrainFX->Apply();
}

void TerrainTool::SetStrength(float strength)
{
	mStrength = strength;
}

void TerrainTool::SetTerrain(Terrain* pTerrain)
{
	mTerrain = pTerrain;
}
	
void TerrainTool::SetTool(ToolType tool)
{
	mCurrentTool = tool;
}

float TerrainTool::GetRadius()
{
	return mRadius;
}
	
float TerrainTool::GetStrength()
{
	return mStrength;
}

void TerrainTool::SetSelectedTexture(int texture)
{
	mSelectedTexture = texture;
}

ToolType TerrainTool::GetActiveTool()
{
	return mCurrentTool;
}

int TerrainTool::GetSelectedLayer()
{
	return mSelectedTexture;
}
