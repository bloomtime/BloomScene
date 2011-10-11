#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

#include "BloomScene.h"
#include "SpaceThing.h"

using namespace ci;
using namespace ci::app;

class SpaceThingApp : public AppCocoaTouch {
public:
    
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
    
    virtual void    touchesBegan( TouchEvent event );
    virtual void    touchesMoved( TouchEvent event );
    virtual void    touchesEnded( TouchEvent event );
    
    BloomSceneRef mBloomSceneRef;
};

void SpaceThingApp::setup()
{
    // initialize controller for all 2D UI
    // this will receive touch events before anything else (so it can cancel them before they hit the world)
    // it's a shared pointer, hence the -Ref suffix and ::create method
    mBloomSceneRef = BloomScene::create( this );
    
    // make a parent "planet":
    SpaceThingRef planet = SpaceThing::create(20.0f, 200.0f, 0.0f, 0.04f);    

    // some moons I guess :)
    planet->addChild( SpaceThing::create( 5.0f, 75.0f, 0.4f, 0.06f ) );
    planet->addChild( SpaceThing::create( 10.0f, 50.0f, 2.0f, 0.02f ) );
    planet->addChild( SpaceThing::create( 8.0f, 60.0f, 4.3f, 0.1f ) );

    // and then add to the scene
    mBloomSceneRef->addChild( planet );
}

void SpaceThingApp::touchesBegan( TouchEvent event )
{
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    
}

void SpaceThingApp::touchesMoved( TouchEvent event )
{
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    
}

void SpaceThingApp::touchesEnded( TouchEvent event )
{
    std::vector<TouchEvent::Touch> touches = event.getTouches();
    
}

void SpaceThingApp::update()
{
    mBloomSceneRef->deepUpdate();
    mBloomSceneRef->setTransform( Matrix44f::createTranslation( Vec3f( getWindowCenter(), 0.0f ) ) );
}

void SpaceThingApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    gl::setMatricesWindow( getWindowSize() );
    
    // scene sets 2D camera to window size and recursively draws children (depth first)
    mBloomSceneRef->deepDraw();
}

CINDER_APP_COCOA_TOUCH( SpaceThingApp, RendererGl )
