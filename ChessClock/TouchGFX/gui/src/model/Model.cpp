#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/ClockTime.hpp>

Model::Model()
    : modelListener(0),
      baseMinutes(0),        // unlimited by default
      incrementSeconds(0),
      state(IDLE),
      turn(WHITE),
      flagged(WHITE),
      lastSampleMs(0)
{
    newGame();
}

void Model::setTimeControl(uint16_t minutes, uint16_t increment)
{
    if (minutes > MAX_MINUTES)
    {
        minutes = MAX_MINUTES;
    }
    if (increment > MAX_INCREMENT)
    {
        increment = MAX_INCREMENT;
    }
    baseMinutes = minutes;
    // An increment only means something when there is a clock to add it to.
    incrementSeconds = (minutes == 0) ? 0 : increment;
}

void Model::newGame()
{
    const uint32_t startMs = isUnlimited() ? 0u
                                           : static_cast<uint32_t>(baseMinutes) * 60u * 1000u;
    for (int i = 0; i < 2; i++)
    {
        clockMs[i] = startMs;
        moveCount[i] = 0;
    }
    state = IDLE;
    turn = WHITE;
    flagged = WHITE;
    lastSampleMs = clocktime::millis();
}

void Model::pressSide(Side side)
{
    switch (state)
    {
    case IDLE:
        // White always moves first, so the opening tap just starts the game no
        // matter which half was touched.
        turn = WHITE;
        state = RUNNING;
        lastSampleMs = clocktime::millis();
        break;

    case RUNNING:
        if (side != turn)
        {
            return; // a player can only stop their own clock
        }
        moveCount[side]++;
        if (!isUnlimited())
        {
            // Fischer increment: credited once the move has been completed.
            clockMs[side] += static_cast<uint32_t>(incrementSeconds) * 1000u;
        }
        turn = opponentOf(side);
        break;

    case PAUSED:
    case FINISHED:
    default:
        break; // taps on the halves are ignored
    }
}

void Model::togglePause()
{
    if (state == RUNNING)
    {
        state = PAUSED;
    }
    else if (state == PAUSED || state == IDLE)
    {
        state = RUNNING;
        lastSampleMs = clocktime::millis();
    }
}

void Model::tick()
{
    const uint32_t now = clocktime::millis();
    // Unsigned arithmetic, so this stays correct across the 32 bit wrap.
    const uint32_t elapsed = now - lastSampleMs;
    lastSampleMs = now;

    if (state != RUNNING)
    {
        return;
    }

    if (isUnlimited())
    {
        clockMs[turn] += elapsed; // counts up, no flag can fall
    }
    else if (elapsed >= clockMs[turn])
    {
        clockMs[turn] = 0;
        flagged = turn;
        state = FINISHED;
    }
    else
    {
        clockMs[turn] -= elapsed;
    }
}
