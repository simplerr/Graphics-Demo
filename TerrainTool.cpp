#include "TerrainTool.h"
#include "Terrain.h"
#include "Input.h"

TerrainTool::TerrainTool() : mUpdateInterval(0.03f)
{
	SetEnabled(true);	// [NOTE]
	SetTool(TOOL_HEIGHT);
	SetSize(40.0f);
	SetStrength(6.0f);
}

TerrainTool::~TerrainTool()
{

}

void TerrainTool::Update(float dt)
{
	static float timer = 0;
	timer += dt;

	// Enabled?
	if(!mEnabled)
		return;

	// Don't use the tools every frame.
	if(timer > mUpdateInterval)
	{
		// Which tool is selected and was any mouse button pressed?
		if(mCurrentTool == TOOL_HEIGHT)
		{
			if(gInput->KeyDown(VK_LBUTTON))
				ChangeHeight(GetIntersectPoint(), true);
			else if(gInput->KeyDown(VK_RBUTTON))
				ChangeHeight(GetIntersectPoint(), false);
		}
		else if(mCurrentTool == TOOL_SMOTH)
		{
			if(gInput->KeyDown(VK_LBUTTON))
				SmothTerrain(GetIntersectPoint());
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
		float halfSize = mSize / 2.0f;
		for(int x = -halfSize; x < halfSize; x++) {
			for(int z = -halfSize; z < halfSize; z++) {
				float dist = sqrt(x*x + z*z);	// Distance from center.
				float newHeight = mTerrain->GetHeight(center.x + x, center.z + z) + max(0, 14 - dist) / 10 * mStrength * dir;
				mTerrain->SetHeigt(center.x + x, center.z + z, newHeight);
			}
		}

		// Apply some smoothing.
		mTerrain->Smooth(center, mSize*1.2);
		//mTerrain->Smooth(center, mSize*1.2);
		mTerrain->Smooth(center, mSize/2*1.2);
		mTerrain->Smooth(center, mSize/4*1.2);
		mTerrain->BuildHeightmapSRV(GetD3DDevice());
	}
}

void TerrainTool::SmothTerrain(XMFLOAT3 center)
{
	// Did the ray hit the terrain?
	if(center.x != numeric_limits<float>::infinity())
	{
		mTerrain->Smooth(center, mSize);
		mTerrain->Smooth(center, mSize);
		mTerrain->BuildHeightmapSRV(GetD3DDevice());
	}
}

XMFLOAT3 TerrainTool::GetIntersectPoint()
{
	return mTerrain->GetIntersectPoint(gInput->GetWorldPickingRay());
}

void TerrainTool::SetSize(float size)
{
	mSize = size;
}

void TerrainTool::SetStrength(float strength)
{
	mStrength = strength;
}

void TerrainTool::SetTerrain(Terrain* pTerrain)
{
	mTerrain = pTerrain;
}

void TerrainTool::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}
	
void TerrainTool::SetTool(ToolType tool)
{
	mCurrentTool = tool;
}