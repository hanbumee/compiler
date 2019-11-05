#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <stack>
#include <queue>

using namespace std;

enum tokenType {
	OPENSPAR = 0, CLOSESPAR = 1, OPENMPAR = 2, CLOSEMPAR = 3,
	SEMICOLON = 4,
	KEY_IF = 5, KEY_THEN = 6, KEY_ELSE = 7, KEY_WHILE = 8,
	ASSIGN = 9, LARGER = 10, SMALLER = 11, PLUS = 12,
	WORD = 13, NUM = 14, END = 15,
	ERROR, WSPACE
};

enum tableType {
	WORD_T, NUM_T
};

vector<pair< tokenType, string >> token;
char nextChar;
int valNum = 0;
int symNum = 0;
int labeln = 0;
int regn = 0;

class table_t {
public:
	string name;
	tableType type;
	table_t *next;
};

class Node {
public:
	pair<tokenType, string> id;
	int state;
	vector<Node *> children;
	Node * parent;
	int visitedChild;
	Node() {
		state = 0;
		visitedChild = 0;
	}
};

Node * root;
table_t table[100] = {};
table_t symTable[100] = {};

ifstream in;
ofstream code, symbol2;
//lexer�����

tokenType checkKeyword(string s) {
	int state = 1;       // start state 1, final state 14;
	for (char& c : s) {
		switch (state) {
		case 1:
			if (c == 'I')
				state = 2;
			else if (c == 'T')
				state = 3;
			else if (c == 'E')
				state = 6;
			else if (c == 'W')
				state = 9;
			else
				state = 13;
			break;
		case 2:
			if (c == 'F')
				state = 14;
			else
				state = 13;
			break;
		case 3:
			if (c == 'H')
				state = 4;
			else
				state = 13;
			break;
		case 4:
			if (c == 'E')
				state = 5;
			else
				state = 13;
			break;
		case 5:
			if (c == 'N')
				state = 15;
			else
				state = 13;
			break;
		case 6:
			if (c == 'L')
				state = 7;
			else
				state = 13;
			break;
		case 7:
			if (c == 'S')
				state = 8;
			else
				state = 13;
			break;
		case 8:
			if (c == 'E')
				state = 16;
			else
				state = 13;
			break;
		case 9:
			if (c == 'H')
				state = 10;
			else
				state = 13;
			break;
		case 10:
			if (c == 'I')
				state = 11;
			else
				state = 13;
			break;
		case 11:
			if (c == 'L')
				state = 12;
			else
				state = 13;
			break;
		case 12:
			if (c == 'E')
				state = 17;
			else
				state = 13;
			break;
		case 13:
			break;
		case 14:
			state = 13;
			break;
		case 15:
			state = 13;
			break;
		case 16:
			state = 13;
			break;
		case 17:
			state = 13;
			break;
		}
	}

	if (state == 14)
		return KEY_IF;
	if (state == 15)
		return KEY_THEN;
	if (state == 16)
		return KEY_ELSE;
	if (state == 17)
		return KEY_WHILE;
	return ERROR;
}

tokenType checkWord(string s) {
	int state = 1;
	for (char& c : s) {
		switch (state) {
		case 1:
			if (isalpha(c))
				state = 2;
			else
				state = 3;
			break;
		case 2:
			if (isalpha(c))
				state = 2;
			else
				state = 3;
			break;
		case 3:
			break;
		}
	}

	if (state == 2)
		return WORD;

	return ERROR;
}

tokenType checkNum(string s) {
	int state = 1;
	for (char& c : s) {
		switch (state) {
		case 1:
			if (isdigit(c))
				state = 2;
			else
				state = 3;
			break;
		case 2:
			if (isdigit(c))
				state = 2;
			else
				state = 3;
			break;
		case 3:
			break;
		}
	}

	if (state == 2)
		return NUM;

	return ERROR;
}

void printSymTable() {
	int level;
	table_t *tmp;
	symbol2 << "Total index: " << symNum << endl;
	symbol2 << "#: name\n";

	for (int i = 0; i <= symNum / 100; i++) {
		for (int j = 0; j < 100 && i * 100 + j < symNum; j++) {
			tmp = &symTable[j];
			level = i;
			while (level--) {
				tmp = tmp->next;
			}
			symbol2 << i * 100 + j << ": " << tmp->name << endl;
		}
	}
}
pair< tokenType, string >getToken(char first) {
	tokenType returnToken;
	switch (first) {
	case ' ':
		nextChar = in.get();
		break;

	case '\n':
		nextChar = in.get();
		break;

	case '\t':
		nextChar = in.get();
		break;

	case '+':
		nextChar = in.get();
		return pair< tokenType, string >(PLUS, "+");

	case '=':
		nextChar = in.get();
		return pair< tokenType, string >(ASSIGN, "=");

	case '>':
		nextChar = in.get();
		return pair< tokenType, string >(LARGER, ">");

	case '<':
		nextChar = in.get();
		return pair< tokenType, string >(SMALLER, "<");

	case '(':
		nextChar = in.get();
		return pair< tokenType, string >(OPENSPAR, "(");

	case ')':
		nextChar = in.get();
		return pair< tokenType, string >(CLOSESPAR, ")");

	case '{':
		nextChar = in.get();
		return pair< tokenType, string >(OPENMPAR, "{");

	case '}':
		nextChar = in.get();
		return pair< tokenType, string >(CLOSEMPAR, "}");

	case ';':
		nextChar = in.get();
		return pair< tokenType, string >(SEMICOLON, ";");

	default:
		char c;

		string tmp = "";
		tmp += first;
		if (!(isalpha(first) || isdigit(first))) {
			return pair< tokenType, string >(ERROR, "WRONG INPUT");
		}

		else {
			while (1) {
				c = in.get();

				if (!(isdigit(c) || isalpha(c))) {
					nextChar = c;
					break;
				}

				tmp += c;
			}

			returnToken = checkKeyword(tmp);

			if (returnToken == ERROR) {
				returnToken = checkWord(tmp);

				if (returnToken == ERROR) {
					returnToken = checkNum(tmp);
					if (returnToken == ERROR) {
						return pair< tokenType, string >(returnToken, "WRONG WORD");
					}
				}
			}
			return pair< tokenType, string >(returnToken, tmp);
		}


	}
	return pair< tokenType, string>(WSPACE, "NONE");
}

bool chkDuplicate(table_t *t, int num, string s) {
	table_t *tmp;
	for (int i = 0; i <= num / 100; i++) {
		for (int j = 0; j < 100 && i * 100 + j < num; j++) {
			int cnt = i;
			tmp = t + j;
			while (cnt--) {
				tmp = tmp->next;
			}
			if (tmp->name == s) {
				return true;
			}
		}
	}
	return false;
}

string lexer() {
	nextChar = in.get();
	pair< tokenType, string > tmp;
	tokenType tmp2;
	string tokenStr;
	while (nextChar != EOF) {
		tmp = getToken(nextChar);
		tmp2 = tmp.first;
		tokenStr = tmp.second;
		if (tmp2 == ERROR)
			return tokenStr;
		if (tmp2 != WSPACE)
			token.push_back(tmp);
		table_t *tTmp;
		bool isDuplicate;
		int level;
		if (tmp2 == WORD) {
			isDuplicate = chkDuplicate(symTable, symNum, tokenStr);
			if (isDuplicate == false) {
				level = symNum / 100;
				tTmp = &symTable[symNum % 100];
				while (level--) {
					if (tTmp->next == NULL)
						tTmp->next = new table_t;
					tTmp = tTmp->next;
				}

				tTmp->name = tmp.second;
				tTmp->type = WORD_T;
				symNum++;

				tTmp = &table[valNum % 100];
				level = valNum / 100;
				while (level--) {
					if (tTmp->next == NULL)
						tTmp->next = new table_t;
					tTmp = tTmp->next;
				}

				tTmp->name = tmp.second;
				tTmp->type = WORD_T;
				valNum++;
			}
		}
		else if (tmp2 == NUM) {
			isDuplicate = chkDuplicate(table, valNum, tokenStr);
			if (!isDuplicate) {
				level = valNum / 100;
				tTmp = &table[valNum % 100];
				while (level--) {
					if (tTmp->next == NULL) {
						tTmp->next = new table_t;
					}
					tTmp = tTmp->next;
				}
				tTmp->name = tmp.second;
				tTmp->type = NUM_T;
				valNum++;
			}
		}
	}
	token.push_back(pair< tokenType, string >(END, "$"));
	return "ACCEPT";
}

void reform(Node * node) {
	if (node->parent == NULL) {
		//root
		for (auto now : node->children) {
			reform(now);
		}
	}
	else {
		for (auto now : node->children) {
			reform(now);
		}
		switch (node->state) {
		case 0:
			break;
		case 1:
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // B -> ;
				node->id = make_pair(SEMICOLON, ";");
			}
			else if (node->children.size() == 3) { // B -> { S }
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[1];
						node->children[1]->parent = par;
						break;
					}
				}
			}

			break;

		case 2://S
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // S -> T
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[0];
						node->children[0]->parent = par;
						break;
					}
				}
			}
			else if (node->children.size() == 2) { // S -> ST
			   //do nothing
			}

			break;

		case 3://T
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 8) { // T -> if ( C ) then B else B
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[7];//if�� ����
						node->children[7]->parent = par;
						break;
					}
				}
				node->children[7]->children.push_back(node->children[0]);
				node->children[7]->children.push_back(node->children[2]);
				node->children[7]->children.push_back(node->children[5]);
			}
			else if (node->children.size() == 5) { // T -> while ( C ) B
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[4];
						node->children[4]->parent = par;
						break;
					}
				}
				node->children[4]->children.push_back(node->children[0]);
				node->children[4]->children.push_back(node->children[2]);
			}
			else if (node->children.size() == 4) { // W = E ;
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[2];
						node->children[2]->parent = par;
						break;
					}
				}
				node->children[2]->children.push_back(node->children[1]);
				node->children[2]->children.push_back(node->children[3]);
			}
			else if (node->children.size() == 1) { // T -> ;
				node->id = make_pair(SEMICOLON, ";");
			}

			break;

		case 4://C
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 3) { // C -> E > E | C -> E < E
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[1];
						node->children[1]->parent = par;
						break;
					}
				}
				node->children[1]->children.push_back(node->children[0]);
				node->children[1]->children.push_back(node->children[2]);
			}


			break;
		case 5://E
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // E -> F
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[0];
						node->children[0]->parent = par;
						break;
					}
				}

			}
			else if (node->children.size() == 3) { // E -> E + F
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[1];
						node->children[1]->parent = par;
						break;
					}
				}
				node->children[1]->children.push_back(node->children[0]);
				node->children[1]->children.push_back(node->children[2]);

			}

			break;

		case 6://F
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // F -> N | F -> W
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[0];
						node->children[0]->parent = par;
						break;
					}
				}

			}

			break;

		case 7://W
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // W -> word
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[0];
						node->children[0]->parent = par;
						break;
					}
				}

			}

			break;

		case 8://N
			if (node->children.size() == 0) {
				node = NULL;
			}
			else if (node->children.size() == 1) { // N -> num
				Node * par = node->parent;
				for (int i = 0; i < par->children.size(); i++) {
					if (par->children[i] == node) {
						par->children[i] = node->children[0];
						node->children[0]->parent = par;
						break;
					}
				}

			}

			break;

		case 9://P
		   //do nothing   
			break;


		}
	}
}

