/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

#include "VerbFlashCard.h"

static const std::string verb_forms[6] = {"yo form", "tu form", "el/ella form", "nosotros form", "vosotros form", "ellos/ellas/ustedes form"};

VerbFlashCard::VerbFlashCard() {}

VerbFlashCard::VerbFlashCard(const char *question, 
              const char *answer, 
              std::vector<std::string> *verbList) : FlashCard(question, answer) {

    std::deque<std::string> curConjugation;

    curConjugation.push_back((*verbList)[3]);
    curConjugation.push_back((*verbList)[4]);
    curConjugation.push_back((*verbList)[5]);
    curConjugation.push_back((*verbList)[6]);
    curConjugation.push_back((*verbList)[7]);
    curConjugation.push_back((*verbList)[8]);

    this->conjugations.insert(std::pair<std::string, std::deque<std::string> > ((*verbList)[2], curConjugation));	
}

VerbFlashCard::~VerbFlashCard() {}


VerbFlashCard::Conjugation::Conjugation() {
    this->forms = std::deque<std::string> (6, " ");
}
VerbFlashCard::Conjugation::~Conjugation() {}

    //overrides base class variant of ask()...iterates over present conjugations...
    void VerbFlashCard::ask(int *successCount, int *failureCount, int *questionCount, int *result, bool *hintAllowed)  {
        for (std::map<std::string, std::deque<std::string> >::iterator iter = this->conjugations.begin(); iter != this->conjugations.end(); iter++) {
            std::string formStates[6] = {"***", "***", "***", "***", "***", "***"};

            int localSuccessCount = 0;
            for (int i = 0; i < 6; i++) {
                std::string ans;                
                std::cout << std::endl <<"Conjugation of " + this->question << " in the " << iter->first << std::endl << "******************************" << std::endl;
                for (int j = 0; j < 3; j++) {
                    std::cout << formStates[j] + " " + formStates[5-j] << std::endl;
                }
                std::cout << "******************************" << std::endl;
                std::cout << std::endl << "What is the " + verb_forms[i] + " of the " << iter->first << " tense of the verb " << this->question << "?" << std::endl;
                std::getline(std::cin, ans);

                if (ans.compare("q")==0) {
                    send_signal(EXIT_SIGNAL);
                    break;
                }

                std::string reply = (ans.compare(iter->second[i]) == 0) ? "Correct!" : "Incorrect! The answer is " + iter->second[i];
                std::cout << reply;     

                if (ans.compare(iter->second[i]) == 0) {
                    //*successCount+=1;
                    //*result+=1;
                    localSuccessCount+=1;
                    formStates[i] = std::string(ans);
                }
                else {
                    //*failureCount+=1;
                    //*result-=1;
                    localSuccessCount-=1;
                    formStates[i] = std::string("XXX");
                }          
            }           
            if (localSuccessCount == 6) 
            {
                std::cout << localSuccessCount << " verbs succesfully conjuated!" << std::endl;
                *successCount+=1;
                *result+=1;
            }
            else 
            {
                std::cout << "Verb NOT correctly conjuated!" << std::endl;
                *failureCount+=1;
                *result-=1;
            }      
            *questionCount+=1;  
            //check for div by 0
            if (questionCount != 0) std::cout << " (Current Score: " << (100*((float)(*successCount)/(float)(*questionCount))) << "%)" << std::endl;            
        }       
    }

