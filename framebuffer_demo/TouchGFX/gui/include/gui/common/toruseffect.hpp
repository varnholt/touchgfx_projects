#pragma once

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Bitmap.hpp>
#include "matrix.hpp"
#include "torus.hpp"
#include "touchinput.hpp"
#include <array>

class TorusEffect : public touchgfx::Widget
{
public:
    TorusEffect();
    ~TorusEffect() override;
    touchgfx::Rect getSolidRect() const override;
    void draw(const touchgfx::Rect& invalidatedArea) const override;
    void handleTickEvent() override;
    void handleClickEvent(const touchgfx::ClickEvent& event) override;
    void handleDragEvent(const touchgfx::DragEvent& event) override;
    void setEnvMapTexture(touchgfx::BitmapId id);
    void setLowQuality();
    void setMediumQuality();
    void setHighQuality();

private:
    struct Face { float depth; int index; };
    static Vector3D transform(const Vector3D& point, const Matrix& matrix);
    float _time = 0;
    uint32_t _idle_ticks = 0;
    touchgfx::BitmapId _environmentMap = touchgfx::BITMAP_INVALID;
    float _rotationX = 0, _rotationY = 0;
    float _dx = 0, _dy = 0;
    TouchInput _input;
    static constexpr size_t maxVertices = (32 + 1) * (24 + 1);
    static constexpr size_t maxFaces = 32 * 24 * 2;
    // Render scratch storage is explicit and never changes animation state.
    mutable std::array<Vector3D, maxVertices> _cameraVertices;
    mutable std::array<Vector3D, maxVertices> _environmentUV;
    mutable std::array<Face, maxFaces> _faces;
    Torus<8, 6> _low{12, 5};
    Torus<16, 12> _medium{12, 5};
    Torus<32, 24> _high{12, 5};
    Mesh* _mesh = &_high;
};
