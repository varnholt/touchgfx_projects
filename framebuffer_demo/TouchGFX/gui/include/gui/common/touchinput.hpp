#pragma once

// A completed tap survives until the next tick; cancellation discards pending work.
class TouchInput
{
public:
    struct Sample
    {
        int x = 0;
        int y = 0;
        bool pressed = false;
        int dx = 0;
        int dy = 0;
    };

    void press(int x, int y, int width, int height)
    {
        cancel();
        _active = inside(x, y, width, height);
        if (_active)
        {
            _pending.x = x;
            _pending.y = y;
            _pending.pressed = true;
        }
    }

    void drag(int oldX, int oldY, int x, int y, int width, int height)
    {
        if (!_active || !inside(x, y, width, height)) return;
        _pending.x = x;
        _pending.y = y;
        if (inside(oldX, oldY, width, height))
        {
            _pending.dx += x - oldX;
            _pending.dy += y - oldY;
        }
    }

    void release() { _active = false; }
    void cancel() { _active = false; _pending = Sample{}; }
    bool active() const { return _active; }
    Sample consume()
    {
        const Sample result = _pending;
        _pending.pressed = false;
        _pending.dx = _pending.dy = 0;
        return result;
    }

private:
    static bool inside(int x, int y, int width, int height)
    {
        return x >= 0 && y >= 0 && x < width && y < height;
    }
    bool _active = false;
    Sample _pending;
};
