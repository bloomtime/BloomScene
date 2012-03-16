//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "BloomScene.h"

using namespace ci;
using namespace ci::app; // for TouchEvent and TouchEvent::Touch

BloomSceneRef BloomScene::create( Vec2f interfaceSize, Vec2f windowSize )
{
    BloomSceneRef ref = BloomSceneRef( new BloomScene( interfaceSize, windowSize ) );
    ref->mRoot = BloomSceneWeakRef( ref );
    return ref;
}

BloomScene::BloomScene( Vec2f interfaceSize, Vec2f windowSize ): 
    mWindowSize( windowSize ),
    mInterfaceSize( interfaceSize )
{
    mParent = BloomNodeRef(); // NULL, we are the parent (crash rather than recurse)
    mRoot = BloomSceneRef();  // NULL, will be set in create() because we are the root    
}

BloomScene::~BloomScene()
{
}

bool BloomScene::touchesBegan( TouchEvent event )
{
    bool consumed = true;
    for(const TouchEvent::Touch &touch : event.getTouches()) {
        consumed = deepTouchBegan( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesMoved( TouchEvent event )
{
    bool consumed = true;
    for(const TouchEvent::Touch &touch : event.getTouches()) {
        consumed = deepTouchMoved( touch ) && consumed; // recurses to children
    }
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesEnded( TouchEvent event )
{
    bool consumed = true;
    for(const TouchEvent::Touch &touch : event.getTouches()) {
        consumed = deepTouchEnded( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}
