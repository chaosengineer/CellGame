/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "cell_compiler.h"
#include "ast_dumper.h"
#include "string_utils.h"
#include "ir_generator.h"

#include <fstream>

namespace chaos { namespace cell {

using namespace std;

int SyntaxErrorHandler::nErrors = 0;

void dumpAST(const TreeParseResult& parseResult)
{
	cout << "\n--------------------------------------------------------------------------------\n";
	cout << "Dumping AST:\n\n";
	ASTDumper() << parseResult.trees;
	cout << endl;
}

///////////////////////////////////////////////////////////////////////////////
// CellCompiler implementation

CellCompiler::CellCompiler()
{}

CellCompiler::~CellCompiler()
{}

void CellCompiler::run(llvm::Module* module, const std::string& filePath, const std::string& functionName)
{
	if (!module)
		CellError::raise("null module");

	_ast.clear();
	
	processUnit(filePath);

	if (!SyntaxErrorHandler::hasErrors())
	{
		if (module)
		{
			IRGenerator irGenerator(*module, functionName);
			irGenerator.traverse(_ast);
#ifdef _DEBUG
			module->dump();
#endif
		}
	}
}

bool CellCompiler::processUnit(const std::string& unitPath)
{
	string path = unitPath;

	cout << "Processing: " << path << endl;

	ifstream in(path.c_str());
	if (!in)
        return false;

	in.unsetf(ios::skipws); // turn off white space skipping on the stream
	
	typedef istream_iterator<string::value_type> InputIterator;
	
	InputIterator first(in);
	InputIterator last;
	string        source;
	
	copy(first, last, back_inserter(source));

	ParseIterator begin(source.begin(), source.end(), path);
	ParseIterator end;

	static CellGrammar cellGrammar;
	static SkipGrammar skipGrammar;

	auto result = ast_parse<node_iter_data_factory<>, ParseIterator, CellGrammar, SkipGrammar>(begin, end, cellGrammar, skipGrammar);
	
	if (!result.full)
	{
		auto it = result.stop;
		string text(result.stop, safe_advance(it, end, 50));
		return false;
	}

#if 0//_DEBUG
		dumpAST(result);
#endif

	auto unitAST = new ASTTree;
	unitAST->build(result.trees.begin(), path);

	if (SyntaxErrorHandler::hasErrors())
		return false;

	_ast.addChild(unitAST);
	return true;
}

}} // chaos::cell

