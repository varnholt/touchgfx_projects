#pragma once

#include <gui/common/frontendheap.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include <SDL2/SDL_surface.h>

// Runs against the real framework and view lifecycle, then exits without a UI loop.
inline bool runSimulatorSelfTest(touchgfx::HALSDL2& hal)
{
    auto& app = FrontendHeap::getInstance().app;
    std::ofstream log("self-test.log");
    if (!log) return false;
    const auto frame = [&hal]() { hal.vSync(); hal.backPorchExited(); hal.frontPorchEntered(); };
    uint16_t timerCounts[4] = {};
    char names[][12] = {"smoke.bmp", "torus.bmp", "shapes.bmp", "home.bmp"};
    for (int cycle = 0; cycle < 12; ++cycle)
        for (int screen = 0; screen < 4; ++screen)
        {
            switch (screen)
            {
            case 0: app.gotoSmokeScreenNoTransition(); break;
            case 1: app.gotoTorusScreenNoTransition(); break;
            case 2: app.gotoShapesScreenNoTransition(); break;
            default: app.gotohomeScreenNoTransition(); break;
            }
            frame(); frame();
            auto* view = app.getCurrentScreen();
            if (!view) { log << "FAIL: missing screen\n"; return false; }
            const auto timers = app.getNumberOfRegisteredTimerWidgets();
            if (cycle == 0) timerCounts[screen] = timers;
            else if (timers != timerCounts[screen])
            {
                log << "FAIL: screen " << screen << " timer count changed from "
                    << timerCounts[screen] << " to " << timers << "\n";
                return false;
            }
            for (int quality = 0; quality < 3; ++quality)
            {
                const Quality setting = quality == 0 ? Quality::LOW : quality == 1 ? Quality::MEDIUM : Quality::HIGH;
                if (screen == 0) static_cast<SmokeView*>(view)->setQuality(setting);
                if (screen == 1) static_cast<TorusView*>(view)->setQuality(setting);
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::PRESSED, 200, 200));
                app.handleDragEvent(touchgfx::DragEvent(touchgfx::DragEvent::DRAGGED, 200, 200, 220, 210));
                app.handleDragEvent(touchgfx::DragEvent(touchgfx::DragEvent::DRAGGED, 220, 210, 240, 220));
                frame();
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::RELEASED, 240, 220));
                frame();
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::PRESSED, 200, 200));
                app.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::CANCEL, 200, 200));
                frame();
            }
            // Capture actual framebuffer pixels: hidden SDL windows may have a black renderer.
            app.draw();
            const int stride = touchgfx::HAL::lcd().framebufferStride() / 2;
            const int width = touchgfx::HAL::DISPLAY_WIDTH, height = touchgfx::HAL::DISPLAY_HEIGHT;
            auto* buffer = hal.lockFrameBuffer();
            const bool nonempty = std::any_of(buffer, buffer + stride * height,
                                              [](uint16_t pixel) { return pixel != 0; });
            if (!nonempty)
            {
                hal.unlockFrameBuffer();
                log << "FAIL: empty framebuffer for screen " << screen << "\n";
                return false;
            }
            if (cycle == 0)
            {
                SDL_Surface* image = SDL_CreateRGBSurfaceFrom(buffer, width, height, 16,
                    stride * 2, 0xf800, 0x07e0, 0x001f, 0);
                if (!image || SDL_SaveBMP(image, names[screen]) != 0)
                {
                    if (image) SDL_FreeSurface(image);
                    hal.unlockFrameBuffer();
                    log << "FAIL: framebuffer snapshot\n";
                    return false;
                }
                SDL_FreeSurface(image);
            }
            std::vector<uint16_t> before(buffer, buffer + stride * height);
            hal.unlockFrameBuffer();
            app.draw();
            buffer = hal.lockFrameBuffer();
            const bool stable = std::equal(before.begin(), before.end(), buffer);
            // Paint a sentinel to expose writes outside the partial invalidation.
            std::fill(buffer, buffer + stride * height, static_cast<uint16_t>(0x5a5a));
            hal.unlockFrameBuffer();
            touchgfx::Rect partial(123, 71, 29, 17);
            app.draw(partial);
            buffer = hal.lockFrameBuffer();
            bool clipped = true;
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                    if (!(x >= 123 && x < 152 && y >= 71 && y < 88) &&
                        buffer[y * stride + x] != 0x5a5a) clipped = false;
            hal.unlockFrameBuffer();
            if (!stable || !clipped)
            {
                log << "FAIL: screen " << screen << " repeat draw stable=" << stable
                    << ", partial redraw clipped=" << clipped << "\n";
                return false;
            }
        }
    const auto baselineTimers = app.getNumberOfRegisteredTimerWidgets();
    for (int size = 0; size < 2; ++size)
    {
        SmokeEffect effect;
        const int widgetWidth = size == 0 ? 83 : 7, widgetHeight = size == 0 ? 47 : 5;
        effect.setXY(11, 13);
        effect.setWidth(static_cast<int16_t>(widgetWidth));
        effect.setHeight(static_cast<int16_t>(widgetHeight));
        effect.handleClickEvent(touchgfx::ClickEvent(touchgfx::ClickEvent::PRESSED, 3, 2));
        effect.handleTickEvent();
        const int stride = touchgfx::HAL::lcd().framebufferStride() / 2;
        const int width = touchgfx::HAL::DISPLAY_WIDTH, height = touchgfx::HAL::DISPLAY_HEIGHT;
        for (int quality = 0; quality < 3; ++quality)
        {
            if (quality == 0) effect.setLowQuality();
            else if (quality == 1) effect.setMediumQuality();
            else effect.setHighQuality();
            for (int partial = 0; partial < 2; ++partial)
            {
                auto* buffer = hal.lockFrameBuffer();
                std::fill(buffer, buffer + stride * height, static_cast<uint16_t>(0x5a5a));
                hal.unlockFrameBuffer();
                touchgfx::Rect invalidated(partial ? 2 : 0, partial ? 1 : 0,
                    static_cast<int16_t>(partial ? widgetWidth - 3 : widgetWidth),
                    static_cast<int16_t>(partial ? widgetHeight - 2 : widgetHeight));
                effect.draw(invalidated);
                buffer = hal.lockFrameBuffer();
                bool correct = true;
                for (int y = 0; y < height; ++y)
                    for (int x = 0; x < width; ++x)
                    {
                        const bool inside = x >= 11 + invalidated.x && y >= 13 + invalidated.y &&
                            x < 11 + invalidated.right() && y < 13 + invalidated.bottom();
                        if ((buffer[y * stride + x] != 0x5a5a) != inside) correct = false;
                    }
                hal.unlockFrameBuffer();
                if (!correct)
                {
                    log << "FAIL: smoke geometry size=" << size << " quality=" << quality
                        << " partial=" << partial << "\n";
                    return false;
                }
            }
        }
    }
    // The framework compacts unregistered timer entries during its next tick.
    frame();
    if (app.getNumberOfRegisteredTimerWidgets() != baselineTimers)
    {
        log << "FAIL: temporary widget timer lifecycle: " << baselineTimers << " -> "
            << app.getNumberOfRegisteredTimerWidgets() << "\n";
        return false;
    }
    log << "PASS: 48 screen transitions, all quality modes, press/drag/release/cancel, stable redraws, clipped partial redraws, widget offsets and sizes, stable timer counts\n";
    return true;
}
