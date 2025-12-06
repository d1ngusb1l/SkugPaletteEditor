#ifndef DRAWING_H
#define DRAWING_H

#include "pch.h"

class Drawing
{
private:
	inline static LPCSTR lpWindowName = "Skullgirls Pallete Editor 2nd Encore";
	inline static ImVec2 vWindowSize = { 512, 512 };
	inline static ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar;
	inline static bool bDraw = true;
	inline static bool bShow_about_window = false;
	inline static bool bGrouping = true;
	inline static bool bJSONEnable = false;

public:
	static void Active();
	static bool isActive();
	static void Draw();
};

#endif
