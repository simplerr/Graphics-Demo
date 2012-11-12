#include "Util.h"
#include "d3dUtil.h"
#include "Input.h"

string ToString(wstring str)
{
	return string(str.begin(), str.end());
}

bool IsIn3DScreen(GLib::Input* pInput)
{
	XMFLOAT3 pos = pInput->MousePosition();

	if(pos.x < 220 && pos.y > 800)
		return true;	// Left side.
	else if(pos.x > GLib::GetClientWidth() - 200 && pos.y > 500)	// [NOTE][HACK] 500
		return true;	// Right side.
	else if(pos.x > 220 && pos.x < GLib::GetClientWidth() - 200)
		return true;	// Middle.
	else
		return false;
}