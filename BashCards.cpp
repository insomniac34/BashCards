/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

#include "BashCards.h"

static const std::string verb_forms[6] = {"yo form", "tu form", "el/ellos/usted form", "nosotros form", "vosotros form", "ellos/ellas/ustedes form"};
static int SIGNAL=-2;

class FlashCard {
public:
	FlashCard() {}

	explicit FlashCard(const char* question, const char* answer) {
		this->question = std::string(question);
		this->answer = std::string(answer);
		this->hint = " ";
		//std::cout << "added " << this->question << " and " << this->answer << "to flashcards!";
	}

	~FlashCard() {}

	std::string question;
	std::string answer;

	int id;

	void setHint(const char *hint) {
		this->hint = std::string(hint);
	}
	
	const char *getHint() {
		return this->hint.c_str();
	}

	virtual void ask(int *successCount, int *failureCount, int *questionCount, int *result) {
        std::string ans;
		std::cout << "Pregunta: " << this->question << " (s: stop, h: view hint, sh: set hint, m: mark, p: view current score, c: clear terminal) " << this->getHint() << std::endl;		
        std::cin >> ans;

        if (ans.compare("h") == 0) {
            send_signal(2);
        }
        else if (ans.compare("sh") == 0) {
            send_signal(1);
        }
        else if (ans.compare("s") == 0) {
            send_signal(-1);
        }      
        else if (ans.compare("m") == 0) {
            send_signal(3);
        }                
        else if (ans.compare("p") == 0) {
            send_signal(4);
        }       
        else if (ans.compare("c") == 0) {
            send_signal(5);
        }   
        else {
            //std::cout << "comparing " << ans << " to " << this->answer << std::endl;
            std::string reply = (ans.compare(this->answer) == 0) ? "Correct!" : "Wrong! The answer is " + this->answer;
            std::cout << reply;

            //TODO: make cleaner and integrate with above condition evaluations
            if (ans.compare(this->answer) == 0) {
                *successCount+=1;
                *result+=1;
            }
            else {
                *failureCount+=1;
                *result-=1;
            }
            *questionCount+=1;
            std::cout << " (Current Score: " << (100*((float)(*successCount) / (float)(*questionCount))) << "%)" << std::endl;            
        }                                      
	}

private:

	std::string hint;
};

class VerbFlashCard : public FlashCard {
public:
	VerbFlashCard() {}
	typedef struct _Conjugation {
        std::deque<std::string> forms;
        _Conjugation() {
            this->forms = std::deque<std::string> (6, " ");
        }
        ~_Conjugation() {}
	} Conjugation;	

	std::unordered_map<std::string, Conjugation> conjugations; //hash table that maps verb tense to a given conjugation.	

	VerbFlashCard(const char *question, const char *answer, std::vector<std::string> *verbList) : FlashCard(question, answer) {

        Conjugation curConjugation;
        curConjugation.forms[0] = (*verbList)[3];
        curConjugation.forms[1] = (*verbList)[4];
        curConjugation.forms[2] = (*verbList)[5];
        curConjugation.forms[3] = (*verbList)[6];
        curConjugation.forms[4] = (*verbList)[7];
        curConjugation.forms[5] = (*verbList)[8];                                               

		this->conjugations.insert(std::pair<std::string, Conjugation> ((*verbList)[2], curConjugation));	
	}

	//overrides base class variant of ask()...iterates over present conjugations...
	virtual void ask(int *successCount, int *failureCount, int *questionCount, int *result)  {
		for (std::unordered_map<std::string, Conjugation>::iterator iter = this->conjugations.begin(); iter != this->conjugations.end(); iter++) {
            for (int i = 0; i < 6; i++) {
                std::string ans;
    			std::cout << "What is the " + verb_forms[i] + " of the " << iter->first << " tense of the verb " << this->question << "?" << std::endl;
                std::cin >> ans;
                std::string reply = (ans.compare(iter->second.forms[i]) == 0) ? "Correct!" : "Incorrect! The answer is " + iter->second.forms[i];
                std::cout << reply << std::endl;       

                if (ans.compare(iter->second.forms[i]) == 0) {
                    *successCount+=1;
                    *result+=1;
                }
                else {
                    *failureCount+=1;
                    *result-=1;
                }

                *questionCount+=1;
                std::cout << " (Current Score: " << (100*((float)(*successCount) / (float)(*questionCount))) << "%)" << std::endl;
            
                // Ignore to the end of file & line
            }                   
		}		
	}

