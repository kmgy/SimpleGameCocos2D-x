#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class HelloWorld : public cocos2d::LayerColor
{
public:
	HelloWorld();				//
	virtual ~HelloWorld();	//

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* createScene();
	
	void addTarget();
    
    void spriteMoveFinished(Node* sender);

	void gameLogic( float dt);
	void update(float dt);
	
	// a selector callback
	virtual void menuCloseCallback(Object* pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(HelloWorld);

private:
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchCancelled(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	cocos2d::Vector<cocos2d::Sprite*> *_targets;
	cocos2d::Vector<cocos2d::Sprite*> *_projectiles;
	
	int _projectilesDestroyed;
};

#endif // __HELLOWORLD_SCENE_H__
