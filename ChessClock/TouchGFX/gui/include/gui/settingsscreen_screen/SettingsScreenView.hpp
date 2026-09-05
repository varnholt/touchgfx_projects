#ifndef SETTINGSSCREENVIEW_HPP
#define SETTINGSSCREENVIEW_HPP

#include <gui_generated/settingsscreen_screen/SettingsScreenViewBase.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>
#include <gui/common/TimeControls.hpp>

/**
 * Picks the time control: six presets plus a pair of steppers for anything else.
 *
 * Edits are held locally until START GAME is pressed, so CANCEL really does leave the
 * game that is already on the clock untouched.
 */
class SettingsScreenView : public SettingsScreenViewBase
{
public:
    /** Number of preset cells the Designer laid out on this screen. */
    static const int PRESET_CELLS = 8;

    SettingsScreenView();
    virtual ~SettingsScreenView() {}

    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);

private:
    enum ControlId
    {
        CONTROL_NONE = -1,
        // 0 .. PRESET_CELLS-1 are the preset cells
        CONTROL_MINUTES_MINUS = PRESET_CELLS,
        CONTROL_MINUTES_PLUS,
        CONTROL_INCREMENT_MINUS,
        CONTROL_INCREMENT_PLUS,
        CONTROL_CANCEL,
        CONTROL_APPLY,
        CONTROL_COUNT
    };

    void fillPresetLabels();
    void refresh();
    int  controlAt(int16_t x, int16_t y);
    void activateControl(int id);
    touchgfx::Box* controlBox(int id);

    // Designer widgets gathered into arrays, so the six identical cells and the
    // four identical steppers can be driven by a loop.
    touchgfx::Box*                     presetBg[PRESET_CELLS];
    touchgfx::TextAreaWithOneWildcard* presetVal[PRESET_CELLS];
    touchgfx::TextAreaWithOneWildcard* presetSub[PRESET_CELLS];
    touchgfx::Unicode::UnicodeChar*    presetValBuf[PRESET_CELLS];
    touchgfx::Unicode::UnicodeChar*    presetSubBuf[PRESET_CELLS];

    uint16_t minutes;
    uint16_t increment;
    int      pressedControl;
};

#endif // SETTINGSSCREENVIEW_HPP