	~VerbFlashCard() {}

private:
	//std::map<std::string, std::deque<std::string> > conjugations; //maps conjugations to a deque of strings
};

bool smartFlag = true;
bool reverseFlag = false;
static std::string targetFile = "flashcards.txt";

int main(int argc, char **argv) {
	srand(time(NULL));

	if (argc >= 2) {
		if (strcmp(argv[1], "-f") == 0) {
			printf("Using file %s!\n", argv[2]);
			targetFile = std::string(argv[2]);
		}

		if (strcmp(argv[1], "-n") == 0) {
			//printf("adding question %s and answer %s to flashcards!\n", argv[2], argv[3]);
			std::ofstream strm;
			strm.open(targetFile.c_str(), std::fstream::app);
			strm << std::string(argv[2]) + "|" + std::string(argv[3]) + "\n";
			strm.close();
			return 0;
		}	

		if (strcmp(argv[1], "-r") == 0) {
			reverseFlag = true;
			printf("Reverse flashcard mode engaged!\n");
		}		
	}
	
	//run through flashcards:
	std::deque<FlashCard*> cardList;

	std::ifstream input_stream;
	std::string line;
	input_stream.open(targetFile.c_str());

	while (std::getline(input_stream, line)) {
		std::string delimiter = "|";
		std::string question = line.substr(0, line.find(delimiter));
		std::string answer = line.substr(question.length()+1, line.length()-question.length());

		FlashCard *newFlashCard = (reverseFlag) ? new FlashCard(answer.c_str(), question.c_str()) : new FlashCard(question.c_str(), answer.c_str());
		cardList.push_front(newFlashCard);
	}

	input_stream.close();

	std::ifstream verb_stream;
	verb_stream.open("verbs.txt");

	printf("verbs.txt is open!\n");
	if (verb_stream.good()) {

		//verbs.txt contains verbs in the following format: verb_name|english_definition|conjugation|yo|tu|el/ella|nos|vos|ellos
		while (std::getline(verb_stream, line)) {

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

					VerbFlashCard::Conjugation curConjugation;
                 
                    curConjugation.forms[0] = curVerb[3];
                    curConjugation.forms[1] = curVerb[4];
                    curConjugation.forms[2] = curVerb[5];
                    curConjugation.forms[3] = curVerb[6];
                    curConjugation.forms[4] = curVerb[7];
                    curConjugation.forms[5] = curVerb[8];				

					verbExistsRetObj->targetFlashCard->conjugations.insert(std::pair<std::string, VerbFlashCard::Conjugation> (curVerb[2], curConjugation)); //NOW, a new conjugation has been inserted into the VerbFlashCard object inside of cardList.
				}
				else {
					//verb does NOT exist...need to create a new flash card...
					VerbFlashCard *newVerbFlashCard = new VerbFlashCard(curVerb[0].c_str(), curVerb[1].c_str(), &curVerb);
					cardList.push_front(newVerbFlashCard);
				}

				delete verbExistsRetObj;
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

	int index = 0;
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
		std::cout << std::endl;
		//std::cout << "Pregunta: " << cardList[index]->question << " (s: stop, h: view hint, sh: set hint, m: mark, p: view current score, c: clear terminal) " << hint_available << std::endl;
		cardList[index]->ask(&successCount, &failureCount, &questionCount, &results[index]);

        switch(recv_signal()) {
            case -1:
                terminate = true;
                break;
            case 0: //set hint
                {
                    std::string newHint;
                    std::cout << "Please enter the hint for this word: " << std::endl;
                    std::cin >> newHint;
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

        if (smartFlag && results[index] < -10) {
            std::cout << "adding duplicate due to failure count!" << std::endl;
            FlashCard *duplicate = (reverseFlag) ? new FlashCard(cardList[index]->answer.c_str(), cardList[index]->question.c_str()) : new FlashCard(cardList[index]->question.c_str(), cardList[index]->answer.c_str());
            cardList.push_front(duplicate);
            results.push_front(0);
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

static int send_signal(int new_sig) {
    SIGNAL = new_sig;
}

static int recv_signal() {
    int ret = SIGNAL;
    SIGNAL = -2; //reset
    return ret;
}