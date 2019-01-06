#pragma

#include "WordTask.h"
#include "TaskManager.h"

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class WordGame : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual void update(float dt);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
	void menuReplayBack(Ref* pSender);

	Label *CreateTextLabel(char *text, char *ff, int fsize, Vec2 pos);
	Label *CreateScoreLabel(char *text, char *ff, int fsize, Vec2 pos);
	Label *CreateHPLabel(char *text, char *ff, int fsize, Vec2 pos);
	ui::LoadingBar *CreateHPLoadingBar(char *pictureFile, Vec2 pos);

	void addScore(int s);
	void setScore(int s);
	const int getScore() const;
	void setHp(int hp_);
	const int getHp() const;
	void setBestScore(int bs);
	const int getBestScore() const;

	void takeAwayHP();
	void showAnswer();

	void backLabelWordUpp(Sprite *obj, Vec2 scaleBy);
	void MoveObject(Sprite *obj, Vec2 moveTo, Vec2 scaleBy);
 
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event);

	void createEndScene();
	void createWinningScene();
	void showEnd(float dt);
	void showWin(float dt);

	void readBestScore();
	void writeBestScore();

    // implement the "static create()" method manually
    CREATE_FUNC(WordGame);

private:
	Sprite *collision;
	Scene *startScene;
	Scene *endScene;
	Scene *winScene;
	TaskManager taskManager;
	WordTask taskWord;
	Label *taskLabel;
	Label *scoreLabel;
	Label *hpLabel;
	Label *bestScoreLabel;
	int hp;
	int bestScore;
	int countTasks;
	ui::LoadingBar *hpLoadingBar;
	int score;
	bool m_decreasedHP;
	bool m_increasedScore;

private:
	void updateTask(float);
	void initPosMove();
};
