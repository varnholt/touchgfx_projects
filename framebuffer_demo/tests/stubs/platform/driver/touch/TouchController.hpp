#pragma once

#include <cstdint>
#include <cassert>

// Minimal interface stub for adapter tests without the TouchGFX SDK.
namespace touchgfx
{
class TouchController
{
public:
    virtual ~TouchController() = default;
    virtual void init() = 0;
    virtual bool sampleTouch(int32_t& x, int32_t& y) = 0;
};
}
