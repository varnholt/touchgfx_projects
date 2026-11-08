#pragma once

#include <functional>
#include <gui/common/frontendapplication.hpp>
#include <touchgfx/hal/HAL.hpp>

/*! \brief Fps counts the frames per second
 *
 *  The class has a callback that is called once the FPS value changes. 
 *  In order to work, framerate compensation needs to be enabled by calling
 *  HAL::getInstance()->setFrameRateCompensation(true);
 */
struct Fps
{
    //! called every tick, store skipped and processed frames
    void update()
    {
        if (HAL::getInstance()->getLCDRefreshCount() > 1)
        {
            _frameSkippedCounter++;
        }
        else
        {
            _frameCounter++;
        }

        if (_frameCounter + _frameSkippedCounter >= 60)
        {
            _fps_prev = _fps;
            _fps = _frameCounter;

            if (_fps_prev != _fps && _callback)
            {
                _callback(_fps);
            }
            
            reset();
        }
    }

    //! called when 60 frames were processed, reset all counters
    void reset()
    {
        _frameCounter = 0;
        _frameSkippedCounter = 0;   
    }

    uint32_t _frameCounter = 0;
    uint32_t _frameSkippedCounter = 0;
    uint8_t _fps = 0u;
    uint8_t _fps_prev = 0u;
    std::function<void(uint8_t)> _callback;
};

