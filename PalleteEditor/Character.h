#pragma once
#include "pch.h"

class Character {
	public:
	std::string Char_Name;
	int Max_Pallete_Num;
	int Current_Pallete_Num;
	int ID;
	int Num_Of_Color;
	std::vector<__int32> Character_Colors;
	__int32 LineColor;
	__int32 SuperShadowColor1;
	__int32 SuperShadowColor2;
};

inline static const char* characterNames[] = {
	"Filia",
	"Cerebella",
	"Peacock",
	"Parasoul",
	"MsFortune",
	"Painwheel",
	"Valentine",
	"Double",
	"Squigly",
	"BigBand",
	"Eliza",
	"Fukua",
	"Beowulf",
	"RoboFortune",
	"Annie",
	"Umbrella",
	"BlackDahlia",
	"Marie"
};
