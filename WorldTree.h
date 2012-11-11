#pragma once
#include "Gwen/Controls/CollapsibleCategory.h"
#include "Gwen/Controls/TreeControl.h"
#include <map>
#include <string>

using namespace std;

// Forward declarations.
#pragma region Forward declarations.
class World;
class Editor;
#pragma endregion

struct WorldTreeNode
{
	void* pData;
	int type;
};

//! A TreeControl that contains all objects in World.
class WorldTree : public Gwen::Controls::CollapsibleCategory
{
public:
	WorldTree(Gwen::Controls::Base* pParent);
	~WorldTree();

	void CreateTree();
	void OnSelectChange(Gwen::Controls::Base* pControl);
	void SetEditor(Editor* pEditor);
	void SetWorld(World* pWorld);
private:
	map<string, WorldTreeNode> mNodeMap;
	Gwen::Controls::TreeControl* mTreeControl;
	Gwen::Controls::CollapsibleCategory* mCategory;
	Editor* mEditor;
	World* mWorld;
};