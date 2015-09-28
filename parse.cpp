/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
*/

#include <iostream>
#include "scan.h"

const char* name[] = {"read", "write", "id", "literal", "gets",
                        "add", "sub", "mul", "div", "lparen", "rparen", "eof",
                        "if", "while", "end", "equals", "notequals", "less",
                        "greater", "lessequals", "greaterequals", "error"};

const char* follow_stmt_list[] = {"eof"};
const char* first_stmt_list[] = {"id", "read", "write", "if", "while"};
const char* follow_stmt[] = {"id", "read", "write", "if", "while", "end"};
const char* first_stmt[] = {"id", "read", "write", "if", "while"};
const char* follow_cmpr[] = {"id", "read", "write", "if", "while", "end"};
const char* first_cmpr[] = {"lparen", "id", "lit"};
const char* follow_expr[] = {"lparen", "equals", "notequals", "less", "greater", "lessequals",
                                "greaterequals", "id", "read", "write", 
                                "if", "while", "end", "eof"};
const char* first_expr[] = {"lparen", "id", "lit"};
const char* follow_term[] = {"add", "sub", "id", "read", "write", "if", "while", "eof"};
const char* first_term[] = {"lparen", "id", "lit"};
const char* follow_term_tail[] = {"equals", "notequals", "less", "greater", "lessequals", "greaterequals",
                                    "id", "read", "write", "if", "while", "end", "eof"};
const char* first_term_tail[] = {"add", "sub"};
const char* follow_factor[] = {"lparen", "equals", "notequals", "less", "greater", "lessequals",
                                "greaterequals", "id", "read", "write", "if", "while", "end",
                                "eof", "mul", "div", "add", "sub"};
const char* first_factor[] = {"lparen", "id", "lit"};
const char* follow_factor_tail[] = {"add", "sub", "equals", "notequals", "less", "greater", "lessequals",
                                    "greaterequals", "id", "read", "write", "if", "while", "end", "eof"};
const char* first_factor_tail[] = {"mul", "div"}; 
const char* follow_rel_op[] = {"lparen", "id", "lit"};
const char* first_rel_op[] = {"equals", "notequals", "greater", "less", "greaterequals", "lessequals"};
const char* follow_add_op[] = {"lparen", "id", "lit"};
const char* first_add_op[] = {"add", "sub"};
const char* follow_mul_op[] = {"lparen", "id", "lit"};
const char* first_mul_op[] = {"mul", "div"};

static token input_token;
std::string parse_tree;

void error (std::string method_name) {
    std::cout << "\nerror" << method_name << "\n";
    //std::cout <<  "syntax error in " << method_name << "\n";
    // std::exit(1);
}

void match (token expected) {

    if (input_token == expected) {
        if (input_token == t_id || input_token == t_literal) {
            std::string str(name[input_token]);
            parse_tree +=  "(" + str + " " + token_image + ") ";   
        }
        else {
            std::string str(name[input_token]);
            parse_tree += str + " ";
        }
        input_token = scan();
        if(input_token == t_error) {
            error("errorA");
            std::exit(1);
        }
    }
    else {
        //insert the token
        if (input_token == t_literal)
            std::cout << "(" << name[t_literal] << " 1) ";   
        else if(input_token == t_id)
            std::cout << "IDinsertion";//this part fuck if i know
        else {
            std::string str(name[expected]);
            parse_tree += str + " ";
        }
    }
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
            parse_tree = "(program ";//("predict program --> stmt_list eof\n");
            stmt_list ();
            match (t_eof);
            parse_tree += ")\n ";
            break;
        default: error (__func__);
            input_token = scan();
            program();
    }
}

void stmt_list () {
    try {
        switch (input_token) {
            case t_id:
            case t_read:
            case t_if:
            case t_while:
            case t_write:
                parse_tree +=  "(stmt_list ";//("predict stmt_list --> stmt stmt_list\n");
                stmt ();
                stmt_list ();
                parse_tree += ") ";
                return;
            default:
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(follow_stmt_list), std::end(follow_stmt_list), name[input_token]) != std::end(follow_stmt_list)) {
                parse_tree += "() ";
                return;
            }
            
            input_token = scan();

            if(std::find(std::begin(first_stmt_list), std::end(first_stmt_list), name[input_token]) != std::end(first_stmt_list)) {
                stmt_list();
                return;
            }
        }
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            parse_tree +=  "(stmt ";//("predict stmt --> id gets expr\n");
            match(t_id);
            match(t_gets);
            expr();
            parse_tree += ") ";
            break;
        case t_read:
            parse_tree +=  "(stmt ";//("predict stmt --> read id\n");
            match(t_read);
            match(t_id);
            parse_tree += ") ";
            break;
        case t_write:
            parse_tree +=  "(stmt ";//("predict stmt --> write expr\n");
            match(t_write);
            expr();
            parse_tree += ") ";
            break;
        case t_if:
            parse_tree +=  "(stmt ";//("predict stmt --> if expr\n");
            match(t_if);
            cmpr();
            stmt_list();
            match(t_end);
            parse_tree += ") ";
            break;
        case t_while:
            parse_tree +=  "(stmt ";//("predict stmt --> while expr\n");
            match(t_while);
            cmpr();
            stmt_list(); 
            match(t_end);
            parse_tree += ") ";
            break;
        default:
            throw(1);
    }
}

