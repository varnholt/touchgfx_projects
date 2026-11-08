#pragma once

#include <algorithm>
#include <cstdint>

struct RasterClip
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    bool empty() const { return left >= right || top >= bottom; }
};

// Coordinates and clipping are local to the widget; stride is measured in pixels.
class RasterSurface
{
public:
    RasterSurface(uint16_t* pixels, int bufferWidth, int bufferHeight, int stride,
                  int originX, int originY, int width, int height, RasterClip clip)
        : _pixels(pixels), _stride(stride), _originX(originX), _originY(originY), _clip(clip)
    {
        _clip.left = (std::max)(_clip.left, (std::max)(0, -originX));
        _clip.top = (std::max)(_clip.top, (std::max)(0, -originY));
        _clip.right = (std::min)(_clip.right, (std::min)(width, bufferWidth - originX));
        _clip.bottom = (std::min)(_clip.bottom, (std::min)(height, bufferHeight - originY));
        if (!pixels || stride < bufferWidth || bufferWidth <= 0 || bufferHeight <= 0)
            _clip = RasterClip{};
    }

    const RasterClip& clip() const { return _clip; }
    uint16_t* span(int x, int y) const
    {
        return _pixels + (_originY + y) * _stride + _originX + x;
    }

private:
    uint16_t* _pixels;
    int _stride;
    int _originX;
    int _originY;
    RasterClip _clip;
};
