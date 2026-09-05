#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

class ModelListener;

/**
 * The chess clock itself: two countdowns, whose turn it is, and the time control they
 * were armed with. The Model outlives the screens, so the game keeps running while the
 * user is on the settings screen.
 */
class Model
{
public:
    enum Side
    {
        WHITE = 0,
        BLACK = 1
    };

    enum State
    {
        IDLE,      ///< armed, waiting for the first tap
        RUNNING,   ///< one of the two clocks is counting down
        PAUSED,    ///< both clocks stopped, game can be resumed
        FINISHED   ///< a flag has fallen
    };

    /** Zero minutes on the clock means no clock at all - see isUnlimited(). */
    static const uint16_t MIN_MINUTES   = 0;
    static const uint16_t MAX_MINUTES   = 180;
    static const uint16_t MAX_INCREMENT = 60;

    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    /** Called once per frame from FrontendApplication; charges the mover's clock. */
    void tick();

    /** Sets the time control. Takes effect on the next newGame(). */
    void setTimeControl(uint16_t minutes, uint16_t increment);
    uint16_t getMinutes() const   { return baseMinutes; }
    uint16_t getIncrement() const { return incrementSeconds; }

    /**
     * No time limit: the clocks count up from zero instead of down, no flag can
     * fall, and the increment does not apply. This is the default, so a clock
     * that nobody has configured is simply a move timer.
     */
    bool isUnlimited() const { return baseMinutes == 0; }

    /** Rearms both clocks with the current time control and returns to IDLE. */
    void newGame();

    /** The player on `side` has completed a move: bank the increment and hand over. */
    void pressSide(Side side);

    /** START / PAUSE / RESUME, depending on the current state. */
    void togglePause();

    State    getState() const                 { return state; }
    Side     getTurn() const                  { return turn; }
    Side     getFlagged() const               { return flagged; }
    uint16_t getMoves(Side side) const        { return moveCount[side]; }

    /** Time left on a timed clock, or time spent so far when unlimited. */
    uint32_t getClockMs(Side side) const      { return clockMs[side]; }

    static Side opponentOf(Side side)         { return (side == WHITE) ? BLACK : WHITE; }

protected:
    ModelListener* modelListener;

private:
    uint32_t clockMs[2];
    uint16_t moveCount[2];
    uint16_t baseMinutes;
    uint16_t incrementSeconds;
    State    state;
    Side     turn;
    Side     flagged;      ///< only meaningful in FINISHED
    uint32_t lastSampleMs; ///< wall clock reading at the previous tick
};

#endif // MODEL_HPP
