#include <gui/settingsscreen_screen/SettingsScreenView.hpp>
#include <gui/common/Palette.hpp>

namespace
{
/** Coarser steps at the top end, so 90 minutes is not 89 taps away. */
uint16_t stepMinutes(uint16_t value, bool up)
{
    if (up)
    {
        const uint16_t step = (value < 10u) ? 1u : ((value < 60u) ? 5u : 15u);
        value = static_cast<uint16_t>(value + step);
    }
    else
    {
        const uint16_t step = (value <= 10u) ? 1u : ((value <= 60u) ? 5u : 15u);
        value = (value > step) ? static_cast<uint16_t>(value - step) : 0u;
    }
    if (value > Model::MAX_MINUTES)
    {
        value = Model::MAX_MINUTES;
    }
    return value; // stepping below one minute lands on 0, which means unlimited
}

uint16_t stepIncrement(uint16_t value, bool up)
{
    if (up)
    {
        const uint16_t step = (value < 10u) ? 1u : 5u;
        value = static_cast<uint16_t>(value + step);
    }
    else
    {
        const uint16_t step = (value <= 10u) ? 1u : 5u;
        value = (value > step) ? static_cast<uint16_t>(value - step) : 0u;
    }
    if (value > Model::MAX_INCREMENT)
    {
        value = Model::MAX_INCREMENT;
    }
    return value;
}

/** Writes the infinity sign, for a clock with no limit. */
void formatUnlimited(touchgfx::Unicode::UnicodeChar* buffer)
{
    buffer[0] = UNLIMITED_GLYPH;
    buffer[1] = 0;
}

/** A whole preset cell: either "M + I" or just the infinity sign. */
void formatTimeControl(uint16_t minutes, uint16_t increment,
                       touchgfx::Unicode::UnicodeChar* buffer, uint16_t size)
{
    if (minutes == 0)
    {
        formatUnlimited(buffer);
    }
    else
    {
        touchgfx::Unicode::snprintf(buffer, size, "%u + %u", minutes, increment);
    }
}

/** The minutes stepper shows the base time on its own, not the whole control. */
void formatMinutes(uint16_t minutes, touchgfx::Unicode::UnicodeChar* buffer, uint16_t size)
{
    if (minutes == 0)
    {
        formatUnlimited(buffer);
    }
    else
    {
        touchgfx::Unicode::snprintf(buffer, size, "%u", minutes);
    }
}
} // namespace

SettingsScreenView::SettingsScreenView()
    : minutes(0),
      increment(0),
      pressedControl(CONTROL_NONE)
{
    presetBg[0] = &preset0Bg;
    presetBg[1] = &preset1Bg;
    presetBg[2] = &preset2Bg;
    presetBg[3] = &preset3Bg;
    presetBg[4] = &preset4Bg;
    presetBg[5] = &preset5Bg;
    presetBg[6] = &preset6Bg;
    presetBg[7] = &preset7Bg;

    presetVal[0] = &preset0Val;
    presetVal[1] = &preset1Val;
    presetVal[2] = &preset2Val;
    presetVal[3] = &preset3Val;
    presetVal[4] = &preset4Val;
    presetVal[5] = &preset5Val;
    presetVal[6] = &preset6Val;
    presetVal[7] = &preset7Val;

    presetSub[0] = &preset0Sub;
    presetSub[1] = &preset1Sub;
    presetSub[2] = &preset2Sub;
    presetSub[3] = &preset3Sub;
    presetSub[4] = &preset4Sub;
    presetSub[5] = &preset5Sub;
    presetSub[6] = &preset6Sub;
    presetSub[7] = &preset7Sub;

    presetValBuf[0] = preset0ValBuffer;
    presetValBuf[1] = preset1ValBuffer;
    presetValBuf[2] = preset2ValBuffer;
    presetValBuf[3] = preset3ValBuffer;
    presetValBuf[4] = preset4ValBuffer;
    presetValBuf[5] = preset5ValBuffer;
    presetValBuf[6] = preset6ValBuffer;
    presetValBuf[7] = preset7ValBuffer;

    presetSubBuf[0] = preset0SubBuffer;
    presetSubBuf[1] = preset1SubBuffer;
    presetSubBuf[2] = preset2SubBuffer;
    presetSubBuf[3] = preset3SubBuffer;
    presetSubBuf[4] = preset4SubBuffer;
    presetSubBuf[5] = preset5SubBuffer;
    presetSubBuf[6] = preset6SubBuffer;
    presetSubBuf[7] = preset7SubBuffer;
}

void SettingsScreenView::setupScreen()
{
    SettingsScreenViewBase::setupScreen();

    minutes = presenter->getMinutes();
    increment = presenter->getIncrement();

    fillPresetLabels();
    refresh();
}

void SettingsScreenView::tearDownScreen()
{
    SettingsScreenViewBase::tearDownScreen();
}

void SettingsScreenView::fillPresetLabels()
{
    for (int i = 0; i < PRESET_CELLS; i++)
    {
        if (i < static_cast<int>(NUM_TIME_CONTROLS))
        {
            formatTimeControl(TIME_CONTROLS[i].minutes, TIME_CONTROLS[i].increment,
                              presetValBuf[i], PRESET0VAL_SIZE);
            touchgfx::Unicode::strncpy(presetSubBuf[i], TIME_CONTROLS[i].category, PRESET0SUB_SIZE);
        }
        else
        {
            // No preset defined for this cell: leave it blank and inert.
            presetValBuf[i][0] = 0;
            presetSubBuf[i][0] = 0;
        }
    }
}

