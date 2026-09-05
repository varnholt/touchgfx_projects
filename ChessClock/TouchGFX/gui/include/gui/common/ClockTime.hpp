#ifndef CLOCKTIME_HPP
#define CLOCKTIME_HPP

#include <stdint.h>

/**
 * A free running millisecond counter.
 *
 * The chess clock must not count display frames: the TouchGFX tick rate follows the
 * panel refresh, which is only approximately 60 Hz, and the resulting drift would be
 * plainly visible over a 30 minute game. Instead both platforms provide a real time
 * base - HAL_GetTick() on the STM32 and a steady clock in the simulator.
 */
namespace clocktime
{
/** Milliseconds since an arbitrary start point. Wraps every ~49 days. */
uint32_t millis();
}

#endif // CLOCKTIME_HPP
