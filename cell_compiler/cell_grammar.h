/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_grammar_H
#define __CELL_grammar_H

#include "spirit.h"
#include "tokens.h"
#include "rules.h"
#include "skip_grammar.h"
#include "ast_tree.h"
#include "error.h"

#include <iostream>
#include <string>

using namespace spirit_classic;

namespace chaos { namespace cell {

typedef parser_error<CellError, ParseIterator> ParserError;

//! Spirit-compatible error handler used at the parse phase.
struct SyntaxErrorHandler
{
	template<class ScannerT>
	ErrorStatus operator()(const ScannerT& scan, const ParserError& e) const
	{
		auto pos = scan.first.get_position();
		error(e.descriptor, pos.file, pos.line);
		return ErrorStatus::accept;
	}

	//! Prints and count syntax errors.
	static void error(const CellError& e, const std::string& filename, int line)
	{
		++nErrors;
		printf("%s (%d): %s\n", filename.c_str(), line, e.what());
	}

	static bool hasErrors() { return nErrors != 0; }

	static int nErrors; //! The number of all  errors encountered during the parse phase
};

//! The CeLL grammar rules.
//! Nodes marked as [intermediate] do not have AST representation.
struct CellGrammar : public grammar<CellGrammar>
{
	template<typename ScannerT>
	struct definition
	{
		definition(const CellGrammar& self);

		const rule<ScannerT, parser_tag<RID_START_SYMBOL> >&
		start() const { return start_symbol; }

		symbols<> user_type;

	// Keywords
		symbols<> keywords;
		
	// Identifiers
		rule<ScannerT, parser_tag<RID_IDENTIFIER> >                 IDENTIFIER;
		rule<ScannerT, parser_tag<RID_SYSTEM_IDENTIFIER> >          SYSTEM_IDENTIFIER;
																    
	// Literals													    
		rule<ScannerT, parser_tag<RID_INTEGER_LITERAL_HEX> >        INTEGER_LITERAL_HEX;
		rule<ScannerT, parser_tag<RID_INTEGER_LITERAL_OCT> >        INTEGER_LITERAL_OCT;
		rule<ScannerT, parser_tag<RID_INTEGER_LITERAL_DEC> >        INTEGER_LITERAL_DEC;
		rule<ScannerT, parser_tag<RID_INTEGER_LITERAL> >            INTEGER_LITERAL;               // [intermediate]
																    
		rule<ScannerT, parser_tag<RID_REAL_LITERAL_1> >             REAL_LITERAL_1;
		rule<ScannerT, parser_tag<RID_REAL_LITERAL_2> >             REAL_LITERAL_2;
		rule<ScannerT, parser_tag<RID_REAL_LITERAL_3> >             REAL_LITERAL_3;
		rule<ScannerT, parser_tag<RID_REAL_LITERAL> >               REAL_LITERAL;
																    
		rule<ScannerT, parser_tag<RID_BOOLEAN_LITERAL> >            BOOLEAN_LITERAL;
																    
		rule<ScannerT, parser_tag<RID_LITERAL> >                    LITERAL;
																    
	// Basic concepts											    
		rule<ScannerT, parser_tag<RID_START_SYMBOL> >               start_symbol;
		rule<ScannerT, parser_tag<RID_TRANSLATION_UNIT> >           translation_unit;
																    
	// Types													    
		rule<ScannerT, parser_tag<RID_TYPE_SPECIFIER> >             type_specifier;
		rule<ScannerT, parser_tag<RID_TYPE_MODIFIER> >              type_modifier;

