/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_compiler_H
#define __CELL_compiler_H

#include "cell_grammar.h"

namespace llvm {
	class Module;
}

namespace chaos { namespace cell {

//! Loads a module from a .bc file.
llvm::Module* loadModule(const char* modulePath);

//! 
class CellCompiler
{
public:
	CellCompiler();
	~CellCompiler();

	void run(llvm::Module* module, const std::string& filePath, const std::string& functionName);

private:
	void doRun();
	bool processUnit(const std::string& unitPath);

	ASTTree _ast; //! The root node.
};

}} //chaos::cell

#endif // __CELL_compiler_H

