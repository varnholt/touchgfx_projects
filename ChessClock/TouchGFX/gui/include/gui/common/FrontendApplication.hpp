#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>
#include <touchgfx/Callback.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        model.tick();
        FrontendApplicationBase::handleTickEvent();
    }

    /**
     * The Designer only emits goto-methods for screens that one of its interactions
     * navigates to. This application changes screen from C++ instead, so the transition
     * to the settings screen is declared here, following the same deferred-callback
     * pattern as the generated code.
     */
    void gotoSettingsScreenScreenNoTransition();

private:
    void gotoSettingsScreenScreenNoTransitionImpl();

    touchgfx::Callback<FrontendApplication> settingsTransitionCallback;
};

#endif // FRONTENDAPPLICATION_HPP
