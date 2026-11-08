#ifndef SHAPESPRESENTER_HPP
#define SHAPESPRESENTER_HPP

#include <gui/model/modellistener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ShapesView;

class ShapesPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ShapesPresenter(ShapesView& v);

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

    virtual ~ShapesPresenter() {};

private:
    ShapesPresenter();

    ShapesView& view;
};

#endif // SHAPESPRESENTER_HPP
