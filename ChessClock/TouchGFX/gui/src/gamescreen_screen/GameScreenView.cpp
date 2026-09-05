#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <gui/common/Palette.hpp>

namespace
{
const uint32_t WARN_MS   = 30000u; ///< amber below this
const uint32_t DANGER_MS = 10000u; ///< red and blinking below this
const uint32_t TENTHS_MS = 20000u; ///< show tenths of a second below this

/** Renders the clock the way a tournament clock does. */
void formatClock(uint32_t ms, touchgfx::Unicode::UnicodeChar* buffer, uint16_t size, bool tenths)
{
    const uint32_t seconds = ms / 1000u;
    if (tenths)
    {
        touchgfx::Unicode::snprintf(buffer, size, "%02u.%u", seconds, (ms % 1000u) / 100u);
    }
    else if (seconds >= 3600u)
    {
        touchgfx::Unicode::snprintf(buffer, size, "%u:%02u:%02u",
                                    seconds / 3600u, (seconds / 60u) % 60u, seconds % 60u);
    }
    else
    {
        touchgfx::Unicode::snprintf(buffer, size, "%02u:%02u", seconds / 60u, seconds % 60u);
    }
}
} // namespace

GameScreenView::GameScreenView()
    : pressedButton(BUTTON_NONE),
      shownState(-1),
      shownMinutes(0xFFFFu),
      shownIncrement(0xFFFFu)
{
    for (int i = 0; i < 2; i++)
    {
        shownTimeMs[i] = 0xFFFFFFFFu;
        shownPanelColor[i] = 0xFFFFFFFFu;
        shownTimeColor[i] = 0xFFFFFFFFu;
        shownTrimColor[i] = 0xFFFFFFFFu;
        shownBarColor[i] = 0xFFFFFFFFu;
        shownMoves[i] = 0xFFFFu;
        shownPlateAlpha[i] = 0;
        shownCaption[i] = -1;
    }
    for (int i = 0; i < 3; i++)
    {
        shownButtonColor[i] = 0xFFFFFFFFu;
    }
}

void GameScreenView::setupScreen()
{
    GameScreenViewBase::setupScreen();
    refresh(true);
}

void GameScreenView::tearDownScreen()
{
    GameScreenViewBase::tearDownScreen();
}

void GameScreenView::handleTickEvent()
{
    refresh(false);
}

GameScreenView::SideWidgets GameScreenView::sideWidgets(Model::Side side)
{
    SideWidgets w;
    if (side == Model::WHITE)
    {
        w.panel = &panelLeft;
        w.turnBar = &turnBarLeft;
        w.plate = &plateLeft;
        w.name = &nameLeft;
        w.caption = &captionLeft;
        w.time = &timeLeft;
        w.moves = &movesLeft;
        w.captionBuffer = captionLeftBuffer;
        w.timeBuffer = timeLeftBuffer;
        w.movesBuffer = movesLeftBuffer;
    }
    else
    {
        w.panel = &panelRight;
        w.turnBar = &turnBarRight;
        w.plate = &plateRight;
        w.name = &nameRight;
        w.caption = &captionRight;
        w.time = &timeRight;
        w.moves = &movesRight;
        w.captionBuffer = captionRightBuffer;
        w.timeBuffer = timeRightBuffer;
        w.movesBuffer = movesRightBuffer;
    }
    return w;
}

void GameScreenView::refresh(bool force)
{
    refreshSide(Model::WHITE, force);
    refreshSide(Model::BLACK, force);
    refreshBottomBar(force);
}

