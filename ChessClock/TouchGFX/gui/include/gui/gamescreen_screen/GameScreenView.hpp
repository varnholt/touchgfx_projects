#ifndef GAMESCREENVIEW_HPP
#define GAMESCREENVIEW_HPP

#include <gui_generated/gamescreen_screen/GameScreenViewBase.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>
#include <gui/model/Model.hpp>

/**
 * The clock face. Each half of the screen is one player's button, exactly as on a
 * physical clock: tapping your own half ends your move. The strip along the bottom
 * carries start/pause, reset and the way to the settings screen.
 *
 * Each half is headed by a name plate in the piece colour it stands for - dark on
 * white, or white on black - so which half is which needs no reading at all. Whose
 * turn it is then shows as the half filling with colour, a bar lighting up along
 * its outer edge, and the waiting player's whole plate fading back. The status
 * line inside the plate spells it out for anyone who wants the words.
 *
 * The two halves are driven through a single code path; sideWidgets() maps a
 * Model::Side onto the widgets the Designer generated for that half.
 */
class GameScreenView : public GameScreenViewBase
{
public:
    GameScreenView();
    virtual ~GameScreenView() {}

    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);

private:
    enum ButtonId
    {
        BUTTON_NONE = -1,
        BUTTON_PAUSE = 0,
        BUTTON_RESET,
        BUTTON_SETTINGS
    };

    /** The status line under the name plate. */
    enum CaptionId
    {
        CAPTION_BLANK = 0,
        CAPTION_TO_PLAY,
        CAPTION_PAUSED,
        CAPTION_TAP_TO_START,
        CAPTION_FLAG_FELL,
        CAPTION_WINS
    };

    /** How far the plate of the player who is waiting fades back. */
    static const uint8_t PLATE_ALPHA_WAITING = 110;

    /** The widgets belonging to one player's half of the screen. */
    struct SideWidgets
    {
        touchgfx::Box*                     panel;
        touchgfx::Box*                     turnBar;
        touchgfx::Box*                     plate;
        touchgfx::TextArea*                name;
        touchgfx::TextAreaWithOneWildcard* caption;
        touchgfx::TextAreaWithOneWildcard* time;
        touchgfx::TextAreaWithOneWildcard* moves;
        touchgfx::Unicode::UnicodeChar*    captionBuffer;
        touchgfx::Unicode::UnicodeChar*    timeBuffer;
        touchgfx::Unicode::UnicodeChar*    movesBuffer;
    };

    SideWidgets sideWidgets(Model::Side side);

    void refresh(bool force);
    void refreshSide(Model::Side side, bool force);
    void refreshBottomBar(bool force);

    ButtonId buttonAt(int16_t x, int16_t y);
    void     activateButton(ButtonId id);
    touchgfx::Box* buttonBox(ButtonId id);

    ButtonId pressedButton;

    // What is currently on screen, so a tick only invalidates what really changed.
    uint32_t shownTimeMs[2];
    uint32_t shownPanelColor[2];
    uint32_t shownTimeColor[2];
    uint32_t shownTrimColor[2];
    uint32_t shownBarColor[2];
    uint32_t shownButtonColor[3];
    uint16_t shownMoves[2];
    uint8_t  shownPlateAlpha[2];
    int      shownCaption[2];
    int      shownState;
    uint16_t shownMinutes;
    uint16_t shownIncrement;
};

#endif // GAMESCREENVIEW_HPP
