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
	if(pos. x > 220 && pos.x < 1000)
		return true;
	else
		return false;
}