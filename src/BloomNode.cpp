//
//  BloomNode.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include <boost/foreach.hpp>
#include "BloomNode.h"
#include "BloomScene.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app; // for TouchEvent::Touch

int BloomNode::sNextNodeId = 10000000; // start high

void BloomNode::addChild( BloomNodeRef child )
{
    mChildren.push_back( child );
    child->mParent = BloomNodeWeakRef( shared_from_this() );
    // if we have a root node, pass it on
    if ( BloomSceneRef root = mRoot.lock() ) {
        child->deepSetRoot( root );
    }
    // otherwise just wait, it will be set eventually
}

void BloomNode::addChildAt( BloomNodeRef child, const int &index )
{
    mChildren.insert( mChildren.begin() + index, child );
    child->mParent = BloomNodeWeakRef( shared_from_this() );
    // if we have a root node, pass it on
    if ( BloomSceneRef root = mRoot.lock() ) {
        child->deepSetRoot( root );
    }
    // otherwise just wait, it will be set eventually
}

void BloomNode::setChildIndex( BloomNodeRef child, const int &index )
{
    std::vector<BloomNodeRef>::iterator i = std::find(mChildren.begin(), mChildren.end(), child);
    if ( i != mChildren.end() ) {
        mChildren.erase( i );
        mChildren.insert( mChildren.begin() + index, child );
    }
    else {
        addChildAt( child, index );
    }
}

void BloomNode::removeChild( BloomNodeRef child )
{
    std::vector<BloomNodeRef>::iterator i = std::find(mChildren.begin(), mChildren.end(), child);
    if ( i != mChildren.end() ) {
        mChildren.erase( i );
        child->mParent = BloomNodeWeakRef();
        child->deepSetRoot( BloomSceneRef() ); // clears root for all children
    }
}

BloomNodeRef BloomNode::removeChildAt( const int &index )
{
    if (index < mChildren.size()) {
        std::vector<BloomNodeRef>::iterator i = mChildren.begin() + index;
        BloomNodeRef child = *i;
        mChildren.erase( i );
        child->mParent = BloomNodeRef();
        child->deepSetRoot( BloomSceneRef() );
        return child;
    }
    return BloomNodeRef();
}

void BloomNode::removeChildren()
{
    int numChildren = getNumChildren();
    for (int i = numChildren - 1; i >= 0; i--) {
        removeChildAt(i);
    }    
}

BloomNodeRef BloomNode::getChildById( const int &childId ) const
{
    for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); ++i) {
        if ( (*i)->getId() == childId ) {
            return *i;
        }
    }
    return BloomNodeRef(); // aka NULL
}

void BloomNode::deepUpdate( float elapsedSeconds )
{
    if (mVisible) {
        // update self
        update( elapsedSeconds );
        // update children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepUpdate( elapsedSeconds );
        }
    }
}

void BloomNode::deepDraw()
{    
    if (mVisible) {
        // draw self    
        draw();
        
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
    }     
}

Matrix44f BloomNode::getConcatenatedTransform() const
{
    if ( BloomNodeRef parent = mParent.lock() ) {
        return parent->getConcatenatedTransform() * mTransform;
    }
    return mTransform;
}

Vec2f BloomNode::localToGlobal( const Vec2f &pos )
{
    Vec3f globalPos = (getConcatenatedTransform() * Vec3f( pos.x, pos.y, 0));
    return (globalPos).xy();
}

Vec2f BloomNode::globalToLocal( const Vec2f &pos )
{
    Matrix44f invMtx = getConcatenatedTransform().inverted();
    return (invMtx * Vec3f(pos.x,pos.y,0)).xy();    
}

bool BloomNode::deepTouchBegan( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }
    bool consumed = false;
    // check children
    // use reverse so that things that will be drawn on top are checked first
    BOOST_REVERSE_FOREACH(BloomNodeRef node, mChildren) {
        if (node->deepTouchBegan(touch)) {
            consumed = true;
            mActiveTouches[touch.getId()] = node;
            break; // first child wins (touch can't affect more than one child node)
        }
    }    
    if (!consumed) {
        // check self
        if (touchBegan(touch)) {
            BloomNodeRef thisRef = shared_from_this();
            mActiveTouches[touch.getId()] = thisRef;
            dispatchTouchBegan( BloomSceneEventRef( new BloomSceneEvent( thisRef, thisRef, touch ) ) );
            consumed = true;
        }
    }
    return consumed;
}

