#include <string>
#include <iostream>
#include <map>
#include <cctype>
using namespace std;

enum Token_value {
	NAME,	NUMBER,	END,
	PLUS='+',	MINUS='-',	MUL='*',	DIV='/',
	PRINT=';',	ASSIGN='=',	LP='(',		RP=')'
};
Token_value curr_tok = PRINT;
double number_value;

double expr(bool);
map <string,double> table;
string string_value;
int no_of_errors;

double error(const string& s)
{
	no_of_errors++;
	cerr << "errors: " << s << '\n';
	return 1;
}

//parse expression, update global curr_tok
Token_value get_token(){
	char ch;
	do { //skip whitespace except for '\n'
		if (!cin.get(ch)) return curr_tok = END;
	} while (ch!='\n' && isspace(ch));
	switch (ch){
	case ';':
	case '*':
	case '/':
	case '+':
	case '-':
	case '(':
	case ')':
	case '=':
		return curr_tok = Token_value(ch); //trivial return any of the above symbol
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
		cin.putback(ch); //put the char # back in so cin >> can make
		//a number out of it easily.
		cin >> number_value;
		return curr_tok=NUMBER;
	case'\n':
		return curr_tok=PRINT;
	default:
		if (isalpha(ch)){
			string_value=ch;
			while (cin.get(ch) && isalnum(ch)) string_value.push_back(ch);
			cin.putback(ch);
			return curr_tok=NAME;
		}
		error("bad token");
		return curr_tok=PRINT;
	}

}

//lowest level eval; use curr_tok to determine what to do
//and update answer (left)
double prim(bool get){
	if (get) get_token();

	switch (curr_tok){
	case NUMBER: 
	{ //floating-point constant
		double v = number_value; 
		get_token();
		return v;
	}
	case NAME:
	{	double& v = table[string_value];
		if (get_token() == ASSIGN) v = expr(true);
		return v;
	}
	case MINUS:
		return -prim(true);
	case LP:
	{	double e = expr(true);
		if (curr_tok != RP) return error(") expected");
		get_token(); //eat ')'
		return e;
	}
	default:
		return error("primary expected");
	}
}

// do * and / before + and -; lower level eval
//use curr_tok to determine what to do & update answer
double term(bool get){
	double left = prim(get);
	for (;;){
		switch(curr_tok){
		case MUL:
			left*=prim(true);
			break;
		case DIV:
			if (double d = prim(true)){
				left/=d;
				break;
			}
			return error("Divide by 0!");
		default:
			return left;
		}
	}
}

//highest level eval; use curr_tok to update or print answer
double expr (bool get) //add and subtract
{
	double left = term(get);
	for (;;) //"forever"
	{
		switch (curr_tok){
		case PLUS:
			left+=term(true);
			break;
		case MINUS:
			left-=term(true);
			break;
		default:
			return left;
		}
	}
}



int main(){
	table["pi"] = 3.1415926535897932385;
	table["e"] = 2.7182818284590452354;
	while (cin) {
		get_token();
		if (curr_tok == END) break;
		if (curr_tok == PRINT) continue;
		cout << expr(false) << '\n';

	}

	return no_of_errors;
}