void GameScreenView::refreshSide(Model::Side side, bool force)
{
    const Model& m = presenter->getModel();
    const int i = static_cast<int>(side);
    const SideWidgets w = sideWidgets(side);
    const Model::State state = m.getState();
    const bool unlimited = m.isUnlimited();
    const uint32_t ms = m.getClockMs(side);
    const bool onMove = (state == Model::RUNNING || state == Model::PAUSED) && m.getTurn() == side;

    // --- the clock ---------------------------------------------------------
    // Quantised to what the display actually shows, so the text is only rebuilt
    // when a visible digit changes.
    const bool tenths = !unlimited && ms < TENTHS_MS;
    const uint32_t quantised = tenths ? (ms / 100u) * 100u : (ms / 1000u) * 1000u;
    if (force || quantised != shownTimeMs[i])
    {
        shownTimeMs[i] = quantised;
        formatClock(ms, w.timeBuffer, TIMELEFT_SIZE, tenths);
        w.time->invalidate();
    }

    // --- colour of this half ----------------------------------------------
    // trimColor covers the player name and the move counter, which have to stay
    // legible once the half lights up green, amber or red.
    touchgfx::colortype panelColor = palette::panelIdle();
    touchgfx::colortype timeColor = palette::textIdle();
    touchgfx::colortype trimColor = (state == Model::FINISHED) ? palette::textBright() : palette::textDim();
    if (state == Model::FINISHED && m.getFlagged() == side)
    {
        panelColor = palette::panelFlag();
        timeColor = palette::textBright();
    }
    else if (onMove)
    {
        trimColor = palette::textOnPanel();
        if (state == Model::PAUSED)
        {
            panelColor = palette::panelPaused();
        }
        else if (unlimited)
        {
            panelColor = palette::panelActive(); // nothing to run out of
        }
        else if (ms < DANGER_MS)
        {
            // Blink twice a second over the last ten seconds.
            panelColor = ((ms / 500u) & 1u) ? palette::panelDanger() : palette::panelBlink();
        }
        else if (ms < WARN_MS)
        {
            panelColor = palette::panelWarn();
        }
        else
        {
            panelColor = palette::panelActive();
        }
        timeColor = palette::textBright();
    }

    if (force || panelColor.color != shownPanelColor[i] || timeColor.color != shownTimeColor[i] ||
        trimColor.color != shownTrimColor[i])
    {
        shownPanelColor[i] = panelColor.color;
        shownTimeColor[i] = timeColor.color;
        shownTrimColor[i] = trimColor.color;
        w.panel->setColor(panelColor);
        w.time->setColor(timeColor);
        w.moves->setColor(trimColor);
        // Invalidating the panel also repaints the texts drawn on top of it.
        w.panel->invalidate();
    }

    // --- turn bar along the outer edge -------------------------------------
    // White against whatever the half is filled with, and invisible otherwise.
    const touchgfx::colortype barColor = onMove ? palette::textBright() : panelColor;
    if (force || barColor.color != shownBarColor[i])
    {
        shownBarColor[i] = barColor.color;
        w.turnBar->setColor(barColor);
        w.turnBar->invalidate();
    }

    // --- name plate --------------------------------------------------------
    // The plate itself never changes colour - it stands for a piece colour. It
    // only fades back while the other player is on the clock, so the pair reads
    // as one crisp plate and one dimmed one.
    const bool someoneIsOnTheClock = (state == Model::RUNNING || state == Model::PAUSED);
    const uint8_t plateAlpha = (someoneIsOnTheClock && !onMove) ? PLATE_ALPHA_WAITING : 255;
    if (force || plateAlpha != shownPlateAlpha[i])
    {
        shownPlateAlpha[i] = plateAlpha;
        w.plate->setAlpha(plateAlpha);
        w.name->setAlpha(plateAlpha);
        w.caption->setAlpha(plateAlpha);
        w.plate->invalidate(); // covers the name and status line drawn on it
    }

    // --- status line -------------------------------------------------------
    int caption = CAPTION_BLANK;
    if (state == Model::IDLE)
    {
        caption = CAPTION_TAP_TO_START; // shown on both halves, either one starts
    }
    else if (state == Model::FINISHED)
    {
        caption = (m.getFlagged() == side) ? CAPTION_FLAG_FELL : CAPTION_WINS;
    }
    else if (onMove)
    {
        caption = (state == Model::PAUSED) ? CAPTION_PAUSED : CAPTION_TO_PLAY;
    }
    if (force || caption != shownCaption[i])
    {
        shownCaption[i] = caption;
        static const char* const CAPTIONS[] = {
            "", "TO PLAY", "PAUSED", "TAP TO START", "FLAG FELL", "WINS"
        };
        touchgfx::Unicode::strncpy(w.captionBuffer, CAPTIONS[caption], CAPTIONLEFT_SIZE);
        w.caption->invalidate();
    }

    // --- move counter ------------------------------------------------------
    if (force || m.getMoves(side) != shownMoves[i])
    {
        shownMoves[i] = m.getMoves(side);
        touchgfx::Unicode::snprintf(w.movesBuffer, MOVESLEFT_SIZE, "MOVES %u", shownMoves[i]);
        w.moves->invalidate();
    }
}

