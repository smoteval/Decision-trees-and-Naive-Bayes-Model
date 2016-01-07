#include <vector>
#include <iostream>
#include <fstream>
#include <math.h> // log2 computes log (output is double)
#include <queue> 
#include <map> 				
using namespace std;

//Forward declarations needed:
class TreeNode;
class TreeNodePointer; 	

// Global variables neede:
TreeNode *DT;  // Decision Tree
bool TrainData[1061][3566]; /* TrainData[i][j] is  true if article i contains word j, otherwise false */
unsigned int TrainLabel[1061]; // Label of each artile (1: atheism, 2:Graphics)

bool TestData[707][3566];
unsigned int TestLabel[707];
map<int, string> m;

priority_queue<TreeNodePointer> PQ;

/* Implementation of our decison tree
   which in this problem is just a binary tree
*/   
class TreeNode {
public:
	TreeNode *left;
	TreeNode *right;
	unsigned int word; // The word by which we might split this node (Has been evalutaed to give the best information gain)
	unsigned int val; /* val is the value that should be estimated on this node if we dont
	                     want to consider more decision (or there are no more decision)
	                     1 means atheism, 2 mean graphics
	                  */  
	double IGain; // The information gained by splitting this node by word 
	vector<unsigned int> EX;                  
	TreeNode(unsigned int val, unsigned int word, double IGain, vector<unsigned int> EX): val(val), word(word), IGain(IGain), EX(EX) {
		left = NULL;
		right = NULL;
	} 
};


/*
  This is a wrapper class for me to be able to use a priority queue of pointers of TreeNode
*/
class TreeNodePointer {
public:
	TreeNode *p;
	TreeNodePointer(TreeNode *p): p(p) {}
};

 


// Estimates which kind of atricles appears most in the given examples
unsigned int PointEstimate(vector<unsigned int> E);


// This function computes the information content of a set of articles
double IC(vector<unsigned int> &E);

// This function computes the information gain of type 1 (average info gain) given a word and a set of examples
double IG_1(vector<unsigned int> &E, unsigned int word);


// This function computes the information gain of type 2 (weighted info gain) given a word and a set of examples
double IG_2(vector<unsigned int> &E, unsigned int word);


/* This function compute the best word among words to split the example on
   The computation is based on the type of information gain we want
   if type = 1, we consider the average information gain
   if type = 2, we consider the weighted information gain
*/    
unsigned int BestWord(vector<unsigned int> &E, vector<unsigned int> &words, unsigned int type);


/*
  This produces the decision tree
  If type = 1 we consider the splits based on average information gain
  if type = 2 we consider the splits based on Weighted information gain
*/
void DT_Learner(unsigned int type);

/*
  Based on the decision tree produced by DT_Learner, this funcrion determines wether or not
  a given article from our test set is decided correctly by the decision tree
  True means the decision guessed the topic of this article correctly, false otherwise
*/
bool TestCorect(unsigned int test, unsigned int NumOfNodes);


/*
  Based on the decision tree produced by DT_Learner, this funcrion computes the accuracy
  of our test data given the type of the information gain consideration we use and 
  the number of nodes we want to explore in the graph
*/
double TestAccuracy( unsigned int NumOfNodes);

bool operator<(const TreeNodePointer a, const TreeNodePointer b) {
   return a.p->IGain < b.p->IGain;
}

unsigned int PointEstimate(vector<unsigned int> E) {
	unsigned int N1 = 0; // Number of elements of E that are about atheism
	unsigned int N2 = 0; // Number of elements of E that are about graphics
	for(int i = 0; i < E.size(); i++) {
		if(TrainLabel[E[i]] == 1) {
			N1 = N1 + 1;
		}
	}
	N2 = E.size() - N1;
	if(N1 >= N2) {
		return 1;
	}
	else {
		return 2;
	}
}

//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

