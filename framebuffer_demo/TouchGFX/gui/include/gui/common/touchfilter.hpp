#pragma once

#include <cstdlib>

class TouchFilter
{
public:
    bool sample(bool down, int rawX, int rawY, int width, int height, int& x, int& y)
    {
        if (!down || rawX < 0 || rawY < 0 || rawX >= width || rawY >= height)
        {
            reset();
            return false;
        }
        if (!_active || std::abs(rawX - _x) + std::abs(rawY - _y) > 5)
        {
            _x = rawX;
            _y = rawY;
        }
        _active = true;
        x = _x;
        y = _y;
        return true;
    }
    void reset() { _active = false; }

private:
    bool _active = false;
    int _x = 0;
    int _y = 0;
};
