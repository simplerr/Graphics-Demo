#pragma once

#include "d3dUtil.h"
#include "Util.h"

class World;
class Terrain;

//! Can change the height and texture of the terrain.
class TerrainTool
{
public:
	TerrainTool();
	~TerrainTool();

	void Update(float dt);
	void Draw(Graphics* pGraphics);

	void SetTerrain(Terrain* pTerrain);
	void SetTool(ToolType tool);
	void SetRadius(float radius);
	void SetStrength(float strength);
	void SetSelectedLayer(int layer);
	float GetRadius();
	float GetStrength();
	ToolType GetActiveTool();
	int GetSelectedLayer();
private:
	void ChangeHeight(XMFLOAT3 center, bool raise);
	void SmothTerrain(XMFLOAT3 center);
	void EditTextures(XMFLOAT3 center);
	XMFLOAT3 GetIntersectPoint();
private:
	Terrain*	mTerrain;
	ToolType	mCurrentTool;
	float		mRadius;
	float		mStrength;
	const float mUpdateInterval;
	int			mSelectedLayer;
};