bool BloomNode::deepTouchMoved( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }    
    // in this current implementation, children only receive touchMoved calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        BloomNodeRef node = mActiveTouches[touch.getId()];
        if (node->getId() == this->getId()) {
            // check self
            consumed = touchMoved(touch);
            if (consumed) {
                dispatchTouchMoved( BloomSceneEventRef( new BloomSceneEvent( node, node, touch ) ) );
            }
        }
        else {
            consumed = node->deepTouchMoved( touch );
        }
    }
    return consumed;
}

bool BloomNode::deepTouchEnded( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }    
    // in this current implementation, children only receive touchEnded calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        BloomNodeRef node = mActiveTouches[touch.getId()];
        if (node->getId() == this->getId()) {
            // check self
            consumed = touchEnded(touch);
            if (consumed) {
                dispatchTouchEnded( BloomSceneEventRef( new BloomSceneEvent( node, node, touch ) ) );
            }
        }
        else {
            consumed = node->deepTouchEnded( touch );
        }
        mActiveTouches.erase(touch.getId());
    }
    return consumed;
}

bool BloomNode::deepHitTest( const Vec2f &screenPos )
{
    if (mVisible) {
        // test children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            if ( child->deepHitTest( screenPos ) ) {
                return true;
            }
        }
        // test self
        return hitTest( screenPos );
    }
    return false;
}

void BloomNode::deepSetRoot( BloomSceneRef root )
{
    BOOST_FOREACH(BloomNodeRef child, mChildren) {        
        // propagate to children first
        child->deepSetRoot( root );
    }
    // and then set self
    if (root) {
        mRoot = root;
        addedToScene(); // notify subclasses that mRoot and mParent are now valid     
    } else {
        removedFromScene(); // notify subclasses that mRoot and mParent are about to be invalid     
        mRoot = BloomSceneWeakRef();
    }    
}

void BloomNode::dispatchTouchBegan( BloomSceneEventRef eventRef )
{ 
	bool handled = false;
	for( CallbackMgr<bool (BloomSceneEventRef)>::iterator cbIter = mCbTouchBegan.begin(); ( cbIter != mCbTouchBegan.end() ) && ( ! handled ); ++cbIter ) {
		handled = (cbIter->second)( eventRef );
    }
	if( !handled )	{
        if (BloomNodeRef parent = getParent()) {
            eventRef->setSourceRef( parent );
            parent->dispatchTouchBegan( eventRef );
        }        
    }    
}

void BloomNode::dispatchTouchMoved( BloomSceneEventRef eventRef )
{ 
	bool handled = false;
	for( CallbackMgr<bool (BloomSceneEventRef)>::iterator cbIter = mCbTouchMoved.begin(); ( cbIter != mCbTouchMoved.end() ) && ( ! handled ); ++cbIter ) {
		handled = (cbIter->second)( eventRef );
    }
	if( !handled )	{
        if (BloomNodeRef parent = getParent()) {
            eventRef->setSourceRef( parent );    
            parent->dispatchTouchMoved( eventRef );
        }        
    }    
}

void BloomNode::dispatchTouchEnded( BloomSceneEventRef eventRef )
{ 
	bool handled = false;
	for( CallbackMgr<bool (BloomSceneEventRef)>::iterator cbIter = mCbTouchEnded.begin(); ( cbIter != mCbTouchEnded.end() ) && ( ! handled ); ++cbIter ) {
		handled = (cbIter->second)( eventRef );
    }
	if( !handled )	{
        if (BloomNodeRef parent = getParent()) {
            eventRef->setSourceRef( parent );
            parent->dispatchTouchEnded( eventRef );
        }        
    }
}
