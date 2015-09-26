/* Simple ad-hoc scanner for the calculator language.
*/

#include <iostream>

#include "scan.h"

char token_image[100];

token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    int i = 0;              /* index into token_image */

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "write")) return t_write;
        else if (!strcmp(token_image, "if")) return t_if;
        else if (!strcmp(token_image, "while")) return t_while;
        else if (!strcmp(token_image, "end")) return t_end;
        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                std::cout << stderr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_gets;
            }
            break;
        case '!':
            if ((c = getchar()) != '=') {
                std::cout << stderr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_notequals;
            }
            break;
        case '=':
            if ((c = getchar()) != '=') {
                std::cout << stderr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_equals;
            }
            break;
        case '>':
            if ((c = getchar()) != '=') {
                if (c == ' ') return t_greater;
                std::cout << stderr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_greaterequals;
            }
            break;
        case '<':
            if ((c = getchar()) != '=') {
                if (c == ' ') return t_less;
                std::cout << stderr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_lessequals;
            }
            break;
        case '+': c = getchar(); return t_add;
        case '-': c = getchar(); return t_sub;
        case '*': c = getchar(); return t_mul;
        case '/': c = getchar(); return t_div;
        case '(': c = getchar(); return t_lparen;
        case ')': c = getchar(); return t_rparen;
        default:
            std::cout << "error\n";
            exit(1);
    }
}
