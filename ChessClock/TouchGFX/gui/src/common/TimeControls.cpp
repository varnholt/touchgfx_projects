#include <gui/common/TimeControls.hpp>

// The eight presets shown on the settings screen, laid out in a 4x2 grid.
// Zero minutes means no time limit; the clocks then count up (see Model).
const TimeControl TIME_CONTROLS[] = {
    {  0,  0, "UNLIMITED" },
    {  1,  0, "BULLET"    },
    {  2,  1, "BULLET"    },
    {  3,  2, "BLITZ"     },
    {  5,  0, "BLITZ"     },
    { 10,  0, "RAPID"     },
    { 15, 10, "CLASSICAL" },
    { 30,  0, "CLASSICAL" }
};

const uint8_t NUM_TIME_CONTROLS = sizeof(TIME_CONTROLS) / sizeof(TIME_CONTROLS[0]);
