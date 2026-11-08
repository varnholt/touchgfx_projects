#ifndef TORUSPRESENTER_HPP
#define TORUSPRESENTER_HPP

#include <gui/model/modellistener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class TorusView;

class TorusPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    TorusPresenter(TorusView& v);

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

    virtual ~TorusPresenter() {};

private:
    TorusPresenter();

    TorusView& view;
};

#endif // TORUSPRESENTER_HPP
