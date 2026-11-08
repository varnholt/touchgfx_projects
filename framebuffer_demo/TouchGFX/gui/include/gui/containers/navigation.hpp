#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include <gui_generated/containers/NavigationBase.hpp>

class Navigation : public NavigationBase
{
public:
    Navigation();
    virtual ~Navigation() {}

    virtual void initialize();
protected:
};

#endif // NAVIGATION_HPP
