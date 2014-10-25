/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "rules.h"

namespace chaos { namespace cell {

const char* toString(RuleID rid)
{
	switch (rid)
	{
	case RID_IDENTIFIER:                        return "IDENTIFIER";
	case RID_SYSTEM_IDENTIFIER:                 return "SYSTEM_IDENTIFIER";
	case RID_INTEGER_LITERAL_HEX:               return "INTEGER_LITERAL_HEX";
	case RID_INTEGER_LITERAL_OCT:               return "INTEGER_LITERAL_OCT";
	case RID_INTEGER_LITERAL_DEC:               return "INTEGER_LITERAL_DEC";
	case RID_INTEGER_LITERAL:                   return "INTEGER_LITERAL";
	case RID_REAL_LITERAL_1:                    return "REAL_LITERAL_1";
	case RID_REAL_LITERAL_2:                    return "REAL_LITERAL_2";
	case RID_REAL_LITERAL_3:                    return "REAL_LITERAL_3";
	case RID_REAL_LITERAL:                      return "REAL_LITERAL";
	case RID_BOOLEAN_LITERAL:                   return "BOOLEAN_LITERAL";
	case RID_LITERAL:                           return "LITERAL";
	case RID_START_SYMBOL:                      return "start_symbol";
	case RID_TRANSLATION_UNIT:                  return "translation_unit";
	case RID_GLOBAL_DECLARATION:                return "global_declaration";
	case RID_TYPE_SPECIFIER:                    return "type_specifier";
	case RID_TYPE_MODIFIER:                     return "type_modifier";
	case RID_ARGUMENT_LIST:                     return "argument_list";
	case RID_PRIMARY_EXPRESSION:                return "primary_expression";
	case RID_PRIMARY_EXPRESSION_HELPER:         return "primary_expression_helper";
	case RID_PARENTHESIZED_EXPRESSION:          return "parenthesized_expression";
	case RID_MEMBER_ACCESS:                     return "member_access";
	case RID_INVOCATION:                        return "invocation";
	case RID_ELEMENT_ACCESS:                    return "element_access";
	case RID_TYPE_CAST:                         return "type_cast";
	case RID_EXPRESSION_LIST:                   return "expression_list";
	case RID_ACCESS:                            return "access";
	case RID_OBJECT_CREATION_EXPRESSION:        return "object_creation_expression";
	case RID_POSTFIX_EXPRESSION:                return "postfix_expression";
	case RID_UNARY_EXPRESSION:                  return "unary_expression"; 
	case RID_MULTIPLICATIVE_EXPRESSION:         return "multiplicative_expression";
	case RID_ADDITIVE_EXPRESSION:               return "additive_expression";
	case RID_SHIFT_EXPRESSION:                  return "shift_expression";
	case RID_RELATIONAL_EXPRESSION:             return "relational_expression";
	case RID_EQUALITY_EXPRESSION:               return "equality_expression";
	case RID_AND_EXPRESSION:                    return "and_expression";
	case RID_AND_EXPRESSION_HELPER:             return "and_expression_helper";
	case RID_EXCLUSIVE_OR_EXPRESSION:           return "exclusive_or_expression";
	case RID_INCLUSIVE_OR_EXPRESSION:           return "inclusive_or_expression";
	case RID_CONDITIONAL_AND_EXPRESSION:        return "conditional_and_expression";
	case RID_CONDITIONAL_OR_EXPRESSION:         return "conditional_or_expression";
	case RID_CONDITIONAL_EXPRESSION:            return "conditional_expression";
	case RID_ASSIGNMENT:                        return "assignment";
	case RID_ASSIGNMENT_OPERATOR:               return "assignment_operator";
	case RID_EXPRESSION:                        return "expression";
	case RID_ARRAY_CREATION_EXPRESSION:         return "array_creation_expression";
	case RID_STATEMENT:                         return "statement";
	case RID_EMBEDDED_STATEMENT:                return "embedded_statement";
	case RID_BLOCK:                             return "block";
	case RID_STATEMENT_LIST:                    return "statement_list";
	case RID_EMPTY_STATEMENT:                   return "empty_statement";
	case RID_DECLARATION_STATEMENT:             return "declaration_statement";
	case RID_GLOBAL_DECLARATION_STATEMENT:      return "global_declaration_statement";
	case RID_VARIABLE_DECLARATION:              return "variable_declaration";
	case RID_VARIABLE_DECLARATOR:               return "variable_declarator";
	case RID_EXPRESSION_STATEMENT:              return "expression_statement";
	case RID_STATEMENT_EXPRESSION:              return "statement_expression";
	case RID_SELECTION_STATEMENT:               return "selection_statement";
	case RID_IF_STATEMENT:                      return "if_statement";
	case RID_ELSE_STATEMENT:                    return "else_statement";
	case RID_WHILE_STATEMENT:                   return "while_statement";
	case RID_STATEMENT_EXPRESSION_LIST:         return "statement_expression_list";
	case RID_QUIT_STATEMENT:                    return "quit_statement";
	case RID_QUALIFIED_IDENTIFIER:              return "qualified_identifier";
	case RID_TYPE_DECLARATION:                  return "type_declaration";
	case RID_ARRAY_SPECIFIER:                   return "array_specifier";
	default:                                    return "unknown_rule";
	}
}

}} // chaos::cell

