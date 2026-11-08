#pragma once

#include "vertex.hpp"
#include <cstdint>
#include "rastersurface.hpp"

void drawTriangle(const RasterSurface& surface, const Vertex& a, const Vertex& b,
                  const Vertex& c, const uint16_t* texture);
void drawSolidTriangle(const RasterSurface& surface, const Vertex& a, const Vertex& b,
                       const Vertex& c, uint16_t color);

//! draws a triangle (three vertices) to a 16bit framebuffer
void drawTriangle(
    unsigned short* screenBuffer,
    const Vertex* v1,
    const Vertex* v2,
    const Vertex* v3,
    const unsigned short* texture,
    int32_t width,
    int32_t height
);

