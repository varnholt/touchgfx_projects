#ifndef SMOKEPRESENTER_HPP
#define SMOKEPRESENTER_HPP

#include <gui/model/modellistener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SmokeView;

class SmokePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SmokePresenter(SmokeView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~SmokePresenter() {};

private:
    SmokePresenter();

    SmokeView& view;
};

#endif // SMOKEPRESENTER_HPP
