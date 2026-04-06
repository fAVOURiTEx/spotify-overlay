#include "animation.hpp" // Подключаем свой заголовок

// Функция плавности (ease-out cubic)
float ease_out_cubic(float t) {
  t = std::clamp(t, 0.f, 1.f);
  const float u = 1.f - t;
  return 1.f - u * u * u;
}

// Расчёт прозрачности
float calc_alpha(float t) {
  const float fade_in = 0.2f;
  const float hold = 3.3f;
  const float fade_out = 0.5f;

  if (t < fade_in) {
    return t / fade_in;
  }
  if (t < fade_in + hold) {
    return 1.f;
  }
  if (t < fade_in + hold + fade_out) {
    return 1.f - (t - fade_in - hold) / fade_out;
  }
  return 0.f;
}

// Расчёт сдвига
float calc_slide(float t) {
  const float slide_in = 0.3f;
  const float slide_px = 50.f;

  if (t < slide_in) {
    return (1.f - ease_out_cubic(t / slide_in)) * slide_px;
  }
  return 0.f;
}
