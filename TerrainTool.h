#pragma once

#include "d3dUtil.h"

class World;
class Terrain;

enum ToolType
{
	TOOL_HEIGHT,
	TOOL_SMOTH
};

class TerrainTool
{
public:
	TerrainTool();
	~TerrainTool();

	void Update(float dt);
	void Draw(Graphics* pGraphics);

	void SetTerrain(Terrain* pTerrain);
	void SetEnabled(bool enabled);
	void SetTool(ToolType tool);
	void SetRadius(float radius);
	void SetStrength(float strength);
	float GetRadius();
	float GetStrength();
private:
	void ChangeHeight(XMFLOAT3 center, bool raise);
	void SmothTerrain(XMFLOAT3 center);
	XMFLOAT3 GetIntersectPoint();
private:
	Terrain*	mTerrain;
	ToolType	mCurrentTool;
	bool		mEnabled;
	float		mRadius;
	float		mStrength;
	const float mUpdateInterval;
};