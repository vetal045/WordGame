#include "WordGame.h"
#include "SimpleAudioEngine.h"

#include <iostream> 
#include <sstream> 

USING_NS_CC;

#define LABEL_FONTNAME "fonts/Marker Felt.ttf"
#define LABEL_FONTSIZE 24
#define SCORE_CHANGE 10
#define HP_MAX 100
#define HP_NUMBER_HEART 5
#define MAX_TASKS 15
#define MAX_SCORE 150

Scene* WordGame::createScene()
{
    auto scene = Scene::create();
    auto layer = WordGame::create();
    scene->addChild(layer);
    return scene;
}

bool WordGame::init()
{	
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(WordGame::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

	//Vec2 task_pos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 200);
	taskLabel = CreateTextLabel("HI!", LABEL_FONTNAME, LABEL_FONTSIZE, Vec2(0,0));
	//taskLabel->setColor(Color3B::BLACK);

	int best_score_lab_pos_X = origin.x + visibleSize.width - 400;
	int best_lab_pos_Y = origin.y + visibleSize.height - 80;

	Vec2 best_score_pos = Vec2(best_score_lab_pos_X, best_lab_pos_Y);;
	bestScoreLabel = CreateTextLabel("Best score: 0", LABEL_FONTNAME, LABEL_FONTSIZE, best_score_pos);

	int score_lab_pos_X = origin.x + visibleSize.width - 400;	
	int score_lab_pos_Y = origin.y + visibleSize.height - 40;	

	Vec2 score_pos = Vec2(score_lab_pos_X, score_lab_pos_Y);
	scoreLabel = CreateTextLabel("Score: 0/150", LABEL_FONTNAME, LABEL_FONTSIZE, score_pos);

	int hp_lab_pos_X = origin.x + visibleSize.width - 220;	
	int hp_lab_pos_Y = origin.y + visibleSize.height - 40;	

	Vec2 hp_pos = Vec2(hp_lab_pos_X, hp_lab_pos_Y);
	hpLabel = CreateTextLabel("HP: 5/5", LABEL_FONTNAME, LABEL_FONTSIZE, hp_pos);
	
	Vec2 loadingBarPos = Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 40);
	hpLoadingBar = CreateHPLoadingBar("bp.png", loadingBarPos);

	//Vec2 taskMoveTo = origin + Vec2(visibleSize.width / 2, -visibleSize.height / 4);
	//MoveObject((Sprite *)taskLabel, taskMoveTo, Vec2(3, 3));

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(WordGame::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	

	collision = Sprite::create("bp.png");
	collision->setScale(0.1);
	collision->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 700));
	collision->setVisible(false);
	this->addChild(collision, 4);

	auto backgroundImage = Sprite::create("background.jpg");
	backgroundImage->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(backgroundImage);
	
	readBestScore();

	taskManager.init(MAX_TASKS);
	setScore(0);
	setHp(HP_NUMBER_HEART);
	countTasks = 0;

	updateTask(MAX_TASKS);

	scheduleUpdate();

	//CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music.mp3");
	//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("music.mp3", true);
	CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.2f);

    return true;
}

void WordGame::update(float dt)
{
	Rect rect1 = taskLabel->getBoundingBox();
	Rect rect2 = collision->getBoundingBox();

	if (rect1.intersectsRect(rect2) && getHp() == 0)
	{
		scheduleOnce(schedule_selector(WordGame::showEnd), 1);

		writeBestScore();
	}

	if (rect1.intersectsRect(rect2) && m_decreasedHP == false)
	{
		takeAwayHP();

		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("false.mp3");
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("false.mp3");
		
		scheduleOnce(schedule_selector(WordGame::updateTask), 1);
	}
}


void WordGame::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void WordGame::menuReplayBack(Ref * pSender)
{
	Director::getInstance()->popScene();
	//readBestScore();
	setScore(0);
	setHp(HP_NUMBER_HEART);
	updateTask(15);
	countTasks = 0;
}

Label * WordGame::CreateTextLabel(char * text, char * ff, int fsize, Vec2 pos)
{
	Label *label = Label::createWithTTF(text, ff, fsize);
	label->setPosition(pos);
	label->setScale(1);
	this->addChild(label, 1, "text_label");

	return label;
}

Label * WordGame::CreateScoreLabel(char * text, char * ff, int fsize, Vec2 pos)
{
	Label *label = Label::createWithTTF(text, ff, fsize);
	label->setPosition(pos);
	this->addChild(label, 2, "score_label");

	return label;
}

