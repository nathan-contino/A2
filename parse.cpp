/*
 * 
 * Ryan Lee (rlee35) & Nathan Contino (ncontino)
 * CSC 254 -- Fall 2015
 * A2: Syntax Error Recovery
 * 
 */


/****** INCLUDES ******/
#include <iostream>
#include "scan.h"



/****** HARD CODED DATA ******/
//hard coded token names and literal string representations of enumerated tokens
const char* name[] = {"IGNORE", "read", "write", "id", "literal", "gets",
                        "add", "sub", "mul", "div", "lparen", "rparen", "eof",
                        "if", "while", "end", "equals", "notequals", "less",
                        "greater", "lessequals", "greaterequals", "error"};
                        //"IGNORE" and "error" added to handle added NULL_TOKEN and t_error tokens

const char* token_string[] = {"NULL_TOKEN", "t_read", "t_write", "t_id", "t_literal", "t_gets",
                        "t_add", "t_sub", "t_mul", "t_div", "t_lparen", "t_rparen", "t_eof",
                        "t_if", "t_while", "t_end", "t_equals", "t_notequals", "t_less",
                        "t_greater", "t_lessequals", "t_greaterequals", "t_error"};
                        //"NULL_TOKEN" and "error" added to handle added NULL_TOKEN and t_error tokens

//hard coded special token sets
static token only_eof[] = {t_eof, NULL_TOKEN};
static token end_and_eof[] = {t_end, t_eof, NULL_TOKEN};
static token rparen_and_eof[] = {t_rparen, t_eof, NULL_TOKEN};

//hard coded first sets of all non-terminal characters
static token first_stmt_list[] = {t_id, t_read, t_write, t_if, t_while, NULL_TOKEN};
static token first_stmt[] = {t_id, t_read, t_write, t_if, t_while, NULL_TOKEN};
static token first_cmpr[] = {t_lparen, t_id, t_literal, NULL_TOKEN};
static token first_expr[] = {t_lparen, t_id, t_literal, NULL_TOKEN};
static token first_term[] = {t_lparen, t_id, t_literal, NULL_TOKEN};
static token first_term_tail[] = {t_add, t_sub, NULL_TOKEN};
static token first_factor[] = {t_lparen, t_id, t_literal, NULL_TOKEN};
static token first_factor_tail[] = {t_mul, t_div, NULL_TOKEN}; 
static token first_rel_op[] = {t_equals, t_notequals, t_greater, t_less, t_greaterequals, t_lessequals, NULL_TOKEN};
static token first_add_op[] = {t_add, t_sub, NULL_TOKEN};
static token first_mul_op[] = {t_mul, t_div, NULL_TOKEN};



/****** FUNCTION PROTOTYPES ******/
//error handlers and messengers
void eof_error(std::string method_name);//handles cases where EOF is seen early
void parse_error(std::string method_name);//handles cases where the next token doesn't match and must be deleted
void match_error(std::string inserted_token);//handles cases when match fails and insertion necessary

//helper methods
int find(token t, token list[]);//looks for token t in the given list
token* concat(token list1[], token list2[]);//concatenate list1 and list2 together
std::string match(token expected);//attempts to match the given token with the next token from scanner, deals wtih errors

//parse methods for corresponding non-terminal characters
std::string program();
std::string stmt_list(token cs_follow[]);
std::string stmt(token cs_follow[]);
std::string expr(token cs_follow[]);
std::string cmpr(token cs_follow[]);
std::string term_tail(std::string Term, token cs_follow[]);
std::string term(token cs_follow[]);
std::string factor_tail(std::string Factor, token cs_follow[]);
std::string factor(token cs_follow[]);
std::string rel_op();
std::string add_op();
std::string mul_op();


/****** GLOBALS ******/
//global token input_token to hold the token last scanned in
static token input_token;



/************ IMPLEMENTATION ************/

/****** ERRORS ******/
void eof_error (token expected, std::string expected_image, std::string method_name) {
    //print respective error messages based on where the eof token was seen
    //at end of E1 in C -> E1 ro E2, before ro
    if(!method_name.compare("REL_OP")) {
        std::cout << "EOF ERROR: No relative operator found after EXPR1 within CMPR. Missing operator causes a fatal error in "  + method_name + "\n"; 
        std::exit(1);
    }
    //beginning of E2 after ro in C-> E1 ro E2
    else if(!method_name.compare("CMPR")) {
        std::cout << "EOF ERROR: Found t_eof after " << token_string[expected] << ". Dangling \"" + expected_image + "\" operator causes a fatal error in " + method_name + "\n"; 
        std::exit(1);
    }
    else if(!method_name.compare("FACTOR") || !method_name.compare("EXPR") || !method_name.compare("TERM")) {
        std::cout << "EOF ERROR: Found t_eof after a parse_error.  Unable to parse a valid " + method_name + ", a fatal error\n"; 
        std::exit(1);
    }
    // else if(!method_name.compare("EXPR")) {
    //     std::cout << "ERROR: Found t_eof after a parse_error.  Unable to find a valid " + method_name + ", a fatal error in " + method_name + "\n"; 
    //     exit(1);
    // }
    // else if()
    //otherwise, before T after ao in TT -> ao T TT OR before F after mo in FT -> mo F FT
    std::cout << "EOF ERROR: Found t_eof after " << token_string[expected] << ". Deleted dangling \"" + expected_image + "\" operator in " + method_name + " and parse continued\n";
}

void parse_error(std::string method_name) {
    //print respective error message based on if the parse error is in something that has the ability to go to epsilon (just not in this context)
    //non-terminal characters that can go to epsilon
    if(!method_name.compare("STMT_LIST") || !method_name.compare("FACTOR_TAIL") || !method_name.compare("TERM_TAIL"))
        std::cout << "PARSE ERROR: Found " << token_string[input_token] << " token in " + method_name + ". " << token_string[input_token] << " is not in the first-set of " + method_name + " & is not in the context-sensitive follow-set. " << token_string[input_token] << " token deleted & parse continued\n";
    //everything else that can't go to epsilon
    else
        std::cout << "PARSE ERROR: Found " << token_string[input_token] << " token in " + method_name + ". " << token_string[input_token] << " is not in the first-set of " + method_name + " & " + method_name + " cannot go to epsilon. " << token_string[input_token] << " token deleted & parse continued\n";
    //scan in next token, effectively deleting the current token
    input_token = scan();
}

void match_error(std::string inserted_token, std::string inserted_terminal) {
    //print error message with specific information about what was inserted to move forward
    std::cout << "MATCH ERROR: Expected an " + inserted_token + " token, found " << token_string[input_token] << " token instead. Inserted \"" + inserted_terminal + "\" & parse continued\n";
}



/****** HELPER METHODS ******/
//Match and expected token
std::string match(token expected) {
    //create return string
    std::string temp;

    //if match successful, catch any errors that could result from the next token being t_eof and read in the next token
    if (input_token == expected) {
        std::string str(name[input_token]);
        temp = token_image;
        input_token = scan();
        
        if(input_token == t_eof) {
            if(find(expected, first_add_op)) {
                eof_error(expected, temp, "TERM_TAIL");
                return "";
            }
            else if(find(expected, first_mul_op)) {
                eof_error(expected, temp, "FACTOR_TAIL");
                return "";
            }
            else if(find(expected, first_rel_op))
                eof_error(expected, temp, "CMPR");
        }
    }
    //if match fails, insert the expected token
    else {
        switch (expected) {
            //insert id
            case t_id:
                match_error("id", "ID_MISSING");
                temp = "ID_MISSING";
                break;
            //insert end
            case t_end:
                match_error("t_end", "end");
                temp = "";//end character not in syntax tree
                break;
            //insert :=
            case t_gets:
                match_error("t_gets", ":=");
                temp = ":= ";
                break;
            //insert )
            case t_rparen:
                match_error("t_rparen", ")");
                temp = "";//right paren not in syntax tree
                break;
            default:
                temp = "";
        }
    }
    //return whatever was matched or inserted
    return temp;
}

//search the list for token t
int find(token t, token list[]) {
    int i = 0;
    while(list[i])
        if(t == list[i++])
            //token found, return 1 == true == found
            return 1;
    //default return 0 == false == not found
    return 0;
}

//concatenate list1 and list2 to build context sensitive follow sets
token* concat(token list1[], token list2[]) {
    int size1 = 0, size2 = 0, i = 0, j = 0;

    //get length of list1 and list2
    while(list1[size1]) 
        size1++;
    while(list2[size2])
        size2++;
    
    //allocate memory and fill list
    token* net_list = (token *)malloc(sizeof(token) * (size1 + size2 + 1));
    while(i < size1) {
        net_list[i] = list1[i];
        i++;
    }
    while(i < (size1 + size2))
        net_list[i++] = list2[j++];

    //ensure array ends in NULL (0) so while loops will stop properly without segfault
    net_list[i] = NULL_TOKEN;

    //return list
    return net_list;
}



/****** PARSING ******/
//Parse a program
std::string program() {
    std::string P = "(program";//start of the syntax tree
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            P += stmt_list(only_eof);//parse a statement list, that can only be epsilon if EOF is seen
            match(t_eof);//match the end of file token
            P += ")\n";//end of the syntax tree
            break;
        default:
            parse_error("PROGRAM");//read next token
            program();//attempt to parse program again (eventually will hit eof and will return an empty program "(program)")
    }
    return P;
}

//statement list character "SL" non-terminal
std::string stmt_list(token cs_follow[]) {
    std::string SL = " (";
    try {
        switch (input_token) {
            //in first of statement list (== first of statement)
            case t_id:
            case t_read:
            case t_if:
            case t_while:
            case t_write:
                SL += stmt(concat(first_stmt_list, cs_follow));//parse statement
                SL += stmt_list(cs_follow) + ")";//
                return SL;
            default://not in first, throw error
                throw(1);
        }
    }
    catch(int e) {
        while(1) {//catch error, read until token in SL first set or context sensitive follow set
            //if in context sensitive follow set, treat like an epsilon
            if(find(input_token, cs_follow))
                return "";
            
            //if not in first and follow (as checked in the try block) error + read next token
            parse_error("STMT_LIST");

            //if new token in first set, attempt parse statement list again
            if(find(input_token, first_stmt_list))
                return stmt_list(cs_follow);
        }
    }
}

//statement character "S" non-terminal
std::string stmt(token cs_follow[]) {
    std::string S;
    switch (input_token) {
        case t_id://S -> id := E
            S = match(t_id);
            S = match(t_gets) + " " + S + " ";//gets can be inserted
            S += expr(cs_follow);
            return S;
        case t_read://S -> read id
            S = match(t_read) + " ";
            S += match(t_id);//id can be inserted as MISSING_ID
            return S;
        case t_write://S -> write E
            S = match(t_write) + " ";
            S += expr(cs_follow);
            return S;
        case t_if:
        case t_while:
            S = match(input_token) + " ";//match if or while
            S += cmpr(concat(first_stmt_list, end_and_eof));//parse comparison
            S += stmt_list(end_and_eof);//parse statement list with new context sensitive follow set
            match(t_end);//match end, including eof in the context sensitive follow set allows end to be inserted
            return S;
        default://throw error
            throw(1);
            return "";//purely to surpress a warning (will never reach)
    }
}

//comparison character "C" non-terminal
std::string cmpr(token cs_follow[]) {
    std::string C;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                C = expr(concat(first_rel_op, only_eof));//parse expression with newly constructed context sensitive follow set
                C = "(" + rel_op() + C + " ";//parse relative op
                C += expr(cs_follow) + ")";//parse expression with supplied context sensitive follow set
                return C;
            default://throw error
                throw(1);
        }
    }
    catch(int e) {
        while(1) {//if fail, read tokens until end of file or token in first of comparison
            if(input_token == t_eof)
                eof_error(t_error, "", "CMPR");
            parse_error("CMPR");
            if(find(input_token, first_cmpr))
                return cmpr(cs_follow);
        }
    }
}

//expression character "E" non-terminal
std::string expr(token cs_follow[]) {
    std::string E;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                E = term(concat(first_term_tail, cs_follow));//parse term with modified context sensitive follow set
                if(input_token == t_eof)//if at the end of file after term, return the parsed term
                    return E;
                //otherwise parse term_tail
                E = term_tail(E, cs_follow);
                return E;
            default://throw error
                throw(1);
        }
    }
    catch (int e) {
        while(1) {//if fails, delete tokens until token in first set of expr seen or end of files
            if(input_token == t_eof)
                eof_error(t_error, "", "EXPR");
            parse_error("EXPR");
            if(find(input_token, first_expr))
                return expr(cs_follow);
        }
    }
}

//term-tail character "TT" non-terminal
std::string term_tail(std::string Term, token cs_follow[]) {
    std::string TT;
    try {
        switch (input_token) {
            case t_add:
            case t_sub:
                TT = "(" + add_op() + Term + " ";//parse add op
                if(input_token == t_eof)//if at the end of file after add_op, return the passed term
                    return Term;
                //otherwise, parse term and term_tail
                TT += term(concat(first_term_tail, cs_follow)) + ")";//parse term with modified context sensitive follow set
                TT = term_tail(TT, cs_follow);
                return TT;
            default://throw error
                throw(1);
        }
    }
    catch (int e) {
        while(1) {//if fails, delete tokens until token in first set of term_tail is seen or find token in the context sensitive follow set
            if(find(input_token, cs_follow))
                return Term;

            parse_error("TERM_TAIL");

            if(find(input_token, first_term_tail))
                return term_tail(Term, cs_follow);
        }
    }
}

//term character "T" non-terminal
std::string term(token cs_follow[]) {
    std::string T;
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                T = factor(concat(first_factor_tail, cs_follow));//parse factor and modify context sensitive follow set
                if(input_token == t_eof)//if at the end of file, return just the factor
                    return T;
                //otherwise, parse factor tail and return result
                T = factor_tail(T, cs_follow);
                return T;
            default://throw error
                throw(1);
        }
    }
    catch (int e) {
        while(1) {//if fails, delete tokens until token in first set of term is seen or end of file
            if(input_token == t_eof)
                eof_error(t_error, "", "TERM");
            parse_error("TERM");
            if(find(input_token, first_term))
                return term(cs_follow);
        }
    }
}

//factor-tail character "FT" non-terminal
std::string factor_tail(std::string Factor, token cs_follow[]) {
    std::string FT;
    try {
        switch (input_token) {
            case t_mul:
            case t_div:
                FT = "(" + mul_op() + Factor + " ";//match multiplicative operator
                if(input_token == t_eof)//if now at end of file, dangling multiplicative operator, return passed Factor string
                    return Factor;
                //otherwise parse factor and factor tail with updated context sensitive follow sets
                FT += factor(concat(first_factor_tail, cs_follow)) + ")";
                FT = factor_tail(FT, cs_follow);
                return FT;
            default:
                throw(1);//throw error
        }
    }
    catch (int e) {//error case, delete tokens until token in first of factor_tail found or token in context sensitive follow set found
        while(1) {
            if(find(input_token, cs_follow))
                return Factor;
            parse_error("FACTOR_TAIL");
            if(find(input_token, first_factor_tail))
                return factor_tail(Factor, cs_follow);
        }
    }
}

//factor character "F" non-terminal
std::string factor(token cs_follow[]) {
    std::string F;
    switch (input_token) {
        case t_id :
        case t_literal:
            F = match(input_token);//match an id or literal
            return F;
        case t_lparen://match F -> (E)
            match(t_lparen);
            F = expr(rparen_and_eof);
            match(t_rparen);
            return F;
        default://if fails, delete tokens until token in first set of factor is seen or end of file
            while(1) {
                if(input_token == t_eof)
                    eof_error(t_error, "", "FACTOR");
                parse_error("FACTOR");//error message
                if(find(input_token, first_factor))
                    return factor(cs_follow);
            }
    }
}

//additive operator "ao" non-terminal
std::string add_op() {
    std::string AO;
    switch (input_token) {
        case t_add:
        case t_sub:
            AO = match (input_token) + " ";//match the additive operator and return token image
            return AO;
        default:
            throw(1);//thow back to calling method if invalid token seen
    }
}

//multiplicative operator "mo" non-terminal
std::string mul_op() {
    std::string MO;
    switch (input_token) {
        case t_mul:
        case t_div:
            MO = match(input_token) + " ";//match the multiplicative operator and return token image
            return MO;
        default:
            throw(1);//thow back to calling method if invalid token seen
    }
}

//relative operator "ro" non-terminal
std::string rel_op() {
    std::string RO;
    switch (input_token) {
        case t_equals:
        case t_notequals:
        case t_lessequals:
        case t_greaterequals:
        case t_greater:
        case t_less:
            RO = match(input_token) + " ";//match the relative operator and return the token image
            return RO;
        default:
            //error case, delete tokens until end of file or token in first of rel_op found
            while(1) {
                if(input_token == t_eof)
                    eof_error(t_error, "", "REL_OP");
                parse_error("REL_OP");//error message
                if(find(input_token, first_rel_op))
                    return rel_op();
            }
    }
}

//main method for parsing and syntax tree creation
int main () {
    //scan in first token
    input_token = scan();
    //parse program and create/print syntax tree at the same time
    std::cout << program();

    return 0;
}
