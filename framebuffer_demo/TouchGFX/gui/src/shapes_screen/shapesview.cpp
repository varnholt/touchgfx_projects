#include <gui/shapes_screen/shapesview.hpp>

ShapesView::ShapesView()
{
    // configure effect
    _effect.setWidth(HAL::DISPLAY_WIDTH);
    _effect.setHeight(HAL::DISPLAY_HEIGHT);
    _effect.setXY(0, 0);

    // insert the effect at the very bottom of the stack
    container.insert(&_background, _effect);

    // set up FPS updates
    _fps._callback = [this](uint8_t fps) {updateFPS(fps); };

    // enable frame rate compensation to enable FPS counter
    HAL::getInstance()->setFrameRateCompensation(true);
}


void ShapesView::handleTickEvent()
{
    _fps.update();
}


void ShapesView::updateFPS(uint8_t fps)
{
    Unicode::snprintf(_fpsNumberBuffer, 3, "%d", fps);
    _fpsNumber.invalidate();
}

