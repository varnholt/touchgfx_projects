#ifndef PALETTE_HPP
#define PALETTE_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/hal/Types.hpp>

/** The colours used across both screens, in one place. */
namespace palette
{
inline touchgfx::colortype rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return touchgfx::Color::getColorFromRGB(r, g, b);
}

inline touchgfx::colortype background()  { return rgb(0x0B, 0x0D, 0x12); }

// Player halves
inline touchgfx::colortype panelIdle()   { return rgb(0x17, 0x1A, 0x22); }
inline touchgfx::colortype panelActive() { return rgb(0x1F, 0x6F, 0x4A); }
inline touchgfx::colortype panelPaused() { return rgb(0x33, 0x46, 0x60); }
inline touchgfx::colortype panelWarn()   { return rgb(0xB4, 0x79, 0x1C); }
inline touchgfx::colortype panelDanger() { return rgb(0xA8, 0x28, 0x1E); }
inline touchgfx::colortype panelBlink()  { return rgb(0x5E, 0x16, 0x11); }
inline touchgfx::colortype panelFlag()   { return rgb(0x6E, 0x14, 0x10); }

// Buttons
inline touchgfx::colortype button()        { return rgb(0x2A, 0x2F, 0x3D); }
inline touchgfx::colortype buttonPressed() { return rgb(0x45, 0x4C, 0x60); }
inline touchgfx::colortype buttonDisabled(){ return rgb(0x1A, 0x1D, 0x25); }
inline touchgfx::colortype buttonGo()      { return rgb(0x1F, 0x6F, 0x4A); }
inline touchgfx::colortype buttonSelected(){ return rgb(0x2C, 0x59, 0x77); }

// Text
inline touchgfx::colortype textBright()   { return rgb(0xFF, 0xFF, 0xFF); }
inline touchgfx::colortype textOnPanel()  { return rgb(0xE6, 0xEC, 0xF2); }
inline touchgfx::colortype textIdle()     { return rgb(0x8A, 0x92, 0xA5); }
inline touchgfx::colortype textDim()      { return rgb(0x7A, 0x81, 0x94); }
inline touchgfx::colortype textDisabled() { return rgb(0x4A, 0x50, 0x60); }
inline touchgfx::colortype accent()       { return rgb(0xE9, 0xC4, 0x6A); }

}

#endif // PALETTE_HPP
