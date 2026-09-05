#ifndef GAMESCREENPRESENTER_HPP
#define GAMESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameScreenView;

class GameScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GameScreenPresenter(GameScreenView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~GameScreenPresenter() {}

    /** The view polls the clock once per tick rather than being pushed at. */
    const Model& getModel() const { return *model; }

    void sidePressed(Model::Side side) { model->pressSide(side); }
    void togglePause()                 { model->togglePause(); }
    void newGame()                     { model->newGame(); }
    void gotoSettings();

private:
    GameScreenPresenter();

    GameScreenView& view;
};

#endif // GAMESCREENPRESENTER_HPP
