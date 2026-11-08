#pragma once

#include <gui_generated/smoke_screen/SmokeViewBase.hpp>

#include <gui/common/fps.hpp>
#include <gui/common/smokeeffect.hpp>
#include <gui/smoke_screen/smokepresenter.hpp>
#include <touchgfx/mixins/Draggable.hpp>


/*! \brief SmokeView is a container class for the \c SmokeEffect implementation.
 *
 *  It passes events from the TouchGFX framework to the SmokeEffect such as the
 *  display quality updates. Moreoever, it implements a simple FPS display.
 */
class SmokeView : public SmokeViewBase
{
public:

    SmokeView() = default;

    //! set up the smoke effect and enable frame rate compensation
    void setupScreen() override;

    //! setter for the three different quality types
    void setQuality(Quality value) override;


    //! Control the optional diagnostic HUD, including clean gallery captures.
    void setPerformanceCounterVisible(bool visible)
    {
        _fpsText.setVisible(visible);
        _fpsNumber.setVisible(visible);
    }

protected:

    void handleTickEvent() override;
    void tearDownScreen() override;


private:

    void updateFPS(uint8_t fps);
    SmokeEffect _effect;
    Fps _fps;
};

