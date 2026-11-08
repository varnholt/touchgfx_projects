#pragma once

#include <gui/common/frontendheap.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <SDL2/SDL_surface.h>
#include <fstream>

// Capture representative views separately from the stress-test input sequences.
inline bool captureSimulatorPreviews(touchgfx::HALSDL2& hal)
{
    auto& app = FrontendHeap::getInstance().app;
    std::ofstream log("preview-captures.log");
    if (!log) return false;
    const auto frame = [&hal]() { hal.vSync(); hal.backPorchExited(); hal.frontPorchEntered(); };
    const char* names[] = {"home.bmp", "torus.bmp", "smoke.bmp", "shapes.bmp"};
    for (int screen = 0; screen < 4; ++screen)
    {
        switch (screen)
        {
        case 0: app.gotohomeScreenNoTransition(); break;
        case 1: app.gotoTorusScreenNoTransition(); break;
        case 2: app.gotoSmokeScreenNoTransition(); break;
        default: app.gotoShapesScreenNoTransition(); break;
        }
        frame(); frame();
        if (screen == 1) static_cast<TorusView*>(app.getCurrentScreen())->setQuality(Quality::HIGH);
        if (screen == 2) static_cast<SmokeView*>(app.getCurrentScreen())->setQuality(Quality::HIGH);
        if (screen == 1) static_cast<TorusView*>(app.getCurrentScreen())->setPerformanceCounterVisible(false);
        if (screen == 2) static_cast<SmokeView*>(app.getCurrentScreen())->setPerformanceCounterVisible(false);
        if (screen == 3) static_cast<ShapesView*>(app.getCurrentScreen())->setPerformanceCounterVisible(false);
        const int ticks = screen == 2 ? 480 : 120;
        for (int tick = 0; tick < ticks; ++tick)
        {
            // A slight tilt keeps the torus opening clearly visible.
            if (screen == 1 && tick == 0)
            {
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::PRESSED, 400, 240));
                app.handleDragEvent(touchgfx::DragEvent(touchgfx::DragEvent::DRAGGED, 400, 240, 404, 246));
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::RELEASED, 404, 246));
            }
            frame();
        }
        app.draw();
        auto* pixels = hal.lockFrameBuffer();
        SDL_Surface* image = SDL_CreateRGBSurfaceFrom(pixels, touchgfx::HAL::DISPLAY_WIDTH,
            touchgfx::HAL::DISPLAY_HEIGHT, 16, touchgfx::HAL::lcd().framebufferStride(),
            0xf800, 0x07e0, 0x001f, 0);
        const bool saved = image && SDL_SaveBMP(image, names[screen]) == 0;
        if (image) SDL_FreeSurface(image);
        hal.unlockFrameBuffer();
        if (!saved) { log << "FAIL: " << names[screen] << "\n"; return false; }
    }
    log << "PASS: Home, Torus, Smoke and Shapes preview captures\n";
    return true;
}
