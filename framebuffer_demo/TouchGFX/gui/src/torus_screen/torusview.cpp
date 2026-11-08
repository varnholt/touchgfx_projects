#include <gui/torus_screen/torusview.hpp>
#include <BitmapDatabase.hpp>


void TorusView::setupScreen()
{
   TorusViewBase::setupScreen();

   // configure the effect
   _effect.setWidth(HAL::DISPLAY_WIDTH);
   _effect.setHeight(HAL::DISPLAY_HEIGHT);
   _effect.setXY(0, 0);
   _effect.setEnvMapTexture(BITMAP_ENVMAP_2_ID);
   Bitmap::cache(BITMAP_ENVMAP_2_ID);

   // inser the effect on top of the background image
   container.insert(&backgroundImage, _effect);

   // set up FPS counter
   _fps._callback = [this](uint8_t fps){updateFPS(fps);};
   HAL::getInstance()->setFrameRateCompensation(true);
}


void TorusView::setQuality(Quality value)
{
    TorusViewBase::setQuality(value);
    
	switch (value) 
	{
		case Quality::LOW:
		{
			_effect.setLowQuality();
			break;
		}
		case Quality::MEDIUM:
		{
			_effect.setMediumQuality();
			break;
		}
		case Quality::HIGH:
		{
			_effect.setHighQuality();
			break;
		}
	}
}


void TorusView::handleTickEvent()
{
	_fps.update();
}

void TorusView::tearDownScreen()
{
	Bitmap::clearCache();
}


void TorusView::updateFPS(uint8_t fps)
{
    Unicode::snprintf(_fpsNumberBuffer, 3, "%d", fps);
    _fpsNumber.invalidate();
}