void cmpr () {
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                parse_tree +=  "(cmpr ";//("predict cmpr --> expr rel_op expr\n");
                expr ();
                rel_op ();
                expr ();
                parse_tree += ") ";
                break;
            default: 
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_cmpr), std::end(first_cmpr), name[input_token]) != std::end(first_cmpr)) {
                cmpr();
                return;
            }
            else
                input_token = scan();
        }
    }
}

void expr() {
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                parse_tree +=  "(expr ";//("predict expr --> term term_tail\n");
                term();
                term_tail();
                parse_tree += ") ";
                break;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(first_expr), std::end(first_expr), name[input_token]) != std::end(first_expr)) {
                expr();
                return;
            }
            else
                input_token = scan();
        }
    }
}

void term_tail() {
    try {
        switch (input_token) {
            case t_add:
            case t_sub:
                parse_tree +=  "(term_tail ";//("predict term_tail --> add_op term term_tail\n");
                add_op();
                term();
                term_tail();
                parse_tree += ") ";
                break;
            default:
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(follow_term_tail), std::end(follow_term_tail), name[input_token]) != std::end(follow_term_tail)) {
                parse_tree += "() ";
                return;
            }
             
            input_token = scan();


            if(std::find(std::begin(first_term_tail), std::end(first_term_tail), name[input_token]) != std::end(first_term_tail)) {
                term_tail();
                return;
            }
        }
    }
}

void term () {
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                parse_tree +=  "(term ";//("predict term --> factor factor_tail\n");
                factor ();
                factor_tail ();
                parse_tree += ") ";
                break;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(first_term), std::end(first_term), name[input_token]) != std::end(first_term)) {
                term();
                return;
            }
            else
                input_token = scan();
        }
    }
}

void factor_tail () {
    try {
        switch (input_token) {
            case t_mul:
            case t_div:
                parse_tree +=  "(factor_tail ";//("predict factor_tail --> mul_op factor factor_tail\n");
                mul_op();
                factor();
                factor_tail();
                parse_tree += ") ";
                break;
            default:
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(follow_factor_tail), std::end(follow_factor_tail), name[input_token]) != std::end(follow_factor_tail)) {
                parse_tree += "() ";
                return;
            }

            input_token = scan();

            if(std::find(std::begin(first_factor_tail), std::end(first_factor_tail), name[input_token]) != std::end(first_factor_tail)) {
                factor_tail();
                return;
            }
        }
    }
}

void factor() {
    try {
        switch (input_token) {
            case t_id :
                parse_tree +=  "(factor ";//("predict factor --> id\n");
                match (t_id);
                parse_tree += ") ";
                break;
            case t_literal:
                parse_tree +=  "(factor ";//("predict factor --> literal\n");
                match (t_literal);
                parse_tree += ") ";
                break;
            case t_lparen:
                parse_tree +=  "(factor ";//("predict factor --> lparen expr rparen\n");
                match (t_lparen);
                expr ();
                match (t_rparen);
                parse_tree += ") ";
                break;
            default: 
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_factor), std::end(first_factor), name[input_token]) != std::end(first_factor)) {
                factor();
                return;
            }
            else
                input_token = scan();
        }
    }
}

void add_op () {
    switch (input_token) {
        case t_add:
            parse_tree +=  "(add_op ";//("predict add_op --> add\n");
            match (t_add);
            parse_tree += ") ";
            break;
        case t_sub:
            parse_tree +=  "(add_op ";//("predict add_op --> sub\n");
            match (t_sub);
            parse_tree += ") ";
            break;
        default:
            throw(1);
    }
}

void mul_op () {
    switch (input_token) {
        case t_mul:
            parse_tree +=  "(mul_op ";//("predict mul_op --> mul\n");
            match (t_mul);
            parse_tree += ") ";
            break;
        case t_div:
            parse_tree +=  "(mul_op ";//("predict mul_op --> div\n");
            match (t_div);
            parse_tree += ") ";
            break;
        default: 
            throw(1);
    }
}

void rel_op () {
    try {
        switch (input_token) {
            case t_equals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> equals\n");
                match (t_equals);
                parse_tree += ") ";
                break;
            case t_notequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> notequals\n");
                match (t_notequals);
                parse_tree += ") ";
                break;
            case t_lessequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> lessequals\n");
                match (t_lessequals);
                parse_tree += ") ";
                break;
            case t_greaterequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> greaterequals\n");
                match (t_greaterequals);
                parse_tree += ") ";
                break;
            case t_greater:
                parse_tree +=  "(rel_op ";//("predict rel_op --> greater\n");
                match (t_greater);
                parse_tree += ") ";
                break;
            case t_less:
                parse_tree +=  "(rel_op ";//("predict rel_op --> less\n");
                match (t_less);
                parse_tree += ") ";
                break;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_rel_op), std::end(first_rel_op), name[input_token]) != std::end(first_rel_op)) {
                rel_op();
                return;
            }
            else
                input_token = scan();
        }
    }
}

int main () {
    input_token = scan();
    if(input_token == t_error)
        error("errorB");
    program();
    std::cout << parse_tree;
    
    return 0;
}