void SettingsScreenView::refresh()
{
    // With no clock there is nothing for an increment to be added to, so the
    // whole increment group is greyed out and stops taking taps.
    const bool unlimited = (minutes == 0);

    formatMinutes(minutes, minValueBuffer, MINVALUE_SIZE);
    touchgfx::Unicode::snprintf(incValueBuffer, INCVALUE_SIZE, "%u", increment);

    for (int i = 0; i < PRESET_CELLS; i++)
    {
        const bool defined = i < static_cast<int>(NUM_TIME_CONTROLS);
        const bool selected = defined &&
                              TIME_CONTROLS[i].minutes == minutes &&
                              TIME_CONTROLS[i].increment == increment;

        touchgfx::colortype c = palette::button();
        if (!defined)
        {
            c = palette::buttonDisabled();
        }
        else if (i == pressedControl)
        {
            c = palette::buttonPressed();
        }
        else if (selected)
        {
            c = palette::buttonSelected();
        }
        presetBg[i]->setColor(c);
        presetVal[i]->setColor(selected ? palette::textBright() : palette::textIdle());
        presetSub[i]->setColor(selected ? palette::accent() : palette::textDim());
    }

    for (int id = CONTROL_MINUTES_MINUS; id < CONTROL_COUNT; id++)
    {
        const bool inertIncrement = unlimited && (id == CONTROL_INCREMENT_MINUS ||
                                                  id == CONTROL_INCREMENT_PLUS);
        touchgfx::colortype c = (id == CONTROL_APPLY) ? palette::buttonGo() : palette::button();
        if (inertIncrement)
        {
            c = palette::buttonDisabled();
        }
        else if (id == pressedControl)
        {
            c = palette::buttonPressed();
        }
        controlBox(id)->setColor(c);
    }

    const touchgfx::colortype incColor = unlimited ? palette::textDisabled() : palette::textBright();
    incValueBg.setColor(unlimited ? palette::buttonDisabled() : palette::panelIdle());
    incValue.setColor(incColor);
    incMinus.setColor(incColor);
    incPlus.setColor(incColor);
    incLabel.setColor(unlimited ? palette::textDisabled() : palette::textDim());

    // settingsBg spans the whole screen, so this repaints every widget on it.
    settingsBg.invalidate();
}

touchgfx::Box* SettingsScreenView::controlBox(int id)
{
    if (id >= 0 && id < PRESET_CELLS)
    {
        return presetBg[id];
    }
    switch (id)
    {
    case CONTROL_MINUTES_MINUS:
        return &minMinusBg;
    case CONTROL_MINUTES_PLUS:
        return &minPlusBg;
    case CONTROL_INCREMENT_MINUS:
        return &incMinusBg;
    case CONTROL_INCREMENT_PLUS:
        return &incPlusBg;
    case CONTROL_CANCEL:
        return &btnCancelBg;
    case CONTROL_APPLY:
    default:
        return &btnApplyBg;
    }
}

int SettingsScreenView::controlAt(int16_t x, int16_t y)
{
    for (int i = 0; i < PRESET_CELLS; i++)
    {
        if (i < static_cast<int>(NUM_TIME_CONTROLS) && presetBg[i]->getAbsoluteRect().intersect(x, y))
        {
            return i;
        }
    }
    for (int id = CONTROL_MINUTES_MINUS; id < CONTROL_COUNT; id++)
    {
        if (minutes == 0 && (id == CONTROL_INCREMENT_MINUS || id == CONTROL_INCREMENT_PLUS))
        {
            continue; // increment group is inert while there is no clock
        }
        if (controlBox(id)->getAbsoluteRect().intersect(x, y))
        {
            return id;
        }
    }
    return CONTROL_NONE;
}

void SettingsScreenView::activateControl(int id)
{
    if (id >= 0 && id < static_cast<int>(NUM_TIME_CONTROLS))
    {
        minutes = TIME_CONTROLS[id].minutes;
        increment = TIME_CONTROLS[id].increment;
        refresh();
        return;
    }

    switch (id)
    {
    case CONTROL_MINUTES_MINUS:
        minutes = stepMinutes(minutes, false);
        break;
    case CONTROL_MINUTES_PLUS:
        minutes = stepMinutes(minutes, true);
        break;
    case CONTROL_INCREMENT_MINUS:
        increment = stepIncrement(increment, false);
        break;
    case CONTROL_INCREMENT_PLUS:
        increment = stepIncrement(increment, true);
        break;
    case CONTROL_CANCEL:
        presenter->gotoGame();
        return; // this screen is about to be torn down
    case CONTROL_APPLY:
        presenter->applyTimeControl(minutes, increment);
        return; // this screen is about to be torn down
    default:
        break;
    }
    if (minutes == 0)
    {
        increment = 0; // keep the greyed out reading honest
    }
    refresh();
}

void SettingsScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    const int16_t x = evt.getX();
    const int16_t y = evt.getY();

    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        pressedControl = controlAt(x, y);
        if (pressedControl != CONTROL_NONE)
        {
            refresh();
        }
    }
    else // RELEASED or CANCEL
    {
        const int released = pressedControl;
        pressedControl = CONTROL_NONE;
        if (released == CONTROL_NONE)
        {
            return;
        }
        // Only fire if the finger came up over the control it went down on.
        if (evt.getType() == touchgfx::ClickEvent::RELEASED && controlAt(x, y) == released)
        {
            activateControl(released);
        }
        else
        {
            refresh();
        }
    }
}
