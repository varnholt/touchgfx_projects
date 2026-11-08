#pragma once

#include <gui/common/fps.hpp>
#include <gui/common/toruseffect.hpp>
#include <gui_generated/torus_screen/TorusViewBase.hpp>
#include <gui/torus_screen/toruspresenter.hpp>
#include <touchgfx/mixins/Draggable.hpp>


/*! \brief TorusView is a container class for a torus effect.
 *
 *  It passes in a quality setting from TouchGFX and displays the current FPS.
 */
class TorusView : public TorusViewBase
{

public:

    TorusView() = default;

    //! configure the effect and set up FPS
    void setupScreen() override;

    //! setter for the effect quality
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
    TorusEffect _effect;
    Fps _fps;
};

