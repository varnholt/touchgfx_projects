#include "gui/common/smokeeffect.hpp"
#include "gui/common/effectsurface.hpp"
#include <touchgfx/Application.hpp>
#include <algorithm>
#include <cmath>

namespace
{
float densityValue(float value)
{
    return std::isfinite(value) ? (std::max)(0.0f, (std::min)(1.0f, value)) : 0.0f;
}
}

SmokeEffect::SmokeEffect()
{
    setWidth(touchgfx::HAL::DISPLAY_WIDTH);
    setHeight(touchgfx::HAL::DISPLAY_HEIGHT);
    touchgfx::Application::getInstance()->registerTimerWidget(this);
    setTouchable(true);
}

SmokeEffect::~SmokeEffect()
{
    touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}

touchgfx::Rect SmokeEffect::getSolidRect() const
{
    return touchgfx::Rect(0, 0, getWidth(), getHeight());
}

void SmokeEffect::draw(const touchgfx::Rect& invalidatedArea) const
{
    const EffectSurface frame(*this, invalidatedArea);
    const auto surface = frame.surface();
    const auto& clip = surface.clip();
    if (clip.empty()) return;
    const touchgfx::Bitmap gradient(_gradient);
    const bool hasGradient = _gradient != touchgfx::BITMAP_INVALID &&
        gradient.getFormat() == touchgfx::Bitmap::RGB565 && gradient.getWidth() > 0 && gradient.getHeight() == 1;
    const auto* colors = hasGradient ? reinterpret_cast<const uint16_t*>(gradient.getData()) : nullptr;
    constexpr int resolution = SmokeSimulation::resolution;
    for (int cy = 0; cy < resolution; ++cy)
    {
        const int y0 = cy * getHeight() / resolution;
        const int y1 = (cy + 1) * getHeight() / resolution;
        const int top = (std::max)(y0, clip.top), bottom = (std::min)(y1, clip.bottom);
        if (top >= bottom) continue;
        for (int cx = 0; cx < resolution; ++cx)
        {
            const int x0 = cx * getWidth() / resolution;
            const int x1 = (cx + 1) * getWidth() / resolution;
            const int left = (std::max)(x0, clip.left), right = (std::min)(x1, clip.right);
            if (left >= right) continue;
            const float tl = densityValue(_simulation.density(cx, cy));
            const float tr = densityValue(_simulation.density(cx + 1, cy));
            const float bl = densityValue(_simulation.density(cx, cy + 1));
            const float br = densityValue(_simulation.density(cx + 1, cy + 1));
            for (int y = top; y < bottom; ++y)
            {
                const float fy = (y - y0) / static_cast<float>(y1 - y0);
                const float l = tl + (bl - tl) * fy;
                const float r = tr + (br - tr) * fy;
                const float delta = (r - l) / (x1 - x0);
                float value = l + (left - x0) * delta;
                auto* pixels = surface.span(left, y);
                for (int x = left; x < right; ++x)
                {
                    const float d = _quality == Quality::Low ? tl : densityValue(value);
                    *pixels++ = _quality == Quality::High && colors
                        ? colors[static_cast<int>(d * (gradient.getWidth() - 1))]
                        : static_cast<uint16_t>(static_cast<int>(d * 63) << 5);
                    value += delta;
                }
            }
        }
    }
}

void SmokeEffect::handleTickEvent()
{
    const touchgfx::Bitmap overlay(_overlay);
    const bool hasOverlay = _overlay != touchgfx::BITMAP_INVALID && overlay.getFormat() == touchgfx::Bitmap::ARGB8888;
    _simulation.step(getWidth(), getHeight(), hasOverlay ? overlay.getData() : nullptr,
                     hasOverlay ? overlay.getWidth() : 0, hasOverlay ? overlay.getHeight() : 0);
    invalidate();
}

void SmokeEffect::handleClickEvent(const touchgfx::ClickEvent& event)
{
    auto& input = _simulation.input();
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        input.press(event.getX(), event.getY(), getWidth(), getHeight());
        if (input.active()) _simulation.wake();
    }
    else if (event.getType() == touchgfx::ClickEvent::CANCEL) input.cancel();
    else input.release();
}

void SmokeEffect::handleDragEvent(const touchgfx::DragEvent& event)
{
    auto& input = _simulation.input();
    if (!input.active()) return;
    input.drag(event.getOldX(), event.getOldY(), event.getNewX(), event.getNewY(), getWidth(), getHeight());
    _simulation.wake();
}

void SmokeEffect::setGradientTexture(touchgfx::BitmapId id) { _gradient = id; }
void SmokeEffect::setOverlayTexture(touchgfx::BitmapId id) { _overlay = id; }
void SmokeEffect::setLowQuality() { _quality = Quality::Low; }
void SmokeEffect::setMediumQuality() { _quality = Quality::Medium; }
void SmokeEffect::setHighQuality() { _quality = Quality::High; }
