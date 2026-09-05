#ifndef SETTINGSSCREENPRESENTER_HPP
#define SETTINGSSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SettingsScreenView;

class SettingsScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SettingsScreenPresenter(SettingsScreenView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~SettingsScreenPresenter() {}

    uint16_t getMinutes() const   { return model->getMinutes(); }
    uint16_t getIncrement() const { return model->getIncrement(); }

    /** Stores the new time control, rearms both clocks and returns to the game. */
    void applyTimeControl(uint16_t minutes, uint16_t increment);

    /** Returns to the game screen leaving the clock as it was. */
    void gotoGame();

private:
    SettingsScreenPresenter();

    SettingsScreenView& view;
};

#endif // SETTINGSSCREENPRESENTER_HPP
