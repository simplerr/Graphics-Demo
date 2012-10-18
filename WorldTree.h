#pragma once
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

class WorldTree : public Gwen::Controls::TreeControl
{
public:
	WorldTree(Gwen::Controls::Base* pParent);
	~WorldTree();

	void CreateTree(World* pWorld);
	void OnSelectChange(Gwen::Controls::Base* pControl);
	void SetEditor(Editor* pEditor);
private:
	map<string, WorldTreeNode> mNodeMap;
	Editor* mEditor;
};