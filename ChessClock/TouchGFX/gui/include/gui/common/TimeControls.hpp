#ifndef TIMECONTROLS_HPP
#define TIMECONTROLS_HPP

#include <stdint.h>

/**
 * U+221E INFINITY, shown wherever a time control has no limit. The glyph is part
 * of the Value typography wildcard range in assets/texts/texts.xml.
 */
const uint16_t UNLIMITED_GLYPH = 0x221E;

/** One preset on the settings screen: base time plus Fischer increment. */
struct TimeControl
{
    uint16_t    minutes;    ///< 0 means no time limit
    uint16_t    increment;  ///< seconds added after each completed move
    const char* category;
};

extern const TimeControl TIME_CONTROLS[];
extern const uint8_t NUM_TIME_CONTROLS;

#endif // TIMECONTROLS_HPP
