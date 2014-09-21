/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

class FlashCard;
class VerbFlashCard;

static int send_signal(int);
static int recv_signal();

typedef struct _VerbExistsRetObj {
    bool status;
    VerbFlashCard *targetFlashCard; //a pointer to an already existing conjugation object in memory (pointer to a pointer)

} VerbExistsRetObj;

static void deallocate(std::deque<FlashCard*>*);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
VerbExistsRetObj *contains_verb(std::deque<FlashCard*>*, std::string); //accepts 2 parameters: a deque of cards and a string holding the verb name
