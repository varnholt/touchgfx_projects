#include <gui/smoke_screen/smokeview.hpp>
#include <BitmapDatabase.hpp>


void SmokeView::setupScreen()
{
   SmokeViewBase::setupScreen();

   // configure effect
   _effect.setWidth(HAL::DISPLAY_WIDTH);
   _effect.setHeight(HAL::DISPLAY_HEIGHT);
   _effect.setXY(0, 0);
   _effect.setGradientTexture(BITMAP_GRADIENT_ID);
   _effect.setOverlayTexture(BITMAP_ST_LOGO_ID);

   Bitmap::cache(BITMAP_GRADIENT_ID);
   Bitmap::cache(BITMAP_ST_LOGO_ID);

   // remove the default background added by touchgfx
   remove(__background);

   // insert the effect at the very bottom of the stack
   container.insert(nullptr, _effect);

   // set up FPS updates
   _fps._callback = [this](uint8_t fps){updateFPS(fps);};

   // enable frame rate compensation to enable FPS counter
   HAL::getInstance()->setFrameRateCompensation(true);
}


void SmokeView::setQuality(Quality value)
{
    SmokeViewBase::setQuality(value);
    
	switch (value) {
	case Quality::LOW:
		_effect.setLowQuality();
		break;
	case Quality::MEDIUM:
		_effect.setMediumQuality();
		break;
	case Quality::HIGH:
		_effect.setHighQuality();
		break;
	}
}


void SmokeView::handleTickEvent()
{
	_fps.update();
}

void SmokeView::tearDownScreen()
{
	Bitmap::clearCache();
}


void SmokeView::updateFPS(uint8_t fps)
{
    Unicode::snprintf(_fpsNumberBuffer, 3, "%d", fps);
    _fpsNumber.invalidate();
}

