/* definitions the scanner shares with the parser */

typedef enum {t_read, t_write, t_id, t_literal, t_gets,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof,
                t_if, t_while, t_end, t_equals, t_notequals, t_less, t_greater, t_lessequals, t_greaterequals} token;

extern char token_image[100];

extern token scan();
