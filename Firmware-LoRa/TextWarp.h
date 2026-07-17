#ifndef TEXTWRAP_H
#define TEXTWRAP_H

#include <Adafruit_SharpMem.h>

inline int16_t printWrapped(Adafruit_SharpMem &d, int16_t x, int16_t y,
                             uint16_t maxW, int16_t maxY,
                             const String &text, uint8_t size = 1) {
  uint16_t maxChars = maxW / (6 * size);
  if (maxChars == 0) return y;
  d.setTextSize(size);
  for (uint16_t i = 0; i < text.length() && y + 8 * size <= maxY; i += maxChars, y += 8 * size + 2) {
    d.setCursor(x, y);
    d.print(text.substring(i, i + maxChars));
  }
  return y;
}

#endif