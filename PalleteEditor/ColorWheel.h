#pragma once
#include "Character.h"
#include "Data\GroupJSONFiles.h"

class ColorWheel {
public:
    // Draw the color-wheel/detail window for a specific character and color group.
    // `open` is a reference to the boolean which controls the window visibility.
    static void Draw(Character& currentChar, const ColorGroup& group, bool& open);
};
