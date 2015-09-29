/*
 * 
 * Ryan Lee (rlee35) & Nathan Contino (ncontino)
 * CSC 254 -- Fall 2015
 * A2: Syntax Error Recovery
 * 
 */

/* definitions the scanner shares with the parser */
typedef enum {NULL_TOKEN, t_read, t_write, t_id, t_literal, t_gets,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof,
                t_if, t_while, t_end, t_equals, t_notequals, t_less, t_greater, t_lessequals, t_greaterequals, t_error} token;
//New tokens added, NULL_TOKEN and t_error
//NULL_TOKEN => end of array token with enumerated value 0.  Ensures that when searching through list the loop is not terminated early due to a 0 token value
//t_error => token passed back from scanner when invalid value read

extern std::string token_image;

extern token scan();

