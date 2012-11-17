#pragma once
#include "d3dUtil.h"
#include "Util.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Terrain;
	class World;
}

#pragma endregion

//! Can change the height and texture of the terrain.
class TerrainTool
{
public:
	TerrainTool();
	~TerrainTool();

	void Update(GLib::Input* pInput, float dt);
	void Draw(GLib::Graphics* pGraphics);

	void SetTerrain(GLib::Terrain* pTerrain);
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
private:
	GLib::Terrain*	mTerrain;
	ToolType	mCurrentTool;
	float		mRadius;
	float		mStrength;
	const float mUpdateInterval;
	int			mSelectedLayer;
};