Label * WordGame::CreateHPLabel(char * text, char * ff, int fsize, Vec2 pos)
{
	Label *label = Label::createWithTTF(text, ff, fsize);
	label->setPosition(pos);
	this->addChild(label, 2, "hp_label");

	return label;
}

ui::LoadingBar * WordGame::CreateHPLoadingBar(char * pictureFile, Vec2 pos)
{
	ui::LoadingBar *loadingBar = ui::LoadingBar::create(pictureFile);
	loadingBar->setPosition(pos);
	loadingBar->setScale(0.2, 0.2);
	loadingBar->setPercent(100);
	this->addChild(loadingBar, 3, "loadingBar");

	return loadingBar;
}

void WordGame::addScore(int s)
{
	/*
	std::string currentTextLabelScore = scoreLabel->getString();

	//Finds substring after symbol ':'
	std::string strCurrentScore = currentTextLabelScore.substr(currentTextLabelScore.find(":") + 1);

	//Then converts substring to integer value and adds 10
	int currentScore = std::stoi(strCurrentScore) + 10;
	*/
	m_increasedScore = true;

	setScore(this->score + s);
}

void WordGame::setScore(int s)
{
	this->score = s;

	scoreLabel->setString("Score: " + std::to_string(this->score) + "/150");
}

const int WordGame::getScore() const
{
	/*
	std::string currentTextLabelScore = scoreLabel->getString();

	//Finds substring after symbol ':'
	std::string strCurrentScore = currentTextLabelScore.substr(currentTextLabelScore.find(":") + 1);

	//Finds bustring before symbol '/'
	std::string::size_type pos = strCurrentScore.find('/');

	//Then converts substring to integer value 
	int currentScore = std::stoi(strCurrentScore.substr(0, pos));
	*/

	return score;
}

void WordGame::setHp(int hp_)
{
	if (hp_ >= 0 && hp_ <= HP_NUMBER_HEART)
	{
		this->hp = hp_;
		hpLoadingBar->setPercent(hp * 20);
		hpLabel->setString("HP: " + std::to_string(hp) + "/5");
	}
}

const int WordGame::getHp() const
{
	return hp;
}

void WordGame::setBestScore(int bs)
{
	bestScore = bs;

	bestScoreLabel->setString("Best score: " + std::to_string(bestScore));
}

const int WordGame::getBestScore() const
{
	return bestScore;
}

void WordGame::takeAwayHP()
{
	m_decreasedHP = true;

	setHp( (hpLoadingBar->getPercent() / 20) - 1);
}

void WordGame::showAnswer()
{
	taskLabel->setString(taskWord.getAnswer());
	taskLabel->stopAllActions();
}

void WordGame::backLabelWordUpp(Sprite * obj, Vec2 scaleBy)
{
	taskLabel->stopAllActions();

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vec2 task_pos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 200);
	Vec2 taskMoveTo = origin + Vec2(visibleSize.width / 2, -visibleSize.height / 4);

	auto mb = MoveTo::create(0.1, task_pos);
	auto scale = ScaleBy::create(0.1, scaleBy.x, scaleBy.y);

	obj->runAction(mb);
	obj->runAction(scale);

	MoveObject((Sprite*)taskLabel, taskMoveTo, Vec2(1, 1));

	return;
}

void WordGame::MoveObject(Sprite * obj, Vec2 moveTo, Vec2 scaleBy)
{
	auto acMoveTo = MoveTo::create(4, moveTo);  //move sprite in position (moveTo) during 4 seconds
	auto acScaleBy = ScaleBy::create(2, scaleBy.x, scaleBy.y);
	obj->runAction(acMoveTo);
	obj->runAction(acScaleBy);
	return;
}

