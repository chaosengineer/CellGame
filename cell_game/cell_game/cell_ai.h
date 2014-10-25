/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include <vector>
#include <string>

// Cell Compiler project
#include "..\..\cell_compiler\cell_compiler.h"

namespace llvm {
	class ExecutionEngine;
};

namespace chaos {
namespace cell {

class Cell;
class Vector;

////////////////////////////////////////////////////////////
// ICellAI interface declaration

class ICellAI {

public:
	virtual ~ICellAI();

	virtual void prepare();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const = 0;
};

////////////////////////////////////////////////////////////
// CustomAI declaration

class CustomAI : public ICellAI {
	
public:
	CustomAI(const char *modulePath, const char *scriptPath, const char *uniqueName);
	virtual ~CustomAI();

	virtual void prepare();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;

	void reload();

private:
	static int instanceCount;

	static llvm::Module *baseModule;
	static llvm::ExecutionEngine *executionEngine;

	static chaos::cell::CellCompiler compiler;

	CustomAI(const CustomAI &);
	CustomAI& operator=(const CustomAI &);

	std::string baseModulePath;
	std::string playerScriptPath;
	std::string uniqueScriptName;

	typedef void (*CustomAIFuncion)(Cell *, int, float, Vector *);
	CustomAIFuncion customAI;

	void loadBaseModule();
	void createExecutionEngine();
	void runtimeOptimizeModule();
};

////////////////////////////////////////////////////////////
// DefaultAI declaration

class DefaultAI : public ICellAI {

public:
	virtual ~DefaultAI();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

////////////////////////////////////////////////////////////
// Too stupid. Orbits around fast prey. Accelerates too much and cannot evade larger cells.
// Nickname: Moth.

class Moth : public ICellAI {

public:
	virtual ~Moth();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

////////////////////////////////////////////////////////////
// Never catches anything. Chases prey but never reaches it. Easily scared of larger cells.
// Nickname: Tom.

class Tom : public ICellAI {

public:
	virtual ~Tom();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

////////////////////////////////////////////////////////////
// Doesn't really look for prey, prey finds him. Chases it blindly and dangerously ignores larger cells.
// Nickname: Daredevil.

class Daredevil : public ICellAI {

public:
	virtual ~Daredevil();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

////////////////////////////////////////////////////////////
// Does a neat spin while targeting prey. Easily distracted by other small cells.
// Nickname: Drifter.

class Drifter : public ICellAI {

public:
	virtual ~Drifter();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

////////////////////////////////////////////////////////////
// Chaser

class ChaserAI4 : public ICellAI {

public:
	virtual ~ChaserAI4();

	virtual void calculateForce(std::vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const;
};

}; // namespace cell
}; // namespace chaos