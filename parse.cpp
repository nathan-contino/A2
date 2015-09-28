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

token only_eof[] = {t_eof};
token only_end[] = {t_end};
token follow_stmt_list[] = {t_end, t_eof};
token first_stmt_list[] = {t_id, t_read, t_write, t_if, t_while};
token follow_stmt[] = {t_id, t_read, t_write, t_if, t_while, t_end};
token first_stmt[] = {t_id, t_read, t_write, t_if, t_while};
token follow_cmpr[] = {t_id, t_read, t_write, t_if, t_while, t_end};
token first_cmpr[] = {t_lparen, t_id, t_literal};
token follow_expr[] = {t_lparen, t_equals, t_notequals, t_less, t_greater, t_lessequals,
                                t_greaterequals, t_id, t_read, t_write, 
                                t_if, t_while, t_end, t_eof};
token first_expr[] = {t_lparen, t_id, t_literal};
token follow_term[] = {t_add, t_sub, t_id, t_read, t_write, t_if, t_while, t_eof};
token first_term[] = {t_lparen, t_id, t_literal};
token follow_term_tail[] = {t_equals, t_notequals, t_less, t_greater, t_lessequals, t_greaterequals,
                                    t_id, t_read, t_write, t_if, t_while, t_end, t_eof};
token first_term_tail[] = {t_add, t_sub};
token follow_factor[] = {t_lparen, t_equals, t_notequals, t_less, t_greater, t_lessequals,
                                t_greaterequals, t_id, t_read, t_write, t_if, t_while, t_end,
                                t_eof, t_mul, t_div, t_add, t_sub};
token first_factor[] = {t_lparen, t_id, t_literal};
token follow_factor_tail[] = {t_add, t_sub, t_equals, t_notequals, t_less, t_greater, t_lessequals,
                                    t_greaterequals, t_id, t_read, t_write, t_if, t_while, t_end, t_eof};
token first_factor_tail[] = {t_mul, t_div}; 
token follow_rel_op[] = {t_lparen, t_id, t_literal};
token first_rel_op[] = {t_equals, t_notequals, t_greater, t_less, t_greaterequals, t_lessequals};
token follow_add_op[] = {t_lparen, t_id, t_literal};
token first_add_op[] = {t_add, t_sub};
token follow_mul_op[] = {t_lparen, t_id, t_literal};
token first_mul_op[] = {t_mul, t_div};

static token input_token;
std::string parse_tree;

void error (std::string method_name) {
    std::cout << "\nerror" << method_name << "\n";
    //std::cout <<  "syntax error in " << method_name << "\n";
    // std::exit(1);
}

std::string match (token expected) {
    std::string temp;
    if (input_token == expected) {
        if (input_token == t_id || input_token == t_literal) {
            std::string str(name[input_token]);
            parse_tree +=  "(" + str + " " + token_image + ") ";
            temp = token_image;
        }
        else {
            std::string str(name[input_token]);
            parse_tree += str + " ";
            temp = token_image;
        }
        input_token = scan();
        if(input_token == t_error) {
            error("errorA");
            std::exit(1);
        }
        return temp;
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
            temp = str + " ";
            return temp;
        }
        return "";
    }
}

std::string program();
std::string stmt_list(token cs_follow[]);
std::string stmt();
std::string expr();
std::string cmpr();
std::string term_tail(std::string Term, token cs_follow[]);
std::string term();
std::string factor_tail(std::string Factor, token cs_follow[]);
std::string factor();
std::string rel_op();
std::string add_op();
std::string mul_op();

int find(token t, token list[]) {
    int i = 0;
    while(list[i]) {
        if(t == list[i])
            return 1;
    }
    return 0;
}

std::string program () {
    std::string P = "(program";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            parse_tree = "(program";//("predict program --> stmt_list eof\n");
            P += stmt_list(only_eof);
            P += match(t_eof);
            P += ")\n";
            parse_tree += ")\n ";
            break;
        default: error (__func__);
            input_token = scan();
            program();
    }
    return P;
}

