#include "HelloWorldScene.h"
//#include "SimpleAudioEngine.h"
#include "GameOverScene.h"

USING_NS_CC;

//
HelloWorld::HelloWorld() :
	_targets(nullptr),
	_projectiles(nullptr),
	_projectilesDestroyed(0)
{
}

//
HelloWorld::~HelloWorld()
{
	if (_targets)
	{
		_targets->clear();	//
		delete _targets;	//
		_targets = nullptr;
	}

	if (_projectiles)
	{
		_projectiles->clear();	//
		delete _projectiles;	//
		_projectiles = nullptr;
	}
	// cpp don't need to call super dealloc
	// virtual destructor will do this
}

//static function
CCScene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();				//

	// 'layer' is an autorelease object
	HelloWorld *layer =  HelloWorld::create();		//

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if (!LayerColor::initWithColor(ccc4(255, 255, 255, 255)))	{
		return false;
	}

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	MenuItemImage *pCloseItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
	pCloseItem->setPosition(ccp(Director::sharedDirector()->getWinSize().width - 20, 20));

	// create menu, it's an autorelease object
	Menu* pMenu = Menu::create(pCloseItem, nullptr );
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	Size size = Director::sharedDirector()->getWinSize();

	Sprite* player = Sprite::create("Player.png", CCRectMake(0, 0, 27, 40));
	player->retain();																			//
	// position the sprite on the center of the screen
	player->setPosition(ccp(player->getContentSize().width / 2, size.height / 2));

	// add the sprite as a child to this layer
	this->addChild(player, 0);

	this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0);

	this->setTouchEnabled(true);					//

	_targets = new Vector<Sprite *>;
	_projectiles = new Vector<Sprite *>;

	this->schedule(schedule_selector(HelloWorld::update));
	CCLOG("Loading music");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music.wav");
	//
	// touch
	//
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	_touchListener = touchListener;

	return true;
}

void HelloWorld::gameLogic(float dt)
{
	this->addTarget();
}

void HelloWorld::addTarget()
{
	Sprite *target = Sprite::create("Target.png", CCRectMake(0, 0, 27, 40));

	// Determine where to spawn the target along the Y axis
	Size winSize = Director::sharedDirector()->getWinSize();
	int minY = target->getContentSize().height / 2;
	int maxY = winSize.height - target->getContentSize().height / 2;
	int rangeY = maxY - minY;
	int actualY = (rand() % rangeY) + minY;

	// Create the target slightly off-screen along the right edge,
	// and along a random position along the Y axis as calculated above
	target->setPosition(ccp(winSize.width + target->getContentSize().width / 2, actualY));
	this->addChild(target);

	// Determine speed of the target
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int actualDuration = (rand() % rangeDuration) + minDuration;

	// Create the actions
	FiniteTimeAction *actionMove = MoveTo::create(actualDuration, ccp(-target->getContentSize().width / 2, actualY));
	FiniteTimeAction *actionMoveDone = CallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished));

	target->setTag(1);

	_targets->pushBack(target);																//

	target->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void HelloWorld::spriteMoveFinished(Node* sender)
{
	Sprite *sprite = (Sprite *)sender;
	this->removeChild(sprite, true);
	if (sprite->getTag() == 1)
	{ // target
		_targets->eraseObject(sprite);															//
		GameOverScene *gameOverScene = GameOverScene::node();
		gameOverScene->getLayer()->getLabel()->setString("You Lose! :[");
		CCDirector::sharedDirector()->replaceScene(gameOverScene);

	}
	else if (sprite->getTag() == 2)
	{ // projectile
		_projectiles->eraseObject(sprite);														//
	}
}


