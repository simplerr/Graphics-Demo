#pragma once
#include "Gwen/Controls/CollapsibleCategory.h"
#include "Gwen/Controls/TreeControl.h"
#include <map>
#include <string>

using namespace std;
class World;
class Editor;

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

	void CreateTree(World* pWorld);
	void OnSelectChange(Gwen::Controls::Base* pControl);
	void SetEditor(Editor* pEditor);
private:
	map<string, WorldTreeNode> mNodeMap;
	Gwen::Controls::TreeControl* mTreeControl;
	Editor* mEditor;
};