std::string stmt_list(token cs_follow[]) {
    std::string SL = " (";
    try {
        switch (input_token) {
            //in first
            case t_id:
            case t_read:
            case t_if:
            case t_while:
            case t_write:
                parse_tree +=  "(stmt_list ";//("predict stmt_list --> stmt stmt_list\n");
                SL += stmt();
                SL += stmt_list(cs_follow) + ")";
                parse_tree += ") ";
                return SL;
            //not in first
            default:
                throw(1);
        }
    }
    //catch error
    catch(int e) {
        while(1) {
            //check in follow (can be context sensative)
            if(find(input_token, cs_follow)) {
                parse_tree += "() ";
                return "";
            }
            
            //if not in first and follow (as checked in the try block)
            input_token = scan();

            //check if in first
            if(std::find(std::begin(first_stmt_list), std::end(first_stmt_list), input_token) != std::end(first_stmt_list)) {
                return stmt_list(cs_follow);
            }
        }
    }
}

std::string stmt () {
    std::string S;
    switch (input_token) {
        case t_id:
            parse_tree +=  "(stmt ";//("predict stmt --> id gets expr\n");
            S = match(t_id);
            S = match(t_gets) + " " + S + " ";
            S += expr();
            parse_tree += ") ";
            return S;
        case t_read:
            parse_tree +=  "(stmt ";//("predict stmt --> read id\n");
            S = match(t_read) + " ";
            S += match(t_id);
            parse_tree += ") ";
            return S;
        case t_write:
            parse_tree +=  "(stmt ";//("predict stmt --> write expr\n");
            S = match(t_write) + " ";
            S += expr();
            parse_tree += ") ";
            return S;
        case t_if:
            parse_tree +=  "(stmt ";//("predict stmt --> if expr\n");
            S = match(t_if) + " ";
            S += cmpr();
            S += stmt_list(only_end);
            match(t_end);
            parse_tree += ") ";
            return S;
        case t_while:
            parse_tree +=  "(stmt ";//("predict stmt --> while expr\n");
            S = match(t_while) + " ";
            S += cmpr();
            S += stmt_list(only_end); 
            match(t_end);
            parse_tree += ") ";
            return S;
        default:
            throw(1);
            return "";
    }
}

std::string cmpr() {
    std::string C;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                parse_tree +=  "(cmpr ";//("predict cmpr --> expr rel_op expr\n");
                C = expr();
                C = "(" + rel_op() + C + " ";
                C += expr() + ")";
                parse_tree += ") ";
                return C;
            default: 
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_cmpr), std::end(first_cmpr), input_token) != std::end(first_cmpr)) {
                return cmpr();
            }
            else
                input_token = scan();
        }
    }
}

std::string expr() {
    std::string E;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen://improve
                parse_tree +=  "(expr ";//("predict expr --> term term_tail\n");
                E = term();
                E = term_tail(E);
                parse_tree += ") ";
                return E;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(first_expr), std::end(first_expr), input_token) != std::end(first_expr)) {
                return expr();
            }
            else
                input_token = scan();
        }
    }
}

std::string term_tail(std::string Term, token cs_follow[]) {
    std::string TT;
    try {
        switch (input_token) {
            case t_add:
            case t_sub://improve
                parse_tree +=  "(term_tail ";//("predict term_tail --> add_op term term_tail\n");
                TT = "(" + add_op() + Term + " ";
                TT += term() + ")";
                TT = term_tail(TT);
                parse_tree += ") ";
                return TT;
            default:
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(follow_term_tail), std::end(follow_term_tail), input_token) != std::end(follow_term_tail)) {
                parse_tree += "() ";
                return Term;
            }
             
            input_token = scan();


            if(std::find(std::begin(first_term_tail), std::end(first_term_tail), input_token) != std::end(first_term_tail)) {
                return term_tail(Term);
            }
        }
    }
}

std::string term () {
    std::string T;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                parse_tree +=  "(term ";//("predict term --> factor factor_tail\n");
                T = factor();
                T = factor_tail(T);
                parse_tree += ") ";
                return T;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(first_term), std::end(first_term), input_token) != std::end(first_term)) {
                return term();
            }
            else
                input_token = scan();
        }
    }
}

