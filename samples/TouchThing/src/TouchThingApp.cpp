#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

#include "BloomScene.h"
#include "SpaceThing.h"

using namespace ci;
using namespace ci::app;

class TouchThingApp : public AppCocoaTouch {
public:
    
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
    
    bool onPlanetTouchEnded( BloomSceneEventRef eventRef );
    
    BloomSceneRef mBloomSceneRef;
};

void TouchThingApp::setup()
{
    // initialize controller for all 2D UI
    // this will receive touch events before anything else (so it can cancel them before they hit the world)
    // it's a shared pointer, hence the -Ref suffix and ::create method
    mBloomSceneRef = BloomScene::create( this );
    
    // make a parent "planet":
    SpaceThingRef planet = SpaceThing::create(20.0f, 200.0f, 0.0f, 0.004f);    
    planet->registerTouchEnded( this, &TouchThingApp::onPlanetTouchEnded );
    
    // add to scene
    mBloomSceneRef->addChild( planet );        
}

bool TouchThingApp::onPlanetTouchEnded( BloomSceneEventRef eventRef )
{
    // thing that was touched:
    BloomNodeRef planet = eventRef->getTargetRef();    
    int numChildren = planet->getNumChildren();
    if (numChildren == 0) {
        // add some moons I guess :)
        planet->addChild( SpaceThing::create( 5.0f, 75.0f, 0.4f, 0.006f ) );
        planet->addChild( SpaceThing::create( 10.0f, 50.0f, 2.0f, 0.002f ) );
        planet->addChild( SpaceThing::create( 8.0f, 60.0f, 4.3f, 0.01f ) );
    }
    else {
        // clear children
        planet->removeChildren(); // children are shared pointers so should be cleared up automatically
    }
    return true; // don't bubble
}

void TouchThingApp::update()
{
    mBloomSceneRef->deepUpdate();
    mBloomSceneRef->setTransform( Matrix44f::createTranslation( Vec3f( getWindowCenter(), 0.0f ) ) );
}

void TouchThingApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    gl::setMatricesWindow( getWindowSize() );
    
    // scene sets 2D camera to window size and recursively draws children (depth first)
    mBloomSceneRef->deepDraw();
}

CINDER_APP_COCOA_TOUCH( TouchThingApp, RendererGl )
