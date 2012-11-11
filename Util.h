#pragma once
#include <string>

using namespace std;

namespace GLib {
	class Input;
}

enum ToolType
{
	TOOL_HEIGHT,
	TOOL_SMOTH,
	TOOL_TEXTURE
};

bool IsIn3DScreen(GLib::Input* pInput);

string ToString(wstring str);