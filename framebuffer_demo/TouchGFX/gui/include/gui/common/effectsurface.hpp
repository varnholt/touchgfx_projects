#pragma once

#include "rastersurface.hpp"
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>

// Keep the framebuffer lock paired even when an invalidation is empty.
class EffectSurface
{
public:
    EffectSurface(const touchgfx::Widget& widget, const touchgfx::Rect& invalidated)
        : _hal(*touchgfx::HAL::getInstance()), _pixels(nullptr)
    {
        // These custom effects require an unrotated, full RGB565 framebuffer.
        const bool supported = touchgfx::HAL::lcd().framebufferFormat() == touchgfx::Bitmap::RGB565 &&
            touchgfx::HAL::DISPLAY_ROTATION == touchgfx::rotate0 &&
            _hal.getFrameRefreshStrategy() != touchgfx::HAL::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER;
        assert(supported && "Effects require an unrotated full RGB565 framebuffer");
        if (supported) _pixels = _hal.lockFrameBuffer();
        const touchgfx::Rect absolute = widget.getAbsoluteRect();
        _originX = absolute.x;
        _originY = absolute.y;
        _width = widget.getWidth();
        _height = widget.getHeight();
        _clip = {invalidated.x, invalidated.y,
                 invalidated.x + invalidated.width, invalidated.y + invalidated.height};
    }
    ~EffectSurface() { if (_pixels) _hal.unlockFrameBuffer(); }
    EffectSurface(const EffectSurface&) = delete;
    EffectSurface& operator=(const EffectSurface&) = delete;

    RasterSurface surface() const
    {
        return RasterSurface(_pixels, touchgfx::HAL::FRAME_BUFFER_WIDTH,
                             touchgfx::HAL::FRAME_BUFFER_HEIGHT,
                             touchgfx::HAL::lcd().framebufferStride() / 2,
                             _originX, _originY, _width, _height, _clip);
    }

private:
    touchgfx::HAL& _hal;
    uint16_t* _pixels;
    int _originX, _originY, _width, _height;
    RasterClip _clip;
};
