/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

#include "BashCards.h"
#include "config.h"

static bool DEBUG_OUTPUT = false;
std::string verbs_file = "verbs.txt";

std::deque<bool> hintList;
bool initial_run=true;

bool smartFlag = true;
bool reverseFlag = false;
std::string targetFile = "flashcards.txt";

std::deque<std::string> args;
bool contains(const std::string &str) {
	return (args.end() != std::find(args.begin(), args.end(), str));
}

int main(int argc, char **argv) {
	srand(time(NULL));

	SIGNAL=-2;
	LEARNING_MODE=false;

	for (int i = 0; i < argc; i++) {
		args.push_front(std::string(argv[i]));
	}	

	if (argc >= 2) {
		if (strcmp(argv[1], "-f") == 0) {
			printf("Using vocab file %s!\n", argv[2]);
			targetFile = std::string(argv[2]);
		}
		else if (strcmp(argv[1], "-n") == 0) {
			//printf("adding question %s and answer %s to flashcards!\n", argv[2], argv[3]);
			std::ofstream strm;
			strm.open(targetFile.c_str(), std::fstream::app);
			strm << std::string(argv[2]) + "|" + std::string(argv[3]) + "\n";
			strm.close();
			return 0;
		}	
		else if (strcmp(argv[1], "-v") == 0) {
			printf("Using verbs file %s!\n", argv[2]);
			verbs_file = std::string(argv[2]);			
		}

		if (strcmp(argv[1], "-r") == 0) {
			reverseFlag = true;
			printf("Reverse flashcard mode engaged!\n");
		}		

		if (contains("-l")) {
			printf("**Learning mode engaged!**\n");
			LEARNING_MODE = true;
		}
	}
	
	//run through flashcards:
	std::deque<FlashCard*> cardList;

	std::ifstream input_stream;
	std::string line;
	input_stream.open(targetFile.c_str());

	while (std::getline(input_stream, line)) {
		if (line.c_str()[0] != '#') {
			
			std::string delimiter = "|";
			std::string question = line.substr(0, line.find(delimiter));
			std::string answer = line.substr(question.length()+1, line.length()-question.length());

			FlashCard *newFlashCard = (reverseFlag) ? new FlashCard(answer.c_str(), question.c_str()) : new FlashCard(question.c_str(), answer.c_str());
			cardList.push_front(newFlashCard);
			hintList.push_front(true);
		}
	}

	input_stream.close();

	std::ifstream verb_stream;
	verb_stream.open(verbs_file);

	//printf("verbs.txt is open!\n");
	if (verb_stream.good()) {

		//verbs.txt contains verbs in the following format: verb_name|english_definition|conjugation|yo|tu|el/ella|nos|vos|ellos
		while (std::getline(verb_stream, line)) {
			std::cout << "reading " + line << std::endl;

			if (line.c_str()[0] != '#') {

				char delimiter = '|';
				std::vector<std::string> curVerb = split(line, delimiter); //get verb data in a vector
				
				//////////////////DEBUG/////////////////////////
				printf("vector contains: ");
				for (std::vector<std::string>::iterator iter = curVerb.begin(); iter != curVerb.end(); iter++) {
					printf("%s, ", (*iter).c_str());
				}
				printf("\n");
				////////////////////////////////////////////////

				//is verb already added? if so add a new conjugation
				VerbExistsRetObj *verbExistsRetObj = contains_verb(&cardList, curVerb[0]); //does the list of flash cards contain a verb with this name?
				if (verbExistsRetObj->status == true) {

                    std::deque<std::string> conjugationList;
                    for (int i = 3; i < 9; i++) {
                    	conjugationList.push_back(curVerb[i]);
                    }		
                    puts("bp2");
                    verbExistsRetObj->targetFlashCard->conjugations.insert(std::pair<std::string, std::deque<std::string> > (curVerb[2], conjugationList));
				}
				else {
					//verb does NOT exist...need to create a new flash card...
					VerbFlashCard *newVerbFlashCard = new VerbFlashCard(curVerb[0].c_str(), curVerb[1].c_str(), &curVerb);
					cardList.push_front(newVerbFlashCard);
				}

				puts("bp3");

				delete verbExistsRetObj;
				puts("bp4");
			}
		}
	}

	verb_stream.close();

	std::deque<int> results(cardList.size(), 0);

	if (cardList.size() <= 0) {
		puts("ERROR: You need to add some flashcards first!");
		return 0;
	}

	char order_ans;
	printf("Beginning run! Enter stop to quit! Do you want the flashcards to be presented randomly(r) or in-order(o)?\n");
	scanf("%c", &order_ans);

	bool randomFlag = (order_ans == 'r') ? true : false;

	int index = (rand()%cardList.size());
	char buf[128];
	memset(buf, 0, 128*sizeof(char));

	int failureCount = 0;
	int questionCount = 0;
	int successCount = 0; 

    bool terminate = false;
	while(true) {
		std::cin.clear();
		std::string ans;
		std::string hint_available = " ";
		if (strcmp(cardList[index]->getHint(), " ")!=0) {
			hint_available = "(hint available!)";
		}

        if (initial_run) {
        	initial_run=false;
            std::cout << "clearing buffers!" << std::endl;
            std::cin.clear();
        	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
		std::cout << std::endl;
		cardList[index]->ask(
							 &successCount, 
							 &failureCount, 
							 &questionCount, 
							 &results[index],
							 &(hintList[index])
							);

        switch(recv_signal()) {
            case -1:
                terminate = true;
                std::cout << std::endl << "**BashCards: bye!**" << std::endl;
                break;
            case 0: //set hint
                {
                    std::string newHint;
                    std::cout << "Please enter the hint for this word: " << std::endl;
                    //std::cin >> newHint;
                    std::getline(std::cin, newHint);
                    cardList[index]->setHint(newHint.c_str());
                    break;
                }
            case 1:
                break;
            case 2: //view hint
                std::cout << std::string(cardList[index]->getHint()) << std::endl; 
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
            	std::cout << "viewing raw array: " << std::endl;
            	//for (std::deque<int>::iterator iter = results.begin(); iter != results.end(); iter++) std::cout << std::string("[") << *iter << std::string("]") << " -> " + cardList[std::distance(results.begin(), iter)]->question << " ("+cardList[std::distance(results.begin(), iter)]->answer+")";
            	for (std::deque<int>::iterator iter = results.begin(); iter != results.end(); iter++) std::cout << std::string("[") << *iter << std::string("]") << "(" + cardList[std::distance(results.begin(), iter)]->question + ")" << std::endl;
            	std::cout << std::endl;
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;

            default:
                break;
        }

        if (terminate) break;

        if (smartFlag && results[index] <= FAILURE_THRESHOLD) {

        	//TODO: Still adds a duplicate if you answer correctly if value < threshold

            std::cout << std::endl << "**BashCards: Adding duplicate word \"" << cardList[index]->question << "\" due to failure count!**" << std::endl;
            FlashCard *duplicate = (reverseFlag) ? new FlashCard(cardList[index]->answer.c_str(), cardList[index]->question.c_str()) : new FlashCard(cardList[index]->question.c_str(), cardList[index]->answer.c_str());
            cardList.push_front(duplicate);
            results.push_front(0);
            bool newHint = (LEARNING_MODE) ? true : false;
            hintList.push_front(newHint);
        }

        if (smartFlag && results[index] >= SUCCESS_THRESHOLD) {
        	std::cout << std::endl << "**BashCards: Removing word \"" << cardList[index]->question << "\" due to success count!**" << std::endl;
        	results.erase(results.begin()+index);
        	FlashCard *fc = *(cardList.begin()+index);
        	cardList.erase(cardList.begin()+index);
        	delete fc;
        	hintList.erase(hintList.begin()+index);
        }

        if (cardList.size() == 0) {
        	std::cout << "YOU WON LOL" << std::endl;
        	break;
        }

		if (!randomFlag) index+=1;
		else index = rand() % cardList.size();

		if (!randomFlag && index==cardList.size()) {
			break;
		}

		//system("clear");
		ans.clear();
	}
	deallocate(&cardList);
	return 0;
}

static void deallocate(std::deque<FlashCard*>* dq) {
	while(!dq->empty()) {
		FlashCard *flashCardPtr = dq->back();
		dq->pop_back();
		delete flashCardPtr;
	}
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

VerbExistsRetObj *contains_verb(std::deque<FlashCard*> *cards, std::string verbTitle) {

	VerbExistsRetObj *ret = new VerbExistsRetObj;
	ret->status = false;

	bool verbFound = false;
	std::deque<FlashCard*>::iterator iter;
	for(iter = cards->begin(); iter != cards->end(); iter++) {
		if (strcmp((*iter)->question.c_str(), verbTitle.c_str())==0) {
			verbFound = true;
			break;
		}
	}

	if (verbFound) {
		ret->targetFlashCard = (VerbFlashCard*)(*iter);
		ret->status = true;
	}

	return ret;
}

/* signal system for communicating between main 'game' loop and flashcard methods*/
void send_signal(int new_sig) {
    SIGNAL = new_sig;
}

int recv_signal() {
    int ret = SIGNAL;
    SIGNAL = -2; //reset
    return ret;
}
