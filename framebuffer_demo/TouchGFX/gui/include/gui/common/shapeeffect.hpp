#pragma once

#include "gui/common/vector.hpp"
#include "gui/common/vertex.hpp"
#include "gui/common/rastersurface.hpp"
#include "gui/common/touchinput.hpp"
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/hal/Types.hpp>

#include <array>


/*! \brief The class \c ShapeEffect displays a number of shapes that follow the touch input.
* 
*   The effect draws n vertex-based circles. The rendering code is triangle based.
 */
class ShapeEffect : public touchgfx::Widget
{
public:

    //! set up timer events and enable touch support
    ShapeEffect();
    ~ShapeEffect() override;
    
    //! the whole effect is transparent; therefore return an empty area here
    touchgfx::Rect getSolidRect() const override;

    //! draw the shapes
    void draw(const touchgfx::Rect& invalidatedArea) const override;

    //! invalidate every tick
    void handleTickEvent() override;

    //! move center of effect around
    void handleClickEvent(const touchgfx::ClickEvent& event) override;

    //! move center of effect around
    void handleDragEvent(const touchgfx::DragEvent& event) override;


private:

    void drawVertices(const RasterSurface& surface) const;
    void updateVertices();
    void updateRotation();
    void updateCenters();

    TouchInput _input;
    float _time = 0.0f;
    
    Vector3D _pos;
    float _rot = 0.0f;

    static constexpr uint32_t circles = 50;
    static constexpr uint32_t segments = 6;

    //! one circle has two rings, each ring has n segments
    using Ring = std::array<Vertex, segments>;
    using Circle = std::array<Ring, 2>;

    std::array<Circle, circles> _circles = {};

    std::array<float, circles> _rotations = {};
    std::array<Vector3D, circles> _centers = {};
};

