/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_spirit_H
#define __CELL_spirit_H

#define BOOST_SPIRIT_SINGLE_GRAMMAR_INSTANCE

#if defined(_DEBUG) && defined(DEBUG_PARSER)
	#define BOOST_SPIRIT_DEBUG
#endif

// Includes all needed Spirit headers

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_utility.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_exceptions.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>

namespace spirit_classic = boost::spirit::classic;

//#define skip_node_d discard_node_d
#define skip_node_d no_node_d

namespace chaos { namespace cell {

typedef spirit_classic::position_iterator<std::string::iterator> ParseIterator;
typedef ParseIterator::position_t ParsePosition;
typedef spirit_classic::tree_parse_info<ParseIterator, spirit_classic::node_iter_data_factory<> > TreeParseResult;
typedef spirit_classic::tree_match<ParseIterator, spirit_classic::node_iter_data_factory<> > TreeMatch;
//typedef TreeMatch::tree_iterator TreeIterator;
typedef TreeMatch::const_tree_iterator TreeIterator;
typedef spirit_classic::error_status<> ErrorStatus;

}} // chaos::cell

#endif // __CELL_spirit_H

