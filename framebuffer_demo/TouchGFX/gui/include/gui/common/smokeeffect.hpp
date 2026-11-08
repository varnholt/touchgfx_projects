#pragma once

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Bitmap.hpp>
#include "smokesimulation.hpp"

class SmokeEffect : public touchgfx::Widget
{
public:
    enum class Quality { Low, Medium, High };
    SmokeEffect();
    ~SmokeEffect() override;
    touchgfx::Rect getSolidRect() const override;
    void draw(const touchgfx::Rect& invalidatedArea) const override;
    void handleTickEvent() override;
    void handleClickEvent(const touchgfx::ClickEvent& event) override;
    void handleDragEvent(const touchgfx::DragEvent& event) override;
    void setGradientTexture(touchgfx::BitmapId id);
    void setOverlayTexture(touchgfx::BitmapId id);
    void setLowQuality();
    void setMediumQuality();
    void setHighQuality();

private:
    SmokeSimulation _simulation;
    touchgfx::BitmapId _gradient = touchgfx::BITMAP_INVALID;
    touchgfx::BitmapId _overlay = touchgfx::BITMAP_INVALID;
    Quality _quality = Quality::High;
};
