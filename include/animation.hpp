#pragma once // Защита от повторного включения

#include <algorithm> // для std::clamp

// Объявления функций (только сигнатуры, без тела)
float ease_out_cubic(float t);
float calc_alpha(float t);
float calc_slide(float t);
