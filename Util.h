#pragma once

#include <string>
using namespace std;

enum ToolType
{
	TOOL_HEIGHT,
	TOOL_SMOTH,
	TOOL_TEXTURE
};

bool IsIn3DScreen();

string ToString(wstring str);