std::string factor_tail(std::string Factor, token cs_follow[]) {
    std::string FT;
    try {
        switch (input_token) {
            case t_mul:
            case t_div:
                parse_tree +=  "(factor_tail ";//("predict factor_tail --> mul_op factor factor_tail\n");
                FT = "(" + mul_op() + Factor + " ";
                FT += factor() + ") ";
                FT = factor_tail(FT);
                parse_tree += ") ";
                return FT;
            default:
                throw(1);
        }
    }
    catch (int e) {
        while(1) {
            if(std::find(std::begin(follow_factor_tail), std::end(follow_factor_tail), input_token) != std::end(follow_factor_tail)) {
                parse_tree += "() ";
                return Factor;
            }

            input_token = scan();

            if(std::find(std::begin(first_factor_tail), std::end(first_factor_tail), input_token) != std::end(first_factor_tail)) {
                return factor_tail(Factor);
            }
        }
    }
}

std::string factor() {
    std::string F;//improve
    try {
        switch (input_token) {
            case t_id :
                parse_tree +=  "(factor ";//("predict factor --> id\n");
                F = match(t_id);
                parse_tree += ") ";
                return F;
            case t_literal:
                parse_tree +=  "(factor ";//("predict factor --> literal\n");
                F = match(t_literal);
                parse_tree += ") ";
                return F;
            case t_lparen://improve
                parse_tree +=  "(factor ";//("predict factor --> lparen expr rparen\n");
                match(t_lparen);
                F = expr();
                match(t_rparen);
                parse_tree += ") ";
                return F;
            default: 
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_factor), std::end(first_factor), input_token) != std::end(first_factor)) {
                return factor();
            }
            else
                input_token = scan();
        }
    }
}

std::string add_op() {
    std::string AO;
    switch (input_token) {
        case t_add:
            parse_tree +=  "(add_op ";//("predict add_op --> add\n");
            AO = match (t_add) + " ";
            parse_tree += ") ";
            return AO;
        case t_sub:
            parse_tree +=  "(add_op ";//("predict add_op --> sub\n");
            AO = match (t_sub) + " ";
            parse_tree += ") ";
            return AO;
        default:
            throw(1);
    }
}

std::string mul_op() {
    std::string MO;
    switch (input_token) {
        case t_mul:
            parse_tree +=  "(mul_op ";//("predict mul_op --> mul\n");
            MO = match(t_mul) + " ";
            parse_tree += ") ";
            return MO;
        case t_div:
            parse_tree +=  "(mul_op ";//("predict mul_op --> div\n");
            MO = match(t_div) + " ";
            parse_tree += ") ";
            return MO;
        default: 
            throw(1);
    }
}

std::string rel_op () {
    std::string RO;
    try {
        switch (input_token) {
            case t_equals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> equals\n");
                RO = match(t_equals) + " ";
                parse_tree += ") ";
                return RO;
            case t_notequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> notequals\n");
                RO = match(t_notequals) + " ";
                parse_tree += ") ";
                return RO;
            case t_lessequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> lessequals\n");
                RO = match(t_lessequals) + " ";
                parse_tree += ") ";
                return RO;
            case t_greaterequals:
                parse_tree +=  "(rel_op ";//("predict rel_op --> greaterequals\n");
                RO = match(t_greaterequals) + " ";
                parse_tree += ") ";
                return RO;
            case t_greater:
                parse_tree +=  "(rel_op ";//("predict rel_op --> greater\n");
                RO = match(t_greater) + " ";
                parse_tree += ") ";
                return RO;
            case t_less:
                parse_tree +=  "(rel_op ";//("predict rel_op --> less\n");
                RO = match(t_less) + " ";
                parse_tree += ") ";
                return RO;
            default:
                input_token = scan();
                throw(1);
        }
    }
    catch(int e) {
        while(1) {
            if(std::find(std::begin(first_rel_op), std::end(first_rel_op), input_token) != std::end(first_rel_op)) {
                return rel_op();
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
    std::cout << program() << "\n\n";
    std::cout << parse_tree;

    return 0;
}
