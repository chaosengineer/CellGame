/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_token_parsers_H
#define __CELL_token_parsers_H

#include "types.h"

namespace chaos { namespace cell {

// Token value extractors

void getIntegerLiteral(const std::string& text, std::string& result);
void getRealLiteral(const std::string& text, std::string& result);
void getBoolLiteral(const std::string& text, std::string& result);
long int getIntegerLiteralValue(const std::string& literal, int radix, bool& overflow);
double getRealLiteralValue(const std::string& literal, bool& overflow);
bool getBoolLiteralValue(const std::string& literal);
void getIdentifier(const std::string& text, std::string& result);
TypeSpecifier getTypeSpecifier(const std::string& text);
ExpressionOperator getOperator(const std::string& text);

}} // chaos::cell

#endif // __CELL_token_parsers_H

