#include <gui/common/ClockTime.hpp>

#ifdef SIMULATOR

#include <chrono>

uint32_t clocktime::millis()
{
    static const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    const std::chrono::steady_clock::duration since = std::chrono::steady_clock::now() - start;
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(since).count());
}

#else

// Declared rather than included, so this file does not depend on the CubeMX HAL
// headers being on the include path of the TouchGFX sources.
extern "C" uint32_t HAL_GetTick(void);

uint32_t clocktime::millis()
{
    return HAL_GetTick();
}

#endif
