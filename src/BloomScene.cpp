//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include <boost/foreach.hpp>
#include "BloomScene.h"
//#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

BloomSceneRef BloomScene::create( ci::Vec2f interfaceSize, ci::Vec2f windowSize )
{
    BloomSceneRef ref = BloomSceneRef( new BloomScene( interfaceSize, windowSize ) );
    ref->mRoot = BloomSceneWeakRef( ref );
    return ref;
}

BloomScene::BloomScene( ci::Vec2f interfaceSize, ci::Vec2f windowSize ): 
    mWindowSize( windowSize ),
    mInterfaceSize( interfaceSize )
{
    mParent = BloomNodeRef(); // NULL, we are the parent (crash rather than recurse)
    mRoot = BloomSceneRef();  // NULL, will be set in create() because we are the root
    
//    cbTouchesBegan = mApp->registerTouchesBegan( this, &BloomScene::touchesBegan );
//    cbTouchesMoved = mApp->registerTouchesMoved( this, &BloomScene::touchesMoved );
//    cbTouchesEnded = mApp->registerTouchesEnded( this, &BloomScene::touchesEnded );
    
}

BloomScene::~BloomScene()
{
//    mApp->unregisterTouchesBegan( cbTouchesBegan );
//    mApp->unregisterTouchesMoved( cbTouchesMoved );
//    mApp->unregisterTouchesEnded( cbTouchesEnded );
}

bool BloomScene::touchesBegan( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchBegan( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesMoved( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchMoved( touch ) && consumed; // recurses to children
    }
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesEnded( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchEnded( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}
