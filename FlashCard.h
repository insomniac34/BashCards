/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <list>

#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

#include "config.h"

extern bool LEARNING_MODE;
extern int SIGNAL;
extern void send_signal(int new_sig);
extern int recv_signal();

#ifndef _FLASHCARD
#define _FLASHCARD
class FlashCard {
public:
	FlashCard();
	explicit FlashCard(const char* question, const char* answer);
	~FlashCard();

	std::string question;
	std::string answer;
	int id;

	void setHint(const char *hint);
	const char *getHint();

	virtual void ask(int *successCount, 
                     int *failureCount, 
                     int *questionCount, 
                     int *result, 
                     bool *hintAllowed);

private:
	std::string hint;
};
#endif