void GameScreenView::refreshBottomBar(bool force)
{
    const Model& m = presenter->getModel();
    const Model::State state = m.getState();

    if (force || static_cast<int>(state) != shownState)
    {
        shownState = static_cast<int>(state);
        const char* label = "START";
        switch (state)
        {
        case Model::RUNNING:
            label = "PAUSE";
            break;
        case Model::PAUSED:
            label = "RESUME";
            break;
        case Model::FINISHED:
            label = "TIME UP";
            break;
        case Model::IDLE:
        default:
            label = "START";
            break;
        }
        touchgfx::Unicode::strncpy(btnPauseLabelBuffer, label, BTNPAUSELABEL_SIZE);
        btnPauseLabel.setColor((state == Model::FINISHED) ? palette::textDisabled() : palette::textBright());
        btnPauseLabel.invalidate();
    }

    if (force || m.getMinutes() != shownMinutes || m.getIncrement() != shownIncrement)
    {
        shownMinutes = m.getMinutes();
        shownIncrement = m.getIncrement();
        if (m.isUnlimited())
        {
            touchgfx::Unicode::strncpy(tcLabelBuffer, "NO TIME LIMIT", TCLABEL_SIZE);
        }
        else
        {
            touchgfx::Unicode::snprintf(tcLabelBuffer, TCLABEL_SIZE, "%u MIN + %u SEC",
                                        shownMinutes, shownIncrement);
        }
        tcLabel.invalidate();
    }

    for (int id = BUTTON_PAUSE; id <= BUTTON_SETTINGS; id++)
    {
        touchgfx::colortype c = palette::button();
        if (id == BUTTON_PAUSE && state == Model::FINISHED)
        {
            c = palette::buttonDisabled();
        }
        if (id == static_cast<int>(pressedButton))
        {
            c = palette::buttonPressed();
        }
        if (force || c.color != shownButtonColor[id])
        {
            shownButtonColor[id] = c.color;
            touchgfx::Box* b = buttonBox(static_cast<ButtonId>(id));
            b->setColor(c);
            b->invalidate();
        }
    }
}

touchgfx::Box* GameScreenView::buttonBox(ButtonId id)
{
    switch (id)
    {
    case BUTTON_PAUSE:
        return &btnPauseBg;
    case BUTTON_RESET:
        return &btnResetBg;
    case BUTTON_SETTINGS:
    default:
        return &btnSettingsBg;
    }
}

GameScreenView::ButtonId GameScreenView::buttonAt(int16_t x, int16_t y)
{
    if (btnPauseBg.getAbsoluteRect().intersect(x, y))
    {
        return BUTTON_PAUSE;
    }
    if (btnResetBg.getAbsoluteRect().intersect(x, y))
    {
        return BUTTON_RESET;
    }
    if (btnSettingsBg.getAbsoluteRect().intersect(x, y))
    {
        return BUTTON_SETTINGS;
    }
    return BUTTON_NONE;
}

void GameScreenView::activateButton(ButtonId id)
{
    switch (id)
    {
    case BUTTON_PAUSE:
        if (presenter->getModel().getState() != Model::FINISHED)
        {
            presenter->togglePause();
        }
        break;
    case BUTTON_RESET:
        presenter->newGame();
        break;
    case BUTTON_SETTINGS:
        presenter->gotoSettings();
        return; // this screen is about to be torn down
    default:
        break;
    }
    refresh(false);
}

void GameScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    const int16_t x = evt.getX();
    const int16_t y = evt.getY();

    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        const ButtonId hit = buttonAt(x, y);
        if (hit != BUTTON_NONE)
        {
            pressedButton = hit;
            refreshBottomBar(false);
            return;
        }

        // A half of the screen reacts on press, like the button on a real clock.
        if (panelLeft.getAbsoluteRect().intersect(x, y))
        {
            presenter->sidePressed(Model::WHITE);
            refresh(false);
        }
        else if (panelRight.getAbsoluteRect().intersect(x, y))
        {
            presenter->sidePressed(Model::BLACK);
            refresh(false);
        }
    }
    else // RELEASED or CANCEL
    {
        const ButtonId released = pressedButton;
        pressedButton = BUTTON_NONE;
        refreshBottomBar(false);

        // Only fire if the finger came up over the button it went down on.
        if (released != BUTTON_NONE && evt.getType() == touchgfx::ClickEvent::RELEASED &&
            buttonAt(x, y) == released)
        {
            activateButton(released);
        }
    }
}
