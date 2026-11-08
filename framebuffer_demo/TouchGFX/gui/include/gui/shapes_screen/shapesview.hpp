#pragma once

#include "gui/common/datatypes.hpp"
#include "gui/common/fps.hpp"
#include "gui/common/shapeeffect.hpp"
#include <gui_generated/shapes_screen/ShapesViewBase.hpp>
#include <gui/shapes_screen/shapespresenter.hpp>

/*! \brief ShapesView is a container class for the \c ShapeEffect implementation.
 *
 *  It passes events from the TouchGFX framework to the ShapeEffect such as the
 *  display quality updates. Moreoever, it implements a simple FPS display.
 */
class ShapesView : public ShapesViewBase
{
public:

    //! set up the shape effect and enable frame rate compensation
    ShapesView();

    void handleTickEvent() override;

    //! Control the optional diagnostic HUD, including clean gallery captures.
    void setPerformanceCounterVisible(bool visible)
    {
        _fpsText.setVisible(visible);
        _fpsNumber.setVisible(visible);
    }

private:

    void updateFPS(uint8_t fps);
    ShapeEffect _effect;
    Fps _fps;
};

