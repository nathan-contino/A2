/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
*/

#include <iostream>

#include "scan.h"
#include "string"

using namespace std;

const string names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof"
                        "if", "while", "end", "equals", "notequals", "less", "greater", "lessequals", "greaterequals"};

static token input_token;

void error (std::string method_name) {
    std::cout <<  "syntax error in " << method_name << "\n";
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        //std::cout << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            std::cout << token_image;
        //std::cout <<  ("\n");
        input_token = scan ();
        cout << input_token;
    }
    else error (__func__);
}

void program ();
void stmt_list ();
void stmt ();
void expr ();
void cmpr ();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void rel_op ();
void add_op ();
void mul_op ();

void program () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            std::cout <<  "(program ";//("predict program --> stmt_list eof\n");
            stmt_list ();
            std::cout << ")\n ";
            match (t_eof);
            break;
        default: error (__func__);
    }
}

void stmt_list () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_if:
        case t_while:
        case t_write:
            std::cout <<  "(stmt_list ";//("predict stmt_list --> stmt stmt_list\n");
            stmt ();
            stmt_list ();
            std::cout << ") ";
            break;
        case t_end:
        case t_eof:
            std::cout <<  " () ";//("predict stmt_list --> epsilon\n");
            break;          /*  epsilon production */
        default: error (__func__);
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            std::cout <<  "(stmt ";//("predict stmt --> id gets expr\n");
            match (t_id);
            match (t_gets);
            expr ();
            std::cout << ") ";
            break;
        case t_read:
            std::cout <<  "(stmt ";//("predict stmt --> read id\n");
            match (t_read);
            match (t_id);
            std::cout << ") ";
            break;
        case t_write:
            std::cout <<  "(stmt ";//("predict stmt --> write expr\n");
            match (t_write);
            expr ();
            std::cout << ") ";
            break;
        case t_if:
            std::cout <<  "(stmt ";//("predict stmt --> if expr\n");
            match (t_if);
            cmpr ();
            stmt_list ();
            match (t_end);
            std::cout << ") ";
            break;
        case t_while:
            std::cout <<  "(stmt ";//("predict stmt --> while expr\n");
            match (t_while);
            cmpr ();
            stmt_list (); 
            match (t_end);
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void cmpr () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            std::cout <<  "(cmpr ";//("predict cmpr --> expr rel_op expr\n");
            expr ();
            rel_op ();
            expr ();
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void expr () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            std::cout <<  "(expr ";//("predict expr --> term term_tail\n");
            term ();
            term_tail ();
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void term_tail () {
    switch (input_token) {
        case t_add:
        case t_sub:
            std::cout <<  "(term_tail ";//("predict term_tail --> add_op term term_tail\n");
            add_op ();
            term ();
            term_tail ();
            std::cout << ") ";
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_end:
        case t_equals:
        case t_notequals:
        case t_less:
        case t_greater:
        case t_lessequals:
        case t_greaterequals:
        case t_eof:
            std::cout <<  "()";//("predict term_tail --> epsilon\n");
            break;          /*  epsilon production */
        default: error (__func__);
    }
}

void term () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            std::cout <<  "(term ";//("predict term --> factor factor_tail\n");
            factor ();
            factor_tail ();
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void factor_tail () {
    switch (input_token) {
        case t_mul:
        case t_div:
            std::cout <<  "(factor_tail ";//("predict factor_tail --> mul_op factor factor_tail\n");
            mul_op ();
            factor ();
            factor_tail ();
            std::cout << ") ";
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_if:
        case t_while:
        case t_end:
        case t_write:
        case t_equals:
        case t_notequals:
        case t_less:
        case t_greater:
        case t_lessequals:
        case t_greaterequals:
        case t_eof:
            std::cout <<  "()";//("predict factor_tail --> epsilon\n");
            break;          /*  epsilon production */
        default: error (__func__);
    }
}

void factor () {
    switch (input_token) {
        case t_id :
            std::cout <<  "(factor ";//("predict factor --> id\n");
            match (t_id);
            std::cout << ") ";
            break;
        case t_literal:
            std::cout <<  "(factor ";//("predict factor --> literal\n");
            match (t_literal);
            std::cout << ") ";
            break;
        case t_lparen:
            std::cout <<  "(factor ";//("predict factor --> lparen expr rparen\n");
            match (t_lparen);
            expr ();
            match (t_rparen);
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void add_op () {
    switch (input_token) {
        case t_add:
            std::cout <<  "(add_op ";//("predict add_op --> add\n");
            match (t_add);
            std::cout << ") ";
            break;
        case t_sub:
            std::cout <<  "(add_op ";//("predict add_op --> sub\n");
            match (t_sub);
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void mul_op () {
    switch (input_token) {
        case t_mul:
            std::cout <<  "(mul_op ";//("predict mul_op --> mul\n");
            match (t_mul);
            std::cout << ") ";
            break;
        case t_div:
            std::cout <<  "(mul_op ";//("predict mul_op --> div\n");
            match (t_div);
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

void rel_op () {
    switch (input_token) {
        case t_equals:
            std::cout <<  "(rel_op ";//("predict rel_op --> equals\n");
            match (t_equals);
            std::cout << ") ";
            break;
        case t_notequals:
            std::cout <<  "(rel_op ";//("predict rel_op --> notequals\n");
            match (t_notequals);
            std::cout << ") ";
            break;
        case t_lessequals:
            std::cout <<  "(rel_op ";//("predict rel_op --> lessequals\n");
            match (t_lessequals);
            std::cout << ") ";
            break;
        case t_greaterequals:
            std::cout <<  "(rel_op ";//("predict rel_op --> greaterequals\n");
            match (t_greaterequals);
            std::cout << ") ";
            break;
        case t_greater:
            std::cout <<  "(rel_op ";//("predict rel_op --> greater\n");
            match (t_greater);
            std::cout << ") ";
            break;
        case t_less:
            std::cout <<  "(rel_op ";//("predict rel_op --> less\n");
            match (t_less);
            std::cout << ") ";
            break;
        default: error (__func__);
    }
}

int main () {
    input_token = scan ();
    program ();
}
