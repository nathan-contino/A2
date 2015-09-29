/*
 * 
 * Ryan Lee (rlee35) & Nathan Contino (ncontino)
 * CSC 254 -- Fall 2015
 * A2: Syntax Error Recovery
 * 
 */


/* Simple ad-hoc scanner for the calculator language. */
#include <iostream>
#include "scan.h"

//global string to pass the token image to the parser
std::string token_image;

//syntax error message handler
void syntax_error() {
    std::cout << "ERROR: Invalid token syntax, \"" << token_image << "\" found.  Deleted invalid token & continued parse\n";
}

//raw scanner, attempts to read a token in
token scan_raw() {
	using namespace std;

 	token_image = "";// reset the token image
	static char c = ' ';// next available char; extra (int) width accommodates EOF 

	//skip whitespace
	while (isspace(c) && !cin.eof()) {
		cin >> noskipws >> c;
	}
	//detect if at end of file, return eof token if we are
	if (cin.eof())
		return t_eof;

	//if the first character is a letter, read until invalid character
	else if (isalpha(c)) {
		do {
			token_image += c;
			cin >> noskipws >> c;
		} while ((isalpha(c) || isdigit(c) || c == '_') && !cin.eof());

		//match to terminal words
		if (!token_image.compare("read")) return t_read;
		else if (!token_image.compare("write")) return t_write;
		else if (!token_image.compare("if")) return t_if;
		else if (!token_image.compare("while")) return t_while;
		else if (!token_image.compare("end")) return t_end;
		else return t_id;//if not a terminal, assume it is an id
	}
	//if the first character is a number, read until invalid character
	else if (isdigit(c)) {
		do {
			token_image += c;
			cin >> noskipws >> c;
		} while (isdigit(c) && !cin.eof());
		//return literal token
		return t_literal;
	}
	//otherwise, look for operators based off the first character.  Manually set token image and return t_error tokens when invalid tokens seen
	else switch (c) {
		//if (cin.eof()) cases handle cases where cin would read the same character twice giving a valid character for "=$$" thinking it was "=="
		case ':':
            cin >> noskipws >> c;
			if (cin.eof()) {
				token_image = ": ";
				return t_error;
            }
			else if (c != '=') {
				token_image = ":";
				token_image += c;
				return t_error;
			} else {
				token_image = ":=";
				cin >> noskipws >> c;
				return t_gets;
			}
		case '!':
            cin >> noskipws >> c;
			if (cin.eof()) {
				token_image = "! ";
				return t_error;
            }
			else if (c != '=') {
				token_image = "!";
				token_image += c;
				return t_error;
			} else {
				token_image = "!=";
				cin >> noskipws >> c;
				return t_notequals;
			}
			break;
		case '=':
            cin >> noskipws >> c;
            if (cin.eof()) {
				token_image = "= ";
				return t_error;
            }
			else if (c != '=') {
				token_image = "=";
				token_image += c;
				return t_error;
			} else {
				token_image = "==";
				cin >> noskipws >> c;
				return t_equals;
			}
			break;
		case '>':
            cin >> noskipws >> c;
			if (c != '=') {
				if (c == ' ' || cin.eof()) {
					token_image = ">";
					return t_greater;
				}
				token_image = ">";
				token_image += c;
				return t_error;
			} else {
				token_image = ">=";
				cin >> noskipws >> c;
				return t_greaterequals;
			}
			break;
		case '<':
            cin >> noskipws >> c;
			if (c != '=') {
				if (c == ' ' || cin.eof()) {
					token_image = "<";
					return t_less;
				}
				token_image = "<";
				token_image += c;
				return t_error;
			} else {
				token_image = "<=";
				cin >> noskipws >> c;
				return t_lessequals;
			}
			break;
		case '+': cin >> noskipws >> c; token_image = "+"; return t_add;
		case '-': cin >> noskipws >> c; token_image = "-"; return t_sub;
		case '*': cin >> noskipws >> c; token_image = "*"; return t_mul;
		case '/': cin >> noskipws >> c; token_image = "/"; return t_div;
		case '(': cin >> noskipws >> c; token_image = "("; return t_lparen;
		case ')': cin >> noskipws >> c; token_image = ")"; return t_rparen;
		default:
			token_image = c;
			return t_error;
	}
}

//scanner wrapper method which ensures a valid token
token scan() {
	token t = scan_raw();

	//if the raw scanner returned a t_error token, write an error message and scan again for a valid token
	if(t == t_error) {
		syntax_error();
		return scan();
	}

	return t;
}
