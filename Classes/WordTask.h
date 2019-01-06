#pragma once

#include "cocos2d.h"
#include <fstream>
#include <vector>

using namespace cocos2d;

class WordTask
{
public:
	WordTask();
	~WordTask();

	//int init(char*, char*, char);
	int init(std::string task_, std::string answer, char key);
	bool isKeyCorrect(EventKeyboard::KeyCode keyCode);

	//char getKey()  { return key; };
	//char *getAnswer()  { return answer; };
	//char *getTask()  { return task; };

	inline char getKey() const 
	{ 
		return key;
	}

	inline std::string getAnswer() const 
	{ 
		return answer; 
	}

	inline std::string getTask() const 
	{ 
		return task; 
	}

	//int setKey(char);
	//int setAnswer(char*);
	//int setTask(char*);

	void setKey(char);
	void setAnswer(std::string);
	void setTask(std::string);

private:
	/*char* task;
	char* answer;
	char key;*/

	std::string task;
	std::string answer;
	char key;
};