void HelloWorld::menuCloseCallback(Object* pSender)
{
	Director::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

/*
Touch Began

*/
bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

/*
Touch Moved

@param	Touch *touch
@param	Event *unused_event
*/
void HelloWorld::onTouchMoved(Touch *touch, Event *unused_event)
{

}

/*


*/
void HelloWorld::onTouchCancelled(Touch *touch, Event *unused_event)
{

}

/*


@param	touch			:
@param	unused_event	:
*/
void HelloWorld::onTouchEnded(cocos2d::Touch *touch, Event *unused_event)
{
	Point location = touch->getLocation();							//

	// Set up initial location of projectile
	Size winSize = Director::sharedDirector()->getWinSize();
	Sprite *projectile = Sprite::create("Projectile.png", CCRectMake(0, 0, 20, 20));
	projectile->setPosition(ccp(20, winSize.height / 2));

	// Determine offset of location to projectile
	int offX = location.x - projectile->getPosition().x;
	int offY = location.y - projectile->getPosition().y;

	// Bail out if we are shooting down or backwards
	if (offX <= 0) return;

	// Ok to add now - we've double checked position
	this->addChild(projectile);

	// Determine where we wish to shoot the projectile to
	int realX = winSize.width + (projectile->getContentSize().width / 2);
	float ratio = (float)offY / (float)offX;
	int realY = (realX * ratio) + projectile->getPosition().y;
	Point realDest = ccp(realX, realY);

	// Determine the length of how far we're shooting
	int offRealX = realX - projectile->getPosition().x;
	int offRealY = realY - projectile->getPosition().y;
	float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
	float velocity = 480 / 1; // 480pixels/1sec
	float realMoveDuration = length / velocity;

	projectile->setTag(2);

	_projectiles->pushBack(projectile);											//

	// Move projectile to actual endpoint
	projectile->runAction(Sequence::create(
		MoveTo::create(realMoveDuration, realDest),
		CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)),
		NULL));
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("pew-pew-lei.wav");	//
}

void HelloWorld::update(float dt)
{
	Vector<cocos2d::Sprite*> *projectilesToDelete = new Vector < Sprite * > ;

	Vector<Sprite*>::iterator it, jt;

	for (it = _projectiles->begin(); it != _projectiles->end(); it++)
	{
		Sprite *projectile = *it;
		Rect projectileRect = CCRectMake(
			projectile->getPosition().x - (projectile->getContentSize().width / 2),
			projectile->getPosition().y - (projectile->getContentSize().height / 2),
			projectile->getContentSize().width,
			projectile->getContentSize().height);
		Vector<cocos2d::Sprite*> *targetsToDelete = new Vector < cocos2d::Sprite* > ;
		for (jt = _targets->begin(); jt != _targets->end(); jt++)
		{
			Sprite *target = *jt;
			Rect targetRect = CCRectMake(
				target->getPosition().x - (target->getContentSize().width / 2),
				target->getPosition().y - (target->getContentSize().height / 2),
				target->getContentSize().width,
				target->getContentSize().height);

			if (projectileRect.intersectsRect(targetRect))							//
			{
				targetsToDelete->pushBack(target);										//
			}
		}

		for (jt = targetsToDelete->begin(); jt != targetsToDelete->end(); jt++)
		{
			_targets->eraseObject(*jt);												//
			this->removeChild((*jt), true);

			_projectilesDestroyed++;
			if (_projectilesDestroyed > 30) {
				GameOverScene *gameOverScene = GameOverScene::node();
				_projectilesDestroyed = 0;
				gameOverScene->getLayer()->getLabel()->setString("You Win!");
				CCDirector::sharedDirector()->replaceScene(gameOverScene);
			}
		}

		if (targetsToDelete->size())						//
		{
			projectilesToDelete->pushBack(*it);			//
		}

		targetsToDelete->clear();			//
		delete targetsToDelete;				//
		targetsToDelete = nullptr;			//
	}

	//
	for (it = projectilesToDelete->begin(); it != projectilesToDelete->end(); it++)
	{
		_projectiles->eraseObject(*it);	//
		this->removeChild(*it, true);
	}

	projectilesToDelete->clear();			//
	delete projectilesToDelete;				//
}
