#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>		
#include <cmath>	
#include <math.h>
using namespace std;

// Global variables neede:
bool TrainData[1061][3566]; /* TrainData[i][j] is  true if article i contains word j, otherwise false */
unsigned int TrainLabel[1061]; // Label of each artile (1: atheism, 2:Graphics)

bool TestData[707][3566];
unsigned int TestLabel[707];

double theta; // Pr(An article is about atheism)

//Forward declaration
class Word;
vector<Word *> words;


class Word {
public:
	unsigned int index; // index of this word in the TrainData array
	double theta_1; // Pr(word is in the article | Article is about atheism)
	double theta_2; // Pr(word is in the article | Article is about Graphics)
	/*
	  SO (1-theta_1) = Pr(word is not in the article | Article is about atheism)
	     (1-theta_2) = Pr(word is not in the article | Article is about Graphics)
	*/
	Word(unsigned int index, double theta_1, double theta_2): index(index), theta_1(theta_1), theta_2(theta_2) {}     
};


/*
   In this function we go through all words and assign the appropriate probabilities and put the
   words in the vector words
*/
void BuildWords() {
	for(unsigned int i = 0; i < 3566; i++) {
		double N1 = 1; // # of Articles about atheism that contain the word i
		double N2 = 1; // # of Articles about graphics that contain the word i
		double Atheism = 2; // # of articles about atheism
		double Graphics = 2; // # of articles about graphics
		for(int j = 0; j < 1061; j++) {
			if(TrainLabel[j] == 1) {
				Atheism = Atheism+1;
				if(TrainData[j][i]) {
					N1 = N1+1;
				}
			}
			else {
				Graphics = Graphics+1;
				if(TrainData[j][i]) {
					N2 = N2+1;
				}
			}
		}
		double Th1 = N1/Atheism;
		double Th2 = N2/Graphics;
		Word *word = new Word(i, Th1, Th2);
		words.push_back(word);
	}
}


void computetheta() {
	double N1 = 0; // # of articles about atheism
	for(int i = 0; i < 1061; i++) {
		if(TrainLabel[i] == 1) {
			N1 = N1+1;
		}
	}
	theta = N1/(double) 1061;
}


/*
   computes the probability (not normalized) of an article in the testData to be about atheism
*/
double P_Atheism_Test(unsigned int article) {
	double p = 1;
	for(int i = 0; i < words.size(); i++) {
		if(TestData[article][words[i]->index]) {
			p = p*words[i]->theta_1;
		}
		else {
			p = p*(1-words[i]->theta_1);
		}
	}
	p = p*theta;
	return p;
}


double P_Graphics_Test(unsigned int article) {
	double p = 1;
	for(int i = 0; i < words.size(); i++) {
		if(TestData[article][words[i]->index]) {
			p = p*words[i]->theta_2;
		}
		else {
			p = p*(1-words[i]->theta_2);
		}
	}
	p = p*(1-theta);
	return p;
}

double P_Atheism_Train(unsigned int article) {
	double p = 1;
	for(int i = 0; i < words.size(); i++) {
		if(TrainData[article][words[i]->index]) {
			p = p*words[i]->theta_1;
		}
		else {
			double a = (double)(1-words[i]->theta_1);
			p = p*a;
		}
	}
	p = p*theta;
	return p;
}


double P_Graphics_Train(unsigned int article) {
	double p = 1;
	for(int i = 0; i < words.size(); i++) {
		if(TrainData[article][words[i]->index]) {
			p = p * words[i]->theta_2;
		}
		else {
			double a = (double)(1-words[i]->theta_2);
			p = p*a;
		}
	}
	double b = (double) (1-theta);
	p = p*b;
	return p;
}


unsigned int GuessLabel_Test(unsigned int article) {
	double p_Atheism = P_Atheism_Test(article);
	double p_Graphics = P_Graphics_Test(article);
	if(p_Atheism >= p_Graphics) {
		return 1;
	}
	else {
		return 2;
	}
}

unsigned int GuessLabel_Train(unsigned int article) {
	double p_Atheism = P_Atheism_Train(article);
	double p_Graphics = P_Graphics_Train(article);
	if(TrainLabel[article] == 1) {
		//cout << p_Atheism << " " << p_Graphics << endl;
	}
	if(p_Atheism >= p_Graphics) {
		return 1;
	}
	else {
		return 2;
	}
}


double Percent_Test() {
	double N1 = 0;
	for(int i = 0; i < 707; i++) {
		if(GuessLabel_Test(i) == TestLabel[i]) {
			N1 = N1+1;
		}
	}
	return N1/(double)707;
}

double Percent_Train() {
	double N1 = 0;
	for(int i = 0; i < 1061; i++) {
		if(GuessLabel_Train(i) == TrainLabel[i]) {
			N1 = N1+1;
		}
	}
	return N1/(double)1061;
}

void initiate() {
	for(int i = 0; i < 1061; i++) {
		for(int j = 0; j < 3566; j++) {
			TrainData[i][j] = false;
		}
	}
	for(int i = 0; i < 707; i++) {
		for(int j = 0; j < 3566; j++) {
			TestData[i][j] = false;
		}
	}
}

void print10words() {
	for(int j = 0; j < 10; j++) {
		double maxdif = -1;
		double word;
		Word *theword;
		for(int i = 0; i < words.size(); i++) {
		    double diff = abs(log(words[i]->theta_1) - log(words[i]->theta_2));
		    if(diff > maxdif) {
		    	maxdif = diff;
		    	word = words[i]->index;
		    	theword = words[i];
		    }
		}
		cout << "Index of word " << j << " is " << word+1 << endl;
		words.erase(std::remove(words.begin(), words.end(), theword), words.end());		
	}
}

int main() {
	///////////////////// Reading the data from files//////////////////////////////////////
	initiate();
	ifstream file;
	file.open("trainData.txt");
	for(int i = 0; i < 57095; i++) {
		int articleNumber;
		int wordNumber;
		file >> articleNumber;
		file >> wordNumber;
		wordNumber = wordNumber-1;
		articleNumber = articleNumber-1;
		TrainData[articleNumber][wordNumber] = true;
	}
	file.close();
	file.open("trainLabel.txt");
	for(int i = 0; i < 1061; i++) {
		unsigned int l;
		file >> l;
		TrainLabel[i] = l; 
	}
	file.close();
	file.open("testData.txt");
	for(int i = 0; i < 41115; i++) {
		int articleNumber;
		int wordNumber;
		file >> articleNumber;
		file >> wordNumber;
		wordNumber = wordNumber-1;
		articleNumber = articleNumber-1;
		TestData[articleNumber][wordNumber] = true;		
	}
	file.close();
	file.open("testLabel.txt");
	for(int i = 0; i < 707; i++) {
		unsigned int l;
		file >> l;
		TestLabel[i] = l; 		
	}
	file.close();
	/////////////////////////// End of reading data from files ///////////////////////////////////////////	
	BuildWords();
	computetheta();
	double TestDataAccuracy = Percent_Test();
	double TrainDataAccuracy = Percent_Train();
	cout << "Test Data Accuracy " << TestDataAccuracy << endl;
	cout << "Train Data Accuracy " << TrainDataAccuracy << endl;
	cout << "top 10 words:" << endl;
	print10words();
}







