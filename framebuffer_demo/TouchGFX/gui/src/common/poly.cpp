#include "gui/common/poly.hpp"

#include <algorithm>
#include <cmath>

namespace
{
bool finite(const Vertex& v, bool textured)
{
    return std::isfinite(v.x) && std::isfinite(v.y) &&
           (!textured || (std::isfinite(v.u) && std::isfinite(v.v)));
}

// Texture coordinates use 16:16 fixed point and wrap every 256 texels.
uint32_t wrapped(float value)
{
    return static_cast<uint32_t>(static_cast<int64_t>(std::fmod(value, 16777216.0f)));
}

void rasterize(const RasterSurface& surface, const Vertex& a, const Vertex& b,
               const Vertex& c, const uint16_t* texture, uint16_t color)
{
    const auto& clip = surface.clip();
    if (clip.empty() || !finite(a, texture != nullptr) ||
        !finite(b, texture != nullptr) || !finite(c, texture != nullptr)) return;
    const float area = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    if (area == 0.0f || !std::isfinite(area)) return;

    // Clamp floating-point bounds before converting to integers, even for huge vertices.
    const int firstY = static_cast<int>(std::ceil((std::min)(static_cast<float>(clip.bottom),
        (std::max)(static_cast<float>(clip.top), (std::min)(a.y, (std::min)(b.y, c.y))))));
    const int lastY = static_cast<int>(std::ceil((std::max)(static_cast<float>(clip.top),
        (std::min)(static_cast<float>(clip.bottom), (std::max)(a.y, (std::max)(b.y, c.y))))));
    const Vertex* vertices[] = {&a, &b, &c};
    for (int y = firstY; y < lastY; ++y)
    {
        Vertex hits[2] = {};
        int count = 0;
        for (int edge = 0; edge < 3 && count < 2; ++edge)
        {
            const Vertex* pVertex = vertices[edge];
            const Vertex* qVertex = vertices[(edge + 1) % 3];
            // Shared edges must use the same interpolation direction in both triangles.
            if (pVertex->y > qVertex->y) std::swap(pVertex, qVertex);
            const Vertex& p = *pVertex;
            const Vertex& q = *qVertex;
            if (y < p.y || y >= q.y) continue;
            const float t = (y - p.y) / (q.y - p.y);
            hits[count].x = p.x + t * (q.x - p.x);
            hits[count].u = p.u + t * (q.u - p.u);
            hits[count].v = p.v + t * (q.v - p.v);
            ++count;
        }
        if (count != 2) continue;
        if (hits[0].x > hits[1].x) std::swap(hits[0], hits[1]);
        const Vertex& left = hits[0];
        const Vertex& right = hits[1];
        if (!std::isfinite(left.x) || !std::isfinite(right.x) ||
            left.x >= right.x || right.x <= clip.left || left.x >= clip.right) continue;
        const int x1 = static_cast<int>(std::ceil((std::max)(left.x, static_cast<float>(clip.left))));
        const int x2 = static_cast<int>(std::ceil((std::min)(right.x, static_cast<float>(clip.right))));
        if (x1 >= x2) continue;
        uint16_t* destination = surface.span(x1, y);
        if (!texture)
        {
            std::fill(destination, destination + x2 - x1, color);
            continue;
        }
        const float invWidth = 1.0f / (right.x - left.x);
        const float du = (right.u - left.u) * invWidth;
        const float dv = (right.v - left.v) * invWidth;
        const float startU = left.u + (x1 - left.x) * du;
        const float startV = left.v + (x1 - left.x) * dv;
        if (!std::isfinite(startU) || !std::isfinite(startV) ||
            !std::isfinite(du) || !std::isfinite(dv)) continue;
        uint32_t u = wrapped(startU), v = wrapped(startV);
        const uint32_t stepU = wrapped(du), stepV = wrapped(dv);
        for (int x = x1; x < x2; ++x)
        {
            *destination++ = texture[((v >> 8) & 0xff00u) | ((u >> 16) & 0xffu)];
            u += stepU;
            v += stepV;
        }
    }
}
}

void drawTriangle(const RasterSurface& surface, const Vertex& a, const Vertex& b,
                  const Vertex& c, const uint16_t* texture)
{
    if (texture) rasterize(surface, a, b, c, texture, 0);
}

void drawSolidTriangle(const RasterSurface& surface, const Vertex& a, const Vertex& b,
                       const Vertex& c, uint16_t color)
{
    rasterize(surface, a, b, c, nullptr, color);
}

void drawTriangle(unsigned short* screenBuffer, const Vertex* a, const Vertex* b,
                  const Vertex* c, const unsigned short* texture, int32_t width, int32_t height)
{
    if (!a || !b || !c) return;
    const RasterSurface surface(screenBuffer, width, height, width, 0, 0, width, height,
                                {0, 0, width, height});
    drawTriangle(surface, *a, *b, *c, texture);
}