double IC(vector<unsigned int> &E) {
	double N1 = 0; // Number of articles in E that are about atheism
	double N2 = 0; // Number of articles in E that are about graphics
	double size = E.size();
	for(int i = 0; i < E.size(); i++) {
		if(TrainLabel[E[i]] == 1) {
			N1 = N1 + 1;
		}
		else {
			N2 = N2 + 1;
		}
	}
	if(E.size() == 0) {
		return 1;
	}
	double P1 = N1/size; // P(e in E is about atheism)
	double P2 = N2/size; // P(e in E is about graphics)
	double result;
	if(P1 == 0 && P2 == 0) {
		result = 1;
	}
	else if(P1 == 0) {
		result = ((-P2*log2(P2)));
	}
	else if(P2 == 0) {
		result = ((-P1*log2(P1)));
	}
	else {
		result = ((-P1*log2(P1))+(-P2*log2(P2)));
	}
	return result;
}

double IG_1(vector<unsigned int> &E, unsigned int word) {
	if(E.size() == 0) {
		return 0;
	}	
	vector<unsigned int> E1; // The articles in E that contain the word
	vector<unsigned int> E2; // The articles in E that do not contain the word
	for(int i = 0; i < E.size(); i++) {
		if(TrainData[E[i]][word]) {
			E1.push_back(E[i]);
		}
		else {
			E2.push_back(E[i]);
		}
	}
	double IC_E = IC(E); // Information content of E
	double IC_E1 = IC(E1); // Information content of E1
	double IC_E2 = IC(E2); // Information content of E2
	double result = IC_E - ((0.5*IC_E1) + (0.5*IC_E2));
	return result;
}


double IG_2(vector<unsigned int> &E, unsigned int word) {
	if(E.size() == 0) {
		return 0;
	}
	vector<unsigned int> E1; // The articles in E that contain the word
	vector<unsigned int> E2; // The articles in E that do not contain the word
	for(int i = 0; i < E.size(); i++) {
		if(TrainData[E[i]][word]) {
			E1.push_back(E[i]);
		}
		else {
			E2.push_back(E[i]);
		}
	}
	double IC_E = IC(E); // Information content of E
	double IC_E1 = IC(E1); // Information content of E1
	double IC_E2 = IC(E2); // Information content of E2
	double sizeE1 = E1.size();
	double sizeE2 = E2.size();
	double sizeE = E.size();
	double result = IC_E - ( ( (sizeE1/sizeE) * IC_E1) + ( (sizeE2/sizeE) * IC_E2) );
	return result;
}

////////////////////////////////////////////////////////////

unsigned int BestWord(vector<unsigned int> &E, vector<unsigned int> &words, unsigned int type) {
	unsigned int result = words[0];
	double IGOfWord;
	if( type == 1) {
		IGOfWord = IG_1(E, result);
	}
	if( type == 2) {
		IGOfWord = IG_2(E, result);
	}
	for(int i = 1; i < words.size(); i++) {
		double IG;
		if( type == 1) {
			IG = IG_1(E, words[i]);
		}
		else {
			IG = IG_2(E, words[i]);
		}
		if(IGOfWord < IG) {
			result = words[i];
			IGOfWord = IG;
		}
	}
	return result;
}


void DT_Learner(unsigned int type) {
	vector<unsigned int> E; // all the training examples
	vector<unsigned int> words; // all the words
	for(int i = 0; i < 1061; i++) {
		E.push_back(i);
	}
	for(int i = 0; i < 3566; i++) {
		words.push_back(i);
	}	
	unsigned int RootValue = PointEstimate(E);
	unsigned int bestword =  BestWord(E,words,type);
	double IG;
	if (type == 1) {
		IG = IG_1(E,bestword);
	}
	else {
		IG = IG_2(E,bestword);
	}
	DT = new TreeNode(RootValue, bestword,IG,E);
	TreeNodePointer pointer = TreeNodePointer(DT);
	PQ.push(pointer);
	for(int i = 0; i < 100;  i++)  // WHile stopping criteria is not met
	{
		TreeNode *CurNode = PQ.top().p;
		PQ.pop();
		vector<unsigned int> examples = CurNode->EX; 
		int c1 = 0;
		int c2 = 0;
		for(int i = 0; i < examples.size(); i++) {
			if(TrainLabel[examples[i]] == 1){
				c1 = c1+1;
			}
			else{
				c2 = c2+1;
			}
		}
		vector<unsigned int> E1; // All the elements of e in Examples such that e contains the word CurNode->word
		vector<unsigned int> E2; // Rest of the elements of e
		for(int i = 0; i < examples.size(); i++) {
			if(TrainData[examples[i]][CurNode->word]) {
				E1.push_back(examples[i]);
			}
			else {
				E2.push_back(examples[i]);
			}
		}
		unsigned int LeftVal = PointEstimate(E2);
		unsigned int RightVal = PointEstimate(E1);
		//words.erase(std::remove(words.begin(), words.end(), bestwordforE1), words.end());
		unsigned int bestwordforE2 = BestWord(E2,words,type);
		unsigned int bestwordforE1 = BestWord(E1,words,type);
		double IG1; // information gain of E1
		double IG2; // information gain of E2
		if(type == 1) {
			IG1 = IG_1(E1,bestwordforE1);
			IG2 = IG_1(E2,bestwordforE2);
		}
		else {
			IG1 = IG_2(E1,bestwordforE1);
			IG2 = IG_2(E2,bestwordforE2);
		}
		CurNode->left = new TreeNode(LeftVal,bestwordforE2,IG2, E2 );
		CurNode->right = new TreeNode(RightVal,bestwordforE1,IG1, E1 );	
		TreeNodePointer pointerleft = TreeNodePointer(CurNode->left);
		TreeNodePointer pointerright = TreeNodePointer(CurNode->right);
		PQ.push(pointerleft);
		PQ.push(pointerright);
	}
}




