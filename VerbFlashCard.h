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
#include "FlashCard.h"

extern bool LEARNING_MODE;

extern int SIGNAL;
extern void send_signal(int new_sig);
extern int recv_signal();

#ifndef _VERB_FLASHCARD
#define _VERB_FLASHCARD
class VerbFlashCard : public FlashCard {
public:
	VerbFlashCard();

	VerbFlashCard(const char *question, 
                  const char *answer, 
                  std::vector<std::string> *verbList);

	struct Conjugation {
        std::deque<std::string> forms;
        Conjugation();
        ~Conjugation();
	};	

	std::map<std::string, std::deque<std::string> > conjugations; //hash table that maps verb tense to a given conjugation.		
	
	//overrides base class variant of ask()...iterates over present conjugations...
	virtual void ask(int *successCount, int *failureCount, int *questionCount, int *result, bool *hintAllowed);

	~VerbFlashCard();

private:
	//std::map<std::string, std::deque<std::string> > conjugations; //maps conjugations to a deque of strings
};
#endif