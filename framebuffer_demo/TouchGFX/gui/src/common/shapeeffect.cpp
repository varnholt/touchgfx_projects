#include "gui/common/shapeeffect.hpp"

#include <touchgfx/Application.hpp>
#include <touchgfx/hal/HAL.hpp>

#include "gui/common/poly.hpp"

#include "gui/common/effectsurface.hpp"

// screen configuration
namespace
{
constexpr float thickness = 20.0f;
}


ShapeEffect::ShapeEffect()
{
    setWidth(touchgfx::HAL::DISPLAY_WIDTH);
    setHeight(touchgfx::HAL::DISPLAY_HEIGHT);
    
    touchgfx::Application::getInstance()->registerTimerWidget(this);

    _pos.x = getWidth() / 2.0f;
    _pos.y = getHeight() / 2.0f;

    for (auto i = 0u; i < circles; i++)
    {
        _centers[i] = {getWidth() / 2.0f, getHeight() / 2.0f, 0.0f};
        _rotations[i] = 0.0f;
    }

    setTouchable(true);
}


ShapeEffect::~ShapeEffect()
{
    touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}


touchgfx::Rect ShapeEffect::getSolidRect() const
{
	return touchgfx::Rect(
		0,
		0,
		0,
        0 
    );
}


void ShapeEffect::draw(const touchgfx::Rect& invalidatedArea) const
{
    const EffectSurface frame(*this, invalidatedArea);
    drawVertices(frame.surface());
}


void ShapeEffect::handleTickEvent() 
{
    static const auto dt = 0.01f;
    _time = static_cast<float>(fmod(_time + dt, 6.2831853f));

    updateCenters();
    updateRotation();
    updateVertices();

    invalidate();
}


void ShapeEffect::handleClickEvent(const touchgfx::ClickEvent& event)
{
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        _input.press(event.getX(), event.getY(), getWidth(), getHeight());
        if (_input.active())
        {
            _pos.x = static_cast<float>(event.getX());
            _pos.y = static_cast<float>(event.getY());
        }
    }
    else if (event.getType() == touchgfx::ClickEvent::CANCEL) _input.cancel();
    else _input.release();
}

void ShapeEffect::handleDragEvent(const touchgfx::DragEvent& event)
{
    if (!_input.active()) return;
    _input.drag(event.getOldX(), event.getOldY(), event.getNewX(), event.getNewY(), getWidth(), getHeight());
    const auto position = _input.consume();
    _pos.x = static_cast<float>(position.x);
    _pos.y = static_cast<float>(position.y);
}


void ShapeEffect::drawVertices(const RasterSurface& surface) const
{
    // we skip all black circles for now
    for (auto circle_index = 0u; circle_index < circles; circle_index+=2) 
    {
        auto& circle = _circles[circles - 1 - circle_index];

        auto& r0 = circle[0];
        auto& r1 = circle[1];

        const auto color = (circle_index % 2 == 0) ? 0xffff : 0x0;

        for (auto segment = 0u; segment < segments; segment++)
        {
            auto v0 = &r0[segment];
            auto v1 = &r0[(segment + 1) % segments];
            auto v2 = &r1[(segment + 1) % segments];
            auto v3 = &r1[segment];

            drawSolidTriangle(surface, *v0, *v1, *v3, color);
            drawSolidTriangle(surface, *v3, *v1, *v2, color);
        }
    }
}


void ShapeEffect::updateRotation()
{
    _rot = static_cast<float>(sin(_time)) * 4.0f;

    for (auto i = 0u; i < circles; i++) 
    {
        const auto rotation = _rotations[i];
        const auto delta_rotation = (_rot - rotation) / (i + 1);

        _rotations[i] = rotation + delta_rotation;
    }
}


void ShapeEffect::updateCenters()
{
    for (auto i = 0u; i < circles; i++)
    {
        const auto damping = (1.0f / (i + 1));
        const auto& center = _centers[i];
        auto delta = (_pos - center); 

        delta.x *= damping;
        delta.y *= damping;

        _centers[i] = center + delta;
    }
}


void ShapeEffect::updateVertices()
{
    // we skip all black circles for now
    for (auto circle_index = 1u; circle_index < circles ; circle_index+=2)
    {
        auto& circle = _circles[circle_index];

        for (auto ring_index = 0; ring_index < 2; ring_index++)
        {
            auto& ring = circle[ring_index];

            auto radius = circle_index * thickness + ring_index * thickness;

            for (auto segment = 0u; segment < segments; segment++)
            {
                float step = (segment / static_cast<float>(segments)) * 3.14159265359f * 2.0f;
                auto& vtx = ring[segment];

                // fetch rotation
                const auto rotation = _rotations[circle_index];

                Vector3D pos;
                pos.x = static_cast<float>(cos(step + rotation));
                pos.y = static_cast<float>(sin(step + rotation));
                pos.x *= radius;
                pos.y *= radius;

                // move shape 
                pos.x += _centers[circle_index].x;
                pos.y += _centers[circle_index].y;

                // update vertex
                vtx.x = pos.x;
                vtx.y = pos.y;
            }
        }
    }
}
