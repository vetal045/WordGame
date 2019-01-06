#include "WordTask.h"

using namespace std;
using namespace cocos2d;

#define KEYCODE_OFFSET 27

WordTask::WordTask()
{
	key = ' ';
}

WordTask::~WordTask()
{
}

int WordTask::init(std::string task, std::string answer, char key)
{
	this->task = task;
	this->answer = answer;
	this->key = key;

	return 0;
}

bool WordTask::isKeyCorrect(EventKeyboard::KeyCode keyCode)
{
	if ((EventKeyboard::KeyCode)(this->key + KEYCODE_OFFSET) == keyCode)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WordTask::setKey(char key)
{
	this->key = key;
}

void WordTask::setAnswer(std::string answer)
{
	this->answer = answer;
}

void WordTask::setTask(std::string task)
{
	this->task = task;
}