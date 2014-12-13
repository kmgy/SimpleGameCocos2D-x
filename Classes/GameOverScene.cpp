/*
 *  GameOverScene.cpp
 *  SimpleGame
 *
 *  Created by Clawoo on 8/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameOverScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

bool GameOverLayer::init()
{
	if (!LayerColor::initWithColor(ccc4(255, 255, 255, 255)))				//
	{
		return false;
	}
	Size winSize = Director::sharedDirector()->getWinSize();
	this->_label =  cocos2d::LabelTTF::create(
		"",
		"Arial",
		12,
		Size(0,0),
		TextHAlignment::LEFT,
		TextVAlignment::TOP);													//
	this->_label->retain();
	this->getLabel()->setColor(ccc3(0,0,0));
	this->getLabel()->setPosition(ccp(winSize.width/2, winSize.height/2));
	this->addChild(_label);

	this->runAction(Sequence::create(
		DelayTime::create(3),
		CallFunc::create(this, callfunc_selector(GameOverLayer::gameOverDone)),
		NULL));

	return true;
}

void GameOverLayer::gameOverDone()
{
	Director::sharedDirector()->replaceScene(HelloWorld::createScene());	//
}

GameOverLayer::~GameOverLayer()
{
	_label->release();
}

bool GameOverScene::init()
{
	if( Scene::init() )
	{
		this->_layer = GameOverLayer::node();
		this->_layer->retain();
		this->addChild(_layer);

		return true;
	}
	else
	{
		return false;
	}
}

GameOverScene::~GameOverScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}