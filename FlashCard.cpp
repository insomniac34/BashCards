/*
 *                          Copyright (C) 2014 Tyler Raborn
 */

    #include "FlashCard.h"

    FlashCard::FlashCard() {}

    FlashCard::FlashCard(const char* question, const char* answer) {
        this->question = std::string(question);
        this->answer = std::string(answer);
        this->hint = " ";
        //std::cout << "added " << this->question << " and " << this->answer << "to flashcards!";
    }

    FlashCard::~FlashCard() {}	

    void FlashCard::setHint(const char *hint) {
        this->hint = std::string(hint);
    }

    const char *FlashCard::getHint() {
        return this->hint.c_str();
    }

    void FlashCard::ask(int *successCount, int *failureCount, int *questionCount, int *result, bool *hintAllowed) {
        std::string ans;
		std::cout << "Define: " << this->question << " (s: stop, h: view hint, sh: set hint, m: mark, p: view current score, c: clear terminal) " << this->getHint() << std::endl;		
        //std::cin >> ans;
		std::getline(std::cin, ans);

		if (ans.compare("q") == 0) {
			send_signal(-1);
		}
        else if (ans.compare("h") == 0) {
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
        else if (ans.compare("D") == 0) {
            send_signal(5);
        }   
        else if ((ans.compare("?") == 0) && LEARNING_MODE && (*hintAllowed)) {
        	std::cout << "The answer is " << this->answer << std::endl;
        	*hintAllowed = false;
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
            
            if (questionCount!=0) std::cout << " (Current Score: " << (100*((float)(*successCount) / (float)(*questionCount))) << "%)" << std::endl;            
        	*questionCount+=1;
        }                                      
	}