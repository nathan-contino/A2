/* Simple ad-hoc scanner for the calculator language.
*/

#include <iostream>
#include <string>
#include "scan.h"


token scan() {
	//
	using namespace std;
 
    string token_image;
	static char c = ' ';
		/* next available char; extra (int) width accommodates EOF */
	int i = 0;			  /* index into token_image */

	/* skip white space */
	while (isspace(c) && !cin.eof()) {
		cin >> noskipws >> c;
	}

	if (cin.eof()) {
		return t_eof;
	}
	else if (isalpha(c)) {
		do {
			token_image += c;
			cin >> noskipws >> c;
		} while ((isalpha(c) || isdigit(c) || c == '_') && !cin.eof());
				//token_image[i] = '\0';
		if (!token_image.compare("read")) return t_read;
		else if (!token_image.compare("write")) return t_write;
		else if (!token_image.compare("if")) return t_if;
		else if (!token_image.compare("while")) return t_while;
		else if (!token_image.compare("end")) return t_end;
		else return t_id;
	}
	else if (isdigit(c)) {
		do {
			token_image += c;
			cin >> noskipws >> c;
		} while (isdigit(c));
		//token_image[i] = '\0';
		return t_literal;
	} else switch (c) {
		case ':':
            cin >> noskipws >> c;
			if (c != '=') {
				cout << cerr << "error\n";
				return t_error;
			} else {
				cin >> noskipws >> c;;
				return t_gets;
			}
			break;
		case '!':
            cin >> noskipws >> c;
			if (c != '=') {
				cout << cerr << "error\n";
				return t_error;
			} else {
				cin >> noskipws >> c;
				return t_notequals;
			}
			break;
		case '=':
            cin >> noskipws >> c;
			if (c != '=') {
				cout << cerr << "error\n";
				return t_error;
			} else {
				cin >> noskipws >> c;
				return t_equals;
			}
			break;
		case '>':
            cin >> noskipws >> c;
			if (c != '=') {
				if (c == ' ') return t_greater;
				cout << cerr << "error\n";
				return t_error;
			} else {
				cin >> noskipws >> c;
				return t_greaterequals;
			}
			break;
		case '<':
            cin >> noskipws >> c;
			if (c != '=') {
				if (c == ' ') return t_less;
				cout << cerr << "error\n";
				return t_error;
			} else {
				cin >> noskipws >> c;
				return t_lessequals;
			}
			break;
		case '+': cin >> noskipws >> c; return t_add;
		case '-': cin >> noskipws >> c; return t_sub;
		case '*': cin >> noskipws >> c; return t_mul;
		case '/': cin >> noskipws >> c; return t_div;
		case '(': cin >> noskipws >> c; return t_lparen;
		case ')': cin >> noskipws >> c; return t_rparen;
		default:
			cout << "error\n";
			return t_error;
	}
}
