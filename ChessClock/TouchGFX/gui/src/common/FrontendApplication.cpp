#include <gui/common/FrontendApplication.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <gui/settingsscreen_screen/SettingsScreenView.hpp>
#include <gui/settingsscreen_screen/SettingsScreenPresenter.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap),
      settingsTransitionCallback()
{
}

void FrontendApplication::gotoSettingsScreenScreenNoTransition()
{
    settingsTransitionCallback = touchgfx::Callback<FrontendApplication>(
        this, &FrontendApplication::gotoSettingsScreenScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &settingsTransitionCallback;
}

void FrontendApplication::gotoSettingsScreenScreenNoTransitionImpl()
{
    touchgfx::makeTransition<SettingsScreenView, SettingsScreenPresenter, touchgfx::NoTransition, Model>(
        &currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
