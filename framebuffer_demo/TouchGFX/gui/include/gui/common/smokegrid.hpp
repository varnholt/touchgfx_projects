#pragma once

// The solver keeps one boundary cell around a zero-based interior grid.
template <int Resolution>
struct SmokeGrid
{
    static constexpr int index(int x, int y)
    {
        return x + 1 + (Resolution + 2) * (y + 1);
    }

    static int inputIndex(int x, int y, int width, int height)
    {
        if (width <= 0 || height <= 0 || x < 0 || y < 0 || x >= width || y >= height)
        {
            return -1;
        }
        return index(x * Resolution / width, y * Resolution / height);
    }
};
