#pragma once

#include "solver.h"
#include "smokegrid.hpp"
#include "touchinput.hpp"
#include <array>
#include <cstdlib>
#include <cstdint>

// Fluid state is independent of widgets, framebuffers and drawing frequency.
class SmokeSimulation
{
public:
    static constexpr int resolution = 80;
    static constexpr int size = (resolution + 2) * (resolution + 2);
    using Grid = SmokeGrid<resolution>;

    TouchInput& input() { return _input; }
    void wake() { _idleTicks = 0; }
    float density(int x, int y) const { return _density[Grid::index(x, y)]; }

    void reset()
    {
        _u.fill(0); _v.fill(0); _density.fill(0);
        _uSource.fill(0); _vSource.fill(0); _densitySource.fill(0);
        _input.cancel();
        _tick = _idleTicks = 0;
    }

    void step(int width, int height, const uint8_t* overlay = nullptr,
              int overlayWidth = 0, int overlayHeight = 0)
    {
        constexpr float dt = 0.1f;
        _uSource.fill(0); _vSource.fill(0); _densitySource.fill(0);
        if (_tick <= 150 && overlay && overlayWidth > 0 && overlayHeight > 0)
        {
            for (int y = 0; y < resolution; ++y)
                for (int x = 0; x < resolution; ++x)
                {
                    const int index = Grid::index(x, y);
                    const int px = x * overlayWidth / resolution;
                    const int py = y * overlayHeight / resolution;
                    // ARGB8888 is stored as B,G,R,A on these little-endian targets.
                    _densitySource[index] += 0.4f * overlay[4 * (py * overlayWidth + px) + 2] / 255;
                    if (_tick % 10 == 0)
                    {
                        _uSource[index] = 0.05f * (x / static_cast<float>(resolution) - 0.5f) +
                            0.01f * (std::rand() / static_cast<float>(RAND_MAX) - 0.5f);
                        _vSource[index] = 0.05f * (y / static_cast<float>(resolution) - 0.5f) +
                            0.01f * (std::rand() / static_cast<float>(RAND_MAX) - 0.5f);
                    }
                }
        }
        const auto touch = _input.consume();
        const int index = Grid::inputIndex(touch.x, touch.y, width, height);
        if (index >= 0)
        {
            _uSource[index] += 5.0f * touch.dx;
            _vSource[index] += 5.0f * touch.dy;
            if (touch.pressed) _densitySource[index] += 100.0f;
        }
        if (_tick == 400)
        {
            _uSource[Grid::index(60, 30)] -= 1000;
            _uSource[Grid::index(20, 50)] += 1000;
        }
        _solver.solveVelocity(resolution, _u.data(), _v.data(), _uSource.data(), _vSource.data(), 0, dt);
        _solver.solveDensity(resolution, _density.data(), _densitySource.data(), _u.data(), _v.data(), 0, dt);
        ++_tick;
        if (++_idleTicks > 5000)
        {
            for (int i = 0; i < size; ++i)
            {
                _u[i] *= 0.98f;
                _v[i] *= 0.98f;
                _density[i] *= 0.98f;
            }
        }
        if (_idleTicks >= 6000) reset();
    }

private:
    Solver _solver;
    TouchInput _input;
    std::array<float, size> _u = {}, _v = {}, _density = {};
    std::array<float, size> _uSource = {}, _vSource = {}, _densitySource = {};
    uint32_t _tick = 0, _idleTicks = 0;
};