bool TestCorect(unsigned int test, unsigned int NumOfNodes) {
	TreeNode *n = DT;
	for(int i = 0; i < NumOfNodes && n != NULL; i++) {
		if(TestData[test][n->word]) {
			if(n->right == NULL) {
				break;
			}
			else {
				n = n->right;
			}
		}
		else {
			if(n->left == NULL) {
				break;
			}
			else {
				n = n->left;
			}
		}
	}
	if(TestLabel[test] == n->val) {
		return true;
	}
	else {
		return false;
	}
}

double TestAccuracy(unsigned int NumOfNodes) {
	int incorrect = 0;
	int correct = 0;
	for(int i = 0; i < 707; i++) {
		if(TestCorect(i, NumOfNodes)) {
			correct = correct + 1;
		}
		else {
			incorrect = incorrect + 1;
		}
	}
	double result = correct/(double)(correct+incorrect);
	return result;
}



bool TrainCorect(unsigned int test, unsigned int NumOfNodes) {
	TreeNode *n = DT;
	for(int i = 0; i < NumOfNodes && n != NULL; i++) {
		if(TrainData[test][n->word]) {
			if(n->right == NULL) {
				break;
			}
			else {
				n = n->right;
			}
		}
		else {
			if(n->left == NULL) {
				break;
			}
			else {
				n = n->left;
			}
		}
	}
	if(TrainLabel[test] == n->val) {
		return true;
	}
	else {
		return false;
	}
}

double TrainAccuracy(unsigned int NumOfNodes) {
	int incorrect = 0;
	int correct = 0;
	for(int i = 0; i < 1061; i++) {
		if(TrainCorect(i, NumOfNodes)) {
			correct = correct + 1;
		}
		else {
			incorrect = incorrect + 1;
		}
	}
	double result =  correct/(double)(correct+incorrect);
	return result;
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
	file.open("words.txt");
	for(int i = 0; i < 3566; i++) {
		string s;
		file >> s;
		m[i] = s;
	}
	file.close();
	/////////////////////////// End of reading data from files ///////////////////////////////////////////
	DT_Learner(2);
	ofstream myfile;
	myfile.open("TrainigAccuracyWith2");
	for(int i = 1; i <= 100; i++) {
		double y = TrainAccuracy(i);
		myfile << y << endl;
	}
	myfile.close();
	myfile.open("TestAccuracyWith2");
	for(int i = 1; i <= 100; i++) {
		double y = TestAccuracy(i);
		myfile << y << endl;
	}	
	myfile.close();
	DT = NULL;
	DT_Learner(1);
	PQ = priority_queue<TreeNodePointer>();
	myfile.open("TrainigAccuracyWith1");
	for(int i = 1; i <= 100; i++) {
		double y = TrainAccuracy(i);
		myfile << y << endl;
	}
	myfile.close();
	myfile.open("TestAccuracyWith1");
	for(int i = 1; i <= 100; i++) {
		double y = TestAccuracy(i);
		myfile << y << endl;
	}	
	myfile.close();
}