void WordGame::onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	/*
	if (EventKeyboard::KeyCode::KEY_O == keyCode)
	{
		taskLabel->setOpacity(0);
	}
	else if (EventKeyboard::KeyCode::KEY_A == keyCode)
	{
		backLabelWordUpp((Sprite*)taskLabel, Vec2(3, 3));
	}
	else if (EventKeyboard::KeyCode::KEY_B == keyCode)
	{
		taskLabel->setString("Bye!");
	}
	else if (EventKeyboard::KeyCode::KEY_ESCAPE == keyCode)
	{
		exit(0);
	}
	*/

	if (EventKeyboard::KeyCode::KEY_ESCAPE == keyCode)
	{
		exit(0);
	}
	else if (taskWord.isKeyCorrect(keyCode))
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("true.mp3");
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("true.mp3");

		showAnswer();

		if (m_increasedScore == false)
		{
			addScore(SCORE_CHANGE);
		}

		if (getScore() == MAX_SCORE)
		{
			scheduleOnce(schedule_selector(WordGame::showWin), 1);

			CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("victory2.mp3");
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("victory2.mp3");

			writeBestScore();
		}
		else
		{
			scheduleOnce(schedule_selector(WordGame::updateTask), 1);
		}
	}
	else
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("false.mp3");
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("false.mp3");

		takeAwayHP();

		if (hpLoadingBar->getPercent() == 0)
		{
			scheduleOnce(schedule_selector(WordGame::showEnd), 1);

			CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("lose.mp3");
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lose.mp3");

			writeBestScore();
		}
	}
}

void WordGame::createEndScene()
{
	endScene = Scene::create();

	Label *lbEnd = Label::createWithTTF("GameOver!!!", LABEL_FONTNAME, 3 * LABEL_FONTSIZE);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vec2 pos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 60);

	lbEnd->setPosition(pos);
	endScene->addChild(lbEnd, 1);

	auto replayBut = MenuItemImage::create("button_replay.png", "button_replay.png",
		CC_CALLBACK_1(WordGame::menuReplayBack, this));
	
	replayBut->setPosition(Vec2(pos.x, pos.y - lbEnd->getContentSize().height - 200));
	replayBut->setScale(0.5, 0.5);

	auto menu = Menu::create(replayBut, NULL);
	menu->setPosition(Vec2::ZERO);
	endScene->addChild(menu, 1);

	readBestScore();
}

void WordGame::createWinningScene()
{
	winScene = Scene::create();

	Label *winner = Label::createWithTTF("You win!", LABEL_FONTNAME, 3 * LABEL_FONTSIZE);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vec2 pos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 60);

	winner->setPosition(pos);
	winScene->addChild(winner, 1);

	auto startBut = MenuItemImage::create("play.jpg", "play.jpg",
		CC_CALLBACK_1(WordGame::menuReplayBack, this));

	startBut->setPosition(Vec2(pos.x, pos.y - winner->getContentSize().height - 200));

	auto start = Menu::create(startBut, NULL);
	start->setPosition(Vec2::ZERO);
	winScene->addChild(start, 1);

	auto bgI = Sprite::create("win_scene.jpg");
	bgI->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	winScene->addChild(bgI);

	readBestScore();
}

void WordGame::showEnd(float dt)
{
	createEndScene();
	Director::getInstance()->pushScene(endScene);
}

void WordGame::showWin(float dt)
{
	createWinningScene();
	Director::getInstance()->pushScene(winScene);
}

void WordGame::updateTask(float dt)
{
	if (countTasks == MAX_TASKS)
	{
		scheduleOnce(schedule_selector(WordGame::showWin), 1);

		writeBestScore();
	}
	else
	{
		taskWord = taskManager.getTask(countTasks);
		taskLabel->setString(taskWord.getTask());
		initPosMove();

		++countTasks;
		m_decreasedHP = false;
		m_increasedScore = false;
	}
}

void WordGame::initPosMove()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vec2 task_pos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 200);

	taskLabel->setPosition(task_pos);
	taskLabel->setScale(1);

	Vec2 taskToMove = origin + Vec2(visibleSize.width / 2, -visibleSize.height / 4);

	MoveObject((Sprite*)taskLabel, taskToMove, Vec2(3, 3));
}

void WordGame::readBestScore()
{
	std::string strBestScore;
	std::ifstream inFile("bestScore.txt");

	if (inFile.fail())
	{
		exit(1);
	}

	inFile >> strBestScore;
	setBestScore(std::stoi(strBestScore));
	inFile.close();
}

void WordGame::writeBestScore()
{
	std::string strBestScore;
	std::ifstream inFile("bestScore.txt");
	
	if (inFile.fail())
	{
		exit(1);
	}

	inFile >> strBestScore;
	inFile.close();

	int bestScore = std::stoi(strBestScore);

	std::ofstream outFile("bestScore.txt", std::ios::app || std::ios::trunc);
	//outFile.clear();

	if (getScore() > bestScore)
	{
		outFile << std::to_string(getScore());
	}

	outFile.close();
}



