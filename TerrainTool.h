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
	void SetSize(float size);
	void SetStrength(float strength);
private:
	void ChangeHeight(XMFLOAT3 center, bool raise);
	void SmothTerrain(XMFLOAT3 center);
	XMFLOAT3 GetIntersectPoint();
private:
	Terrain*	mTerrain;
	ToolType	mCurrentTool;
	bool		mEnabled;
	float		mSize;
	float		mStrength;
	const float mUpdateInterval;
};