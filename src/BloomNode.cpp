//
//  BloomNode.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
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
    child->mRoot = mRoot;    
    child->addedToScene(); // notify child that mRoot and mParent are set
}

void BloomNode::addChildAt( BloomNodeRef child, const int &index )
{
    mChildren.insert( mChildren.begin() + index, child );
    child->mParent = BloomNodeWeakRef( shared_from_this() );
    child->mRoot = mRoot;
    child->addedToScene(); // notify child that mRoot and mParent are set
}

void BloomNode::removeChild( BloomNodeRef child )
{
    std::vector<BloomNodeRef>::iterator i = std::find(mChildren.begin(), mChildren.end(), child);
    if ( i != mChildren.end() ) {
        mChildren.erase( i );
        child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid
        child->mParent = BloomNodeWeakRef();
        child->mRoot = BloomSceneWeakRef();
    }
}

BloomNodeRef BloomNode::removeChildAt( const int &index )
{
    BloomNodeRef child = *mChildren.erase( mChildren.begin() + index );
    child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid    
    child->mParent = BloomNodeRef();
    child->mRoot = BloomSceneRef();
    return child;
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

void BloomNode::deepUpdate()
{
    if (mVisible) {
        // update self
        update();
        // update children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepUpdate();
        }
    }
}

void BloomNode::deepDraw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
        // draw self    
        draw();
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
        glPopMatrix();
    }        
}

Matrix44f BloomNode::getConcatenatedTransform() const
{
    if ( BloomNodeRef parent = mParent.lock() ) {
        return parent->getConcatenatedTransform() * mTransform;
    }
    return mTransform;
}

Vec2f BloomNode::localToGlobal(const Vec2f pos)
{
    return (getConcatenatedTransform() * Vec3f(pos.x,pos.y,0)).xy();
}

Vec2f BloomNode::globalToLocal(const Vec2f pos)
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
    BOOST_FOREACH(BloomNodeRef node, mChildren) {
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
            if ( BloomSceneRef root = mRoot.lock() ) {
                root->onBloomNodeTouchBegan(thisRef);
            }
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
                if ( BloomSceneRef root = mRoot.lock() ) {
                    root->onBloomNodeTouchMoved( node );
                }
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
                if ( BloomSceneRef root = mRoot.lock() ) {
                    root->onBloomNodeTouchEnded( node );
                }
            }
        }
        else {
            consumed = node->deepTouchEnded( touch );
        }
        mActiveTouches.erase(touch.getId());
    }
    return consumed;
}