	// Expressions
		rule<ScannerT, parser_tag<RID_ARGUMENT_LIST> >              argument_list;
		rule<ScannerT, parser_tag<RID_PRIMARY_EXPRESSION> >         primary_expression;
		rule<ScannerT, parser_tag<RID_PRIMARY_EXPRESSION_HELPER> >  primary_expression_helper;
		rule<ScannerT, parser_tag<RID_PARENTHESIZED_EXPRESSION> >   parenthesized_expression;
		rule<ScannerT, parser_tag<RID_MEMBER_ACCESS> >              member_access;
		rule<ScannerT, parser_tag<RID_INVOCATION> >                 invocation;
		rule<ScannerT, parser_tag<RID_ELEMENT_ACCESS> >             element_access;
		rule<ScannerT, parser_tag<RID_EXPRESSION_LIST> >            expression_list;
		rule<ScannerT, parser_tag<RID_OBJECT_CREATION_EXPRESSION> > object_creation_expression;
		rule<ScannerT, parser_tag<RID_POSTFIX_EXPRESSION> >         postfix_expression;
		rule<ScannerT, parser_tag<RID_UNARY_EXPRESSION> >           unary_expression;
		rule<ScannerT, parser_tag<RID_MULTIPLICATIVE_EXPRESSION> >  multiplicative_expression;
		rule<ScannerT, parser_tag<RID_ADDITIVE_EXPRESSION> >        additive_expression;
		rule<ScannerT, parser_tag<RID_SHIFT_EXPRESSION> >           shift_expression;
		rule<ScannerT, parser_tag<RID_RELATIONAL_EXPRESSION> >      relational_expression;
		rule<ScannerT, parser_tag<RID_EQUALITY_EXPRESSION> >        equality_expression;
		rule<ScannerT, parser_tag<RID_AND_EXPRESSION> >             and_expression;
		rule<ScannerT, parser_tag<RID_EXCLUSIVE_OR_EXPRESSION> >    exclusive_or_expression;
		rule<ScannerT, parser_tag<RID_INCLUSIVE_OR_EXPRESSION> >    inclusive_or_expression;
		rule<ScannerT, parser_tag<RID_CONDITIONAL_AND_EXPRESSION> > conditional_and_expression;
		rule<ScannerT, parser_tag<RID_CONDITIONAL_OR_EXPRESSION> >  conditional_or_expression;
		rule<ScannerT, parser_tag<RID_CONDITIONAL_EXPRESSION> >     conditional_expression;
		rule<ScannerT, parser_tag<RID_ASSIGNMENT> >                 assignment;
		rule<ScannerT, parser_tag<RID_ASSIGNMENT_OPERATOR> >        assignment_operator;           // [intermediate]
		rule<ScannerT, parser_tag<RID_EXPRESSION> >                 expression;
		rule<ScannerT, parser_tag<RID_ARRAY_CREATION_EXPRESSION> >  array_creation_expression;

	// Statements
		rule<ScannerT, parser_tag<RID_STATEMENT> >                  statement;                     // [intermediate]
		rule<ScannerT, parser_tag<RID_EMBEDDED_STATEMENT> >         embedded_statement;            // [intermediate]
		rule<ScannerT, parser_tag<RID_BLOCK> >                      block;
		rule<ScannerT, parser_tag<RID_STATEMENT_LIST> >             statement_list;
		rule<ScannerT, parser_tag<RID_EMPTY_STATEMENT> >            empty_statement;
		rule<ScannerT, parser_tag<RID_DECLARATION_STATEMENT> >      declaration_statement;         // [intermediate]
		rule<ScannerT, parser_tag<RID_VARIABLE_DECLARATION> >       variable_declaration;
		rule<ScannerT, parser_tag<RID_VARIABLE_DECLARATOR> >        variable_declarator;
		rule<ScannerT, parser_tag<RID_EXPRESSION_STATEMENT> >       expression_statement;
		rule<ScannerT, parser_tag<RID_STATEMENT_EXPRESSION> >       statement_expression;          // [intermediate]
		rule<ScannerT, parser_tag<RID_IF_STATEMENT> >               if_statement;
		rule<ScannerT, parser_tag<RID_ELSE_STATEMENT> >             else_statement;
		rule<ScannerT, parser_tag<RID_WHILE_STATEMENT> >            while_statement;
		rule<ScannerT, parser_tag<RID_STATEMENT_EXPRESSION_LIST> >  statement_expression_list;
		rule<ScannerT, parser_tag<RID_QUIT_STATEMENT> >             quit_statement;
		rule<ScannerT, parser_tag<RID_QUALIFIED_IDENTIFIER> >       qualified_identifier;
		rule<ScannerT, parser_tag<RID_ARRAY_SPECIFIER> >            array_specifier;
	};
};

////////////////////////////////////////////////////////////////////////////////

template<class ScannerT>
CellGrammar::definition<ScannerT>::definition(const CellGrammar& self)
{
	guard<CellError>   guard;
	SyntaxErrorHandler eh;

	assertion<CellError>
		expectLBrace( "{ expected" ),
		expectRBrace( "} expected" ),
		expectSemicolon( "; expected" ),
		expectColon( ": expected" ),
		expectLParen( "( expected" ),
		expectRParen(") expected"),
		expectExpression( "expression expected" ),
		expectConstant( "constant expected" ),
		expectLBracket( "[ expected" ),
		expectRBracket( "] expected" ),
		expectLT( "< expected" ),
		expectGT( "> expected" )
		;

// Keywords

	keywords 
		=
		ELSE_T,
		FALSE_T,
		IF_T, INT_T,
		QUIT_T,
		REAL_T,
		TRUE_T,
		WHILE_T
		;

////////////////////////////////////////////////////////////////////////////////
// Lexical grammar

// Punctuation and single character operators

	chlit<>
		SEMICOLON_SY         = SEMICOLON_T,
		COMMA_SY             = COMMA_T,
		COLON_SY             = COLON_T,
		EQ_SY                = EQ_T,
		LPAREN_SY            = LPAREN_T,
		RPAREN_SY            = RPAREN_T,
		DOT_SY               = DOT_T,
		AND_SY               = AND_T,
		BANG_SY              = BANG_T,
		TILDE_SY             = TILDE_T,
		MINUS_SY             = MINUS_T,
		PLUS_SY              = PLUS_T,
		STAR_SY              = STAR_T,
		SLASH_SY             = SLASH_T,
		MOD_SY               = MOD_T,
		LT_SY                = LT_T,
		GT_SY                = GT_T,
		XOR_SY               = XOR_T,
		OR_SY                = OR_T,
		QUESTION_SY          = QUESTION_T,
		SYSTEM_PREFIX        = SYSTEM_PREFIX_T,
		LBRACE_SY            = LBRACE_T,
		RBRACE_SY            = RBRACE_T,
		LBRACKET_SY          = LBRACKET_T,
		RBRACKET_SY          = RBRACKET_T
		;

// Multi-character operators

	strlit<>
		PLUSEQ_SY            = PLUSEQ_T,
		MINUSEQ_SY           = MINUSEQ_T,
		STAREQ_SY            = STAREQ_T,
		SLASHEQ_SY           = SLASHEQ_T,
		MODEQ_SY             = MODEQ_T,
		XOREQ_SY             = XOREQ_T,
		ANDEQ_SY             = ANDEQ_T,
		OREQ_SY              = OREQ_T,
		LTLT_SY              = LTLT_T,
		GTGT_SY              = GTGT_T,
		LTLTEQ_SY            = LTLTEQ_T,
		GTGTEQ_SY            = GTGTEQ_T,
		EQEQ_SY              = EQEQ_T,
		NOTEQ_SY             = NOTEQ_T,
		LTEQ_SY              = LTEQ_T,
		GTEQ_SY              = GTEQ_T,
		ANDAND_SY            = ANDAND_T,
		OROR_SY              = OROR_T,
		PLUSPLUS_SY          = PLUSPLUS_T,
		MINUSMINUS_SY        = MINUSMINUS_T
		;

// Keywords

	strlit<>
		QUIT_SY              = QUIT_T,
		ELSE_SY              = ELSE_T,
		FALSE_SY             = FALSE_T,
		IF_SY                = IF_T,
		TRUE_SY              = TRUE_T,
		WHILE_SY             = WHILE_T
		;

// Types

	strlit<>
		REAL_SY              = REAL_T,
		INT_SY               = INT_T,
		VECTOR_SY            = VECTOR_T,
		GLOBAL_SY            = GLOBAL_T
		;
	
// Identifiers

	IDENTIFIER
		=
		token_node_d
		[
			lexeme_d
			[
				((alpha_p | '_') >> *(alnum_p | '_'))
				- ((keywords | user_type) >> anychar_p - (alnum_p | '_'))
			]
		]
		;

	SYSTEM_IDENTIFIER
		= token_node_d[ lexeme_d[ skip_node_d[ SYSTEM_PREFIX ] >> IDENTIFIER ] ]
		;

// Integer literals

	INTEGER_LITERAL_HEX
		= token_node_d[ lexeme_d[ '0' >> as_lower_d['x'] >> +xdigit_p ] ]
		;

	INTEGER_LITERAL_OCT
		= token_node_d[ lexeme_d[ '0' >> +chset_p("0-7") ] ]
		;

	INTEGER_LITERAL_DEC
		= token_node_d[ lexeme_d[ +digit_p ] ]
		;

	INTEGER_LITERAL 
		= INTEGER_LITERAL_HEX
		| INTEGER_LITERAL_OCT
		| INTEGER_LITERAL_DEC
		;

// Real literals

	REAL_LITERAL_1    // 12345[eE][+-]123[fF]?
		=
		lexeme_d
		[
				+digit_p
			>>	chset_p("eE")
			>>	!chset_p("+-")
			>>	+digit_p
			>>	!as_lower_d['f']
		]
		;

	REAL_LITERAL_2    // .123([[eE][+-]123)?[fF]?
		=
		lexeme_d
		[
				*digit_p
			>>	'.'
			>>	+digit_p
			>>	!(
						chset_p("eE")
					>>	!chset_p("+-")
					>>	+digit_p
				)
			>>	!as_lower_d['f']
		]
		;

	REAL_LITERAL_3    // 12345.([[eE][+-]123)?[fF]?
		=
		lexeme_d
		[
				+digit_p
			>>	'.'
			>>	*digit_p
			>>	!(
						chset_p("eE")
					>>	!chset_p("+-")
					>>	+digit_p
				)
			>>	!as_lower_d['f']
		]
		;

	REAL_LITERAL
		=
		token_node_d
		[
				REAL_LITERAL_1
			|	REAL_LITERAL_2
			|	REAL_LITERAL_3
		]
		;

	BOOLEAN_LITERAL
		=
		token_node_d
		[
				FALSE_SY
			|	TRUE_SY
		]
		;

	LITERAL
		=
		longest_d
		[
				INTEGER_LITERAL
			|	REAL_LITERAL
			|	BOOLEAN_LITERAL
		]
		;

////////////////////////////////////////////////////////////////////////////////
// Syntactic grammar

// The start symbol

	start_symbol
		= translation_unit >> !end_p
		;

// Basic concepts

	translation_unit
		=
		(	
			+block
		)
		| epsilon_p
		;

// Types

	type_specifier
		= INT_SY
		| REAL_SY
		| VECTOR_SY
		;

	type_modifier
		= GLOBAL_SY
		;

// Expressions

	argument_list
		= expression % skip_node_d[ COMMA_SY ]
		;

	primary_expression
		=	
			LITERAL
		|	(
				parenthesized_expression
			|	array_creation_expression
			|	object_creation_expression
			|	invocation
			|	(qualified_identifier | SYSTEM_IDENTIFIER)
			)
			>> primary_expression_helper
		;

	invocation
		=	(qualified_identifier)
		>>	root_node_d[ LPAREN_SY ]
		>>	!argument_list
		>>	skip_node_d[ RPAREN_SY ]
		;

	primary_expression_helper
		=	(
				member_access
			|	element_access
			)
			>> primary_expression_helper
		| epsilon_p
		;

	parenthesized_expression
		=	
		guard
		(
				root_node_d[ LPAREN_SY ]
			>>	expectExpression( expression )
			>>	skip_node_d[ expectRParen( RPAREN_SY ) ]
		)
		[ eh ]
		;
	
	member_access
		= root_node_d[ DOT_SY ] >> qualified_identifier
		;

	element_access
		=
		guard
		(
				root_node_d[ LBRACKET_SY ]
			>>	expression
			>>	skip_node_d[ expectRBracket( RBRACKET_SY ) ]
		)
		[ eh ]
		;

	expression_list
//		= infix_node_d[ expression % COMMA_SY ]
		= expression >> skip_node_d[ COMMA_SY ]
		;

	object_creation_expression
		=
		guard
		(	
				root_node_d[ token_node_d[ type_specifier ] ]
			>>	skip_node_d[ expectLParen( LPAREN_SY ) ]
			>>	!argument_list
			>>	skip_node_d[ expectRParen( RPAREN_SY ) ]
		)
		[ eh ]
		;

	array_creation_expression
		=
		guard
		(
				root_node_d[ token_node_d[ type_specifier ] ]
			>>	array_specifier
			>>	skip_node_d[ expectLParen( LPAREN_SY ) ]
			>>	!argument_list//expectArrayArgumentList( argument_list )
			>>	skip_node_d[ expectRParen( RPAREN_SY ) ]
		)
		[ eh ]
		;

	postfix_expression
		=	(primary_expression | qualified_identifier | SYSTEM_IDENTIFIER)
		>>	*( root_node_d[ PLUSPLUS_SY | MINUSMINUS_SY ] )
		;
	  
	unary_expression
		= postfix_expression
		| root_node_d[ BANG_SY       ] >> unary_expression
		| root_node_d[ TILDE_SY      ] >> unary_expression
		| root_node_d[ PLUSPLUS_SY   ] >> unary_expression
		| root_node_d[ MINUSMINUS_SY ] >> unary_expression
		| root_node_d[ PLUS_SY       ] >> unary_expression
		| root_node_d[ MINUS_SY      ] >> unary_expression
		;

	multiplicative_expression
		=	unary_expression
		>>	*( root_node_d[ STAR_SY | SLASH_SY | MOD_SY ] >> unary_expression )
		;

	additive_expression
		=	multiplicative_expression
		>>	*( root_node_d[ PLUS_SY | MINUS_SY ] >> multiplicative_expression )
		;

	shift_expression
		=	additive_expression
		>>	*( root_node_d[ LTLT_SY | GTGT_SY ] >> additive_expression )
		;

	relational_expression
		=	shift_expression
		>>	*( root_node_d[ LTEQ_SY | GTEQ_SY| LT_SY | GT_SY ] >> shift_expression )
		;

	equality_expression
		=	relational_expression
		>>	*( root_node_d[ EQEQ_SY | NOTEQ_SY ] >> relational_expression )
		;

	and_expression
		=	equality_expression
		>>	*( root_node_d[ AND_SY ] >> equality_expression )
		;

	exclusive_or_expression
		=	and_expression
		>>	*( root_node_d[ XOR_SY ] >> and_expression )
		;

	inclusive_or_expression
		=	exclusive_or_expression
		>>	*( root_node_d[ OR_SY ] >> exclusive_or_expression )
		;

	conditional_and_expression
		=	inclusive_or_expression
		>>	*( root_node_d[ ANDAND_SY ] >> inclusive_or_expression )
		;

	conditional_or_expression
		=	conditional_and_expression
		>>	*( root_node_d[ OROR_SY ] >> conditional_and_expression )
		;

	conditional_expression
		= 
		guard
		(	
				conditional_or_expression
			>>	!( root_node_d[ QUESTION_SY ] >> expression >> skip_node_d[ expectColon( COLON_SY ) ] >> expression )
		)
		[ eh ]
		;

	assignment
		= unary_expression >> root_node_d[ token_node_d[ assignment_operator ] ] >> expression
		;

	assignment_operator
		= EQ_SY | PLUSEQ_SY | MINUSEQ_SY | STAREQ_SY | SLASHEQ_SY | MODEQ_SY
		| XOREQ_SY | ANDEQ_SY | OREQ_SY | GTGTEQ_SY | LTLTEQ_SY 
		;

	expression
		= conditional_expression
		| assignment
		;

// Statements
	
	statement
		= declaration_statement
		| embedded_statement
		;

	embedded_statement
		= block
		| empty_statement
		| expression_statement
		| if_statement
		| while_statement
		| quit_statement
		;

	block
		=
		guard
		(
				root_node_d[ LBRACE_SY ]
			>>	statement_list
			>>	skip_node_d[ expectRBrace( RBRACE_SY ) ]
		)
		[ eh ]
		;

	statement_list
		= *statement
		;
	
	empty_statement
		= skip_node_d[ SEMICOLON_SY ]
		;

	declaration_statement
		=
		guard
		(
			variable_declaration >> skip_node_d[ expectSemicolon( SEMICOLON_SY ) ]
		)
		[ eh ]
		;

	variable_declaration
		=	!type_modifier
		>>	root_node_d[ token_node_d[ type_specifier ] ]
		>>	!array_specifier	
		>>	variable_declarator
		;

	variable_declarator
		=
		root_node_d[ IDENTIFIER ]
		 ;

	array_specifier
		=
		guard
		(
				root_node_d[ LBRACKET_SY ]
			>>	expectConstant( INTEGER_LITERAL )
			>>	skip_node_d[ expectRBracket( RBRACKET_SY ) ]
		)
		[ eh ]
		;

	expression_statement
		=
		guard
		(
				statement_expression
			>>	root_node_d[ expectSemicolon( SEMICOLON_SY ) ]
		)
		[ eh ]
		;

	statement_expression
		= assignment
		| conditional_expression
		| unary_expression
		;

	if_statement
		= 
		guard
		(		
				root_node_d[ IF_SY  ]
			>>	skip_node_d[ expectLParen( LPAREN_SY ) ]
			>>	expectExpression( expression )
			>>	skip_node_d[ expectRParen( RPAREN_SY ) ]
			>>	embedded_statement
			>>	else_statement
		)
		[ eh ]
		;

	else_statement
		= root_node_d[ ELSE_SY ] >> embedded_statement
		| epsilon_p
		;

	while_statement
		=
		guard
		(
				root_node_d[ WHILE_SY ]
			>>	skip_node_d[ expectLParen( LPAREN_SY ) ]
			>>	expectExpression( expression )
			>>	skip_node_d[ expectRParen( RPAREN_SY ) ]
			>>	embedded_statement
		)
		[ eh ]
		;

	statement_expression_list
		= statement_expression % skip_node_d[ COMMA_SY ]
		;

	quit_statement
		=
		guard
		(
				root_node_d[ QUIT_SY ]
			>>	skip_node_d[ expectSemicolon( SEMICOLON_SY ) ]
		)
		[ eh ]
		;

	qualified_identifier
		= root_node_d[ IDENTIFIER ]
		;
}

}} // chaos::cell

#endif // __CELL_grammar_H

