#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

GameScreenPresenter::GameScreenPresenter(GameScreenView& v)
    : view(v)
{
}

void GameScreenPresenter::activate()
{
}

void GameScreenPresenter::deactivate()
{
}

void GameScreenPresenter::gotoSettings()
{
    // The clock keeps running while another screen is up, so stop it first -
    // nobody should lose time while looking at the settings.
    if (model->getState() == Model::RUNNING)
    {
        model->togglePause();
    }
    static_cast<FrontendApplication*>(touchgfx::Application::getInstance())->gotoSettingsScreenScreenNoTransition();
}
