/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_tokens_H
#define __CELL_tokens_H

// Comments

#define COMMENT_LINE_T      "//"
#define COMMENT_START_T     "/*"
#define COMMENT_END_T       "*/"

// Punctuation and single character operators

#define SEMICOLON_T         ';'
#define COMMA_T             ','
#define COLON_T             ':'
#define EQ_T                '='
#define LPAREN_T            '('
#define RPAREN_T            ')'
#define DOT_T               '.'
#define AND_T               '&'
#define BANG_T              '!'
#define TILDE_T             '~'
#define MINUS_T             '-'
#define PLUS_T              '+'
#define STAR_T              '*'
#define SLASH_T             '/'
#define MOD_T               '%'
#define LT_T                '<'
#define GT_T                '>'
#define XOR_T               '^'
#define OR_T                '|'
#define QUESTION_T          '?'
#define SYSTEM_PREFIX_T     '#'
#define LBRACE_T            '{'
#define RBRACE_T            '}'
#define LBRACKET_T          '['
#define RBRACKET_T          ']'

// Multi-character operators

#define PLUSEQ_T            "+="
#define MINUSEQ_T           "-="
#define STAREQ_T            "*="
#define SLASHEQ_T           "/="
#define MODEQ_T             "%="
#define XOREQ_T             "^="
#define ANDEQ_T             "&="
#define OREQ_T              "|=" 
#define LTLT_T              "<<"
#define GTGT_T              ">>"
#define LTLTEQ_T            "<<="
#define GTGTEQ_T            ">>="
#define EQEQ_T              "=="
#define NOTEQ_T             "!="
#define LTEQ_T              "<="
#define GTEQ_T              ">=" 
#define ANDAND_T            "&&"
#define OROR_T              "||"
#define PLUSPLUS_T          "++"
#define MINUSMINUS_T        "--"

// VRSL keywords

#define ELSE_T              "else"
#define FALSE_T             "false"
#define GLOBAL_T            "global"
#define IF_T                "if"
#define TRUE_T              "true"
#define WHILE_T             "while"
#define QUIT_T              "quit"

// VRSL built-in types

#define REAL_T              "real"
#define INT_T               "int"
#define MATRIX_T            "mat"
#define VECTOR_T            "vec"
#define BOOL_T              "bool"

#endif // __CELL_tokens_H