string parser() {
	bool accept = 0;
	int parsingTable[45][20] = { 0, };
	//shift 0, reduce 100, GOTO 300

	enum tokenType tt;
	tt = WORD;
	parsingTable[0][tt] = 3;

	parsingTable[1][tt = END] = 100;

	parsingTable[2][tt = OPENSPAR] = 4;

	parsingTable[3][tt = OPENSPAR] = 116;
	parsingTable[3][tt = CLOSESPAR] = 116;
	parsingTable[3][tt = ASSIGN] = 116;
	parsingTable[3][SEMICOLON] = 116;
	parsingTable[3][tt = LARGER] = 116;
	parsingTable[3][tt = SMALLER] = 116;
	parsingTable[3][tt = PLUS] = 116;

	parsingTable[4][tt = CLOSESPAR] = 5;

	parsingTable[5][tt = OPENMPAR] = 7;
	parsingTable[5][tt = SEMICOLON] = 8;

	parsingTable[6][tt = END] = 101;

	parsingTable[7][tt = SEMICOLON] = 14;
	parsingTable[7][tt = KEY_IF] = 11;
	parsingTable[7][tt = KEY_WHILE] = 12;
	parsingTable[7][tt = WORD] = 3;

	parsingTable[8][tt = SEMICOLON] = 103;
	parsingTable[8][tt = KEY_IF] = 103;
	parsingTable[8][tt = KEY_WHILE] = 103;
	parsingTable[8][tt = WORD] = 103;
	parsingTable[8][tt = END] = 103;
	parsingTable[8][CLOSEMPAR] = 103;
	parsingTable[8][KEY_ELSE] = 103;

	parsingTable[9][tt = CLOSEMPAR] = 15;
	parsingTable[9][tt = SEMICOLON] = 14;
	parsingTable[9][tt = KEY_IF] = 11;
	parsingTable[9][tt = KEY_WHILE] = 12;
	parsingTable[9][tt = WORD] = 3;

	parsingTable[10][tt = CLOSEMPAR] = 105;
	parsingTable[10][tt = SEMICOLON] = 105;
	parsingTable[10][tt = KEY_IF] = 105;
	parsingTable[10][tt = KEY_WHILE] = 105;
	parsingTable[10][tt = WORD] = 105;

	parsingTable[11][tt = OPENSPAR] = 17;

	parsingTable[12][tt = OPENSPAR] = 18;

	parsingTable[13][tt = ASSIGN] = 19;

	parsingTable[14][tt = CLOSEMPAR] = 109;
	parsingTable[14][tt = SEMICOLON] = 109;
	parsingTable[14][tt = KEY_IF] = 109;
	parsingTable[14][tt = KEY_WHILE] = 109;
	parsingTable[14][tt = WORD] = 109;

	parsingTable[15][tt = CLOSEMPAR] = 102;
	parsingTable[15][tt = SEMICOLON] = 102;
	parsingTable[15][tt = KEY_IF] = 102;
	parsingTable[15][KEY_ELSE] = 102;
	parsingTable[15][KEY_WHILE] = 102;
	parsingTable[15][WORD] = 102;
	parsingTable[15][END] = 102;

	parsingTable[16][tt = CLOSEMPAR] = 104;
	parsingTable[16][tt = SEMICOLON] = 104;
	parsingTable[16][tt = KEY_IF] = 104;
	parsingTable[16][KEY_WHILE] = 104;
	parsingTable[16][WORD] = 104;

	parsingTable[17][WORD] = 3;
	parsingTable[17][NUM] = 25;

	parsingTable[18][WORD] = 3;
	parsingTable[18][NUM] = 25;

	parsingTable[19][WORD] = 3;
	parsingTable[19][NUM] = 25;

	parsingTable[20][CLOSESPAR] = 28;

	parsingTable[21][LARGER] = 29;
	parsingTable[21][SMALLER] = 30;
	parsingTable[21][PLUS] = 31;

	parsingTable[22][CLOSESPAR] = 112;
	parsingTable[22][SEMICOLON] = 112;
	parsingTable[22][LARGER] = 112;
	parsingTable[22][SMALLER] = 112;
	parsingTable[22][PLUS] = 112;

	parsingTable[23][CLOSESPAR] = 114;
	parsingTable[23][SEMICOLON] = 114;
	parsingTable[23][LARGER] = 114;
	parsingTable[23][SMALLER] = 114;
	parsingTable[23][PLUS] = 114;

	parsingTable[24][OPENSPAR] = 115;
	parsingTable[24][CLOSESPAR] = 115;
	parsingTable[24][SEMICOLON] = 115;
	parsingTable[24][LARGER] = 115;
	parsingTable[24][SMALLER] = 115;
	parsingTable[24][PLUS] = 115;

	parsingTable[25][CLOSESPAR] = 117;
	parsingTable[25][SEMICOLON] = 117;
	parsingTable[25][LARGER] = 117;
	parsingTable[25][SMALLER] = 117;
	parsingTable[25][PLUS] = 117;

	parsingTable[26][tt = CLOSESPAR] = 32;

	parsingTable[27][tt = SEMICOLON] = 33;
	parsingTable[27][tt = PLUS] = 31;

	parsingTable[28][tt = KEY_THEN] = 34;

	parsingTable[29][tt = WORD] = 3;
	parsingTable[29][tt = NUM] = 25;

	parsingTable[30][tt = WORD] = 3;
	parsingTable[30][tt = NUM] = 25;

	parsingTable[31][tt = WORD] = 3;
	parsingTable[31][tt = NUM] = 25;

	parsingTable[32][tt = OPENMPAR] = 7;
	parsingTable[32][tt = SEMICOLON] = 8;

	parsingTable[33][tt = CLOSEMPAR] = 108;
	parsingTable[33][tt = SEMICOLON] = 108;
	parsingTable[33][tt = KEY_IF] = 108;
	parsingTable[33][tt = KEY_WHILE] = 108;
	parsingTable[33][tt = WORD] = 108;

	parsingTable[34][tt = OPENMPAR] = 7;
	parsingTable[34][tt = SEMICOLON] = 8;

	parsingTable[35][tt = CLOSESPAR] = 110;
	parsingTable[35][tt = PLUS] = 31;

	parsingTable[36][tt = CLOSESPAR] = 111;
	parsingTable[36][tt = PLUS] = 31;

	parsingTable[37][tt = CLOSESPAR] = 113;
	parsingTable[37][tt = SEMICOLON] = 113;
	parsingTable[37][tt = LARGER] = 113;
	parsingTable[37][tt = SMALLER] = 113;
	parsingTable[37][tt = PLUS] = 113;

	parsingTable[38][tt = CLOSEMPAR] = 107;
	parsingTable[38][tt = SEMICOLON] = 107;
	parsingTable[38][tt = KEY_IF] = 107;
	parsingTable[38][tt = KEY_WHILE] = 107;
	parsingTable[38][tt = WORD] = 107;

	parsingTable[39][tt = KEY_ELSE] = 40;

	parsingTable[40][tt = OPENMPAR] = 7;
	parsingTable[40][tt = SEMICOLON] = 8;

	parsingTable[41][tt = CLOSEMPAR] = 106;
	parsingTable[41][tt = SEMICOLON] = 106;
	parsingTable[41][tt = KEY_IF] = 106;
	parsingTable[41][tt = KEY_WHILE] = 106;
	parsingTable[41][tt = WORD] = 106;




	stack<Node*> symbol;
	stack<int> state;

	Node* endNode = new Node();
	endNode->id = make_pair(END, "$");
	symbol.push(endNode);

	state.push(0);

	for (int inputIndex = 0; inputIndex < token.size();) {
	  //shift
		if (parsingTable[state.top()][token[inputIndex].first] / 100 == 0) {
			if (parsingTable[state.top()][token[inputIndex].first] == 0) {
				if (token[inputIndex].second != "$") return "NOT_ACCEPT";
				else if (state.size() != 1) return "NOT_ACCEPT";
			}
			state.push(parsingTable[state.top()][token[inputIndex].first]);

			Node * newNode = new Node();
			newNode->id = token[inputIndex];
			newNode->state = 0;
			symbol.push(newNode);
			inputIndex++;
		}

		//reduce
		else if (parsingTable[state.top()][token[inputIndex].first] / 100 == 1) {
			Node * newNode = new Node();
			switch (parsingTable[state.top()][token[inputIndex].first] % 100) {
			case(1):
				for (int i = 0; i < 4; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 9;
				symbol.push(newNode);
				if (state.top() == 0) state.push(1);

				break;

			case(2):
				for (int i = 0; i < 3; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 1;
				symbol.push(newNode);
				if (state.top() == 5) state.push(6);
				else if (state.top() == 32) state.push(38);
				else if (state.top() == 34) state.push(39);
				else if (state.top() == 40) state.push(41);

				break;
			case(3):
				newNode->children.push_back(symbol.top());
				symbol.top()->parent = newNode;
				symbol.pop();
				state.pop();
				newNode->state = 1;
				symbol.push(newNode);
				if (state.top() == 5) state.push(6);
				else if (state.top() == 32) state.push(38);
				else if (state.top() == 34) state.push(39);
				else if (state.top() == 40) state.push(41);

				break;

			case(4):
				for (int i = 0; i < 2; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 2;
				symbol.push(newNode);
				if (state.top() == 7) state.push(9);

				break;

			case(5):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 2;
				symbol.push(newNode);
				if (state.top() == 7) state.push(9);

				break;

			case(6):
				for (int i = 0; i < 8; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 3;
				symbol.push(newNode);
				if (state.top() == 7) state.push(10);
				else if (state.top() == 9) state.push(16);

				break;

			case(7):
				for (int i = 0; i < 5; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 3;
				symbol.push(newNode);
				if (state.top() == 7) state.push(10);
				else if (state.top() == 9) state.push(16);

				break;

			case(8):
				for (int i = 0; i < 4; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 3;
				symbol.push(newNode);
				if (state.top() == 7) state.push(10);
				else if (state.top() == 9) state.push(16);

				break;

			case(9):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 3;
				symbol.push(newNode);
				if (state.top() == 7) state.push(10);
				else if (state.top() == 9) state.push(16);

				break;

			case(10):
				for (int i = 0; i < 3; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 4;
				symbol.push(newNode);
				if (state.top() == 17) state.push(20);
				else if (state.top() == 18) state.push(26);

				break;

			case(11):
				for (int i = 0; i < 3; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 4;
				symbol.push(newNode);
				if (state.top() == 17) state.push(20);
				else if (state.top() == 18) state.push(26);

				break;

			case(12):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 5;
				symbol.push(newNode);
				if (state.top() == 17) state.push(21);
				else if (state.top() == 18) state.push(21);
				else if (state.top() == 19) state.push(27);
				else if (state.top() == 29) state.push(35);
				else if (state.top() == 30) state.push(36);

				break;

			case(13):
				for (int i = 0; i < 3; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 5;
				symbol.push(newNode);
				if (state.top() == 17) state.push(21);
				else if (state.top() == 18) state.push(21);
				else if (state.top() == 19) state.push(27);
				else if (state.top() == 29) state.push(35);
				else if (state.top() == 30) state.push(36);

				break;

			case(14):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 6;
				symbol.push(newNode);
				if (state.top() == 17) state.push(22);
				else if (state.top() == 18) state.push(22);
				else if (state.top() == 19) state.push(22);
				else if (state.top() == 29) state.push(22);
				else if (state.top() == 30) state.push(22);
				else if (state.top() == 31) state.push(37);

				break;

			case(15):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 6;
				symbol.push(newNode);
				if (state.top() == 17) state.push(22);
				else if (state.top() == 18) state.push(22);
				else if (state.top() == 19) state.push(22);
				else if (state.top() == 29) state.push(22);
				else if (state.top() == 30) state.push(22);
				else if (state.top() == 31) state.push(37);

				break;

			case(16):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 7;
				symbol.push(newNode);
				if (state.top() == 0) state.push(2);
				else if (state.top() == 7) state.push(13);
				else if (state.top() == 9) state.push(13);
				else if (state.top() == 17) state.push(24);
				else if (state.top() == 18) state.push(24);
				else if (state.top() == 19) state.push(24);
				else if (state.top() == 29) state.push(24);
				else if (state.top() == 30) state.push(24);
				else if (state.top() == 31) state.push(24);

				break;

			case(17):
				for (int i = 0; i < 1; i++) {
					newNode->children.push_back(symbol.top());
					symbol.top()->parent = newNode;
					symbol.pop();
					state.pop();
				}
				newNode->state = 8;
				symbol.push(newNode);
				if (state.top() == 17) state.push(23);
				else if (state.top() == 18) state.push(23);
				else if (state.top() == 19) state.push(23);
				else if (state.top() == 29) state.push(23);
				else if (state.top() == 30) state.push(23);
				else if (state.top() == 31) state.push(23);

				break;
			case(0):
				//accept
				if (token[inputIndex].second == "$") {
					accept = 1;
					root = symbol.top();
					symbol.pop();
					state.pop();
				}
				else {
					return "NOT_ACCEPT";
				}
				break;
			default:
				break;

			}
			//38 while 7
			//41 if 6



			//reduce�ȿ� GOTO���� ���� ó��
		}

	}

	//dfs���
	reform(root);
	if (accept) return "ACCEPT";
	else return "NOT_ACCEPT";
}

string func;

int generation(Node *node) {
	int reg1, reg2, reg3, tmplabel;
	int i, size;

	if (node->state == 0) {
		if (node->id.second == "IF") {
			tmplabel = labeln;
			labeln = labeln + 2;
			reg1 = generation(node->children[2]);
			code << "\tJUMPT Reg#" << reg1 << " label " << tmplabel << endl;
			code << "\tJUMPF Reg#" << reg1 << " label " << tmplabel + 1 << endl;
			code << "label " << tmplabel << ":" << endl;
			reg2 = generation(node->children[1]);
			code << "label " << tmplabel + 1 << ":" << endl;
			reg3 = generation(node->children[0]);
		}
		else if (node->id.second == "WHILE") {
			tmplabel = labeln;
			labeln = labeln + 2;
			code << "label " << tmplabel << ":" << endl;
			reg1 = generation(node->children[1]);
			code << "\tJUMPF Reg#" << reg1 << " label " << tmplabel + 1 << endl;
			reg2 = generation(node->children[0]);
			code << "\tJUMPT Reg#" << reg1 << " label " << tmplabel << endl;
			code << "label " << tmplabel + 1 << ":" << endl;
		}
		else if (node->id.second == "=") {
			reg1 = generation(node->children[1]);
			reg2 = generation(node->children[0]);
			code << "\tMV Reg#" << reg1 << ", Reg#" << reg2 << endl;
			return reg1;
		}
		else if (node->id.second == "<") {
			reg1 = generation(node->children[1]);
			reg2 = generation(node->children[0]);
			reg3 = regn;////////////////// size+1 of word symbol table ////////////////
			regn++;
			code << "\tLT Reg#" << reg3 << ", Reg#" << reg1 << ", Reg#" << reg2 << endl;
			return reg3;
		}
		else if (node->id.second == ">") {
			reg1 = generation(node->children[1]);
			reg2 = generation(node->children[0]);
			reg3 = regn;////////////////// size+1 of word symbol table ////////////////
			regn++;
			code << "\tLT Reg#" << reg3 << ", Reg#" << reg2 << ", Reg#" << reg1 << endl;
			return reg3;
		}
		else if (node->id.second == "+") {
			reg1 = generation(node->children[1]);
			reg2 = generation(node->children[0]);
			reg3 = regn;
			regn++;
			code << "\tADD Reg#" << reg3 << ", Reg#" << reg1 << ", Reg#" << reg2 << endl;
			return reg3;
		}
		else if (node->id.second == ";") {
		}
		else {
			for (i = 0; i < valNum; i++) {
				if (node->id.second == table[i].name) {
					if (table[i].type == NUM_T) {
						code << "\tLD Reg#" << i << ", " << table[i].name << endl;
						return i;
					}
					else {
						return i;
					}
				}
			}
		}
	}
	else if (node->state == 9) {
		code << "BEGIN " << node->children[3]->id.second << endl;
		func = node->children[3]->id.second;
		generation(node->children[0]);
	}
	else {
		size = node->children.size();
		for (i = size - 1; i >= 0; i--) {
			generation(node->children[i]);
		}
	}
}



int main(int argc, char **argv) {
	string err, tmp = "";
	tmp += argv[1];
	in.open(tmp);
	
	err = lexer();
	if (err != "ACCEPT") {
		cout << "Error: " << err << endl;
		exit(1);
	}

	

	err = parser();
	if (err != "ACCEPT") {
		cout << "Syntax error" << endl;
		exit(1);
	}
	regn = valNum;

	code.open(tmp + ".code");
	symbol2.open(tmp + ".symbol");


	printSymTable();

	generation(root);
	code << "END " << func << endl;
	code << "Used Register: " << regn-1 <<endl;

	return 0;
}


