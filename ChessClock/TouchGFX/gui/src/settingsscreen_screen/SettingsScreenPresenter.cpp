#include <gui/settingsscreen_screen/SettingsScreenView.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

SettingsScreenPresenter::SettingsScreenPresenter(SettingsScreenView& v)
    : view(v)
{
}

void SettingsScreenPresenter::activate()
{
}

void SettingsScreenPresenter::deactivate()
{
}

void SettingsScreenPresenter::applyTimeControl(uint16_t minutes, uint16_t increment)
{
    model->setTimeControl(minutes, increment);
    model->newGame();
    gotoGame();
}

void SettingsScreenPresenter::gotoGame()
{
    static_cast<FrontendApplication*>(touchgfx::Application::getInstance())->gotoGameScreenScreenNoTransition();
}
