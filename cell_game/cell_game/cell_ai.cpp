/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

// LLVM
#include "llvm\PassManager.h"
#include "llvm\IR\DataLayout.h"
#include "llvm\Support\TargetSelect.h"
#include "llvm\Analysis\Passes.h"
#include "llvm\Analysis\Verifier.h"
#include "llvm\Transforms\IPO.h"
#include "llvm\Transforms\Scalar.h"
#include "llvm\ExecutionEngine\JIT.h"
#include "llvm\ExecutionEngine\ExecutionEngine.h"

// Cell Compiler project
#include "..\..\cell_compiler\ir_generator.h"

// Project headers
#include "cell.h"
#include "cell_ai.h"
#include "math_utils.h"

using namespace std;
using namespace llvm;
using namespace chaos::cell;

////////////////////////////////////////////////////////////
// ICellAI implementation

ICellAI::~ICellAI() {
}

void ICellAI::prepare() {
}

////////////////////////////////////////////////////////////
// CustomAI implementation

int CustomAI::instanceCount = 0;
Module* CustomAI::baseModule = NULL;
ExecutionEngine *CustomAI::executionEngine = NULL;
CellCompiler CustomAI::compiler;

CustomAI::CustomAI(const char *modulePath, const char *scriptPath, const char *uniqueName) 
	: baseModulePath(modulePath)
	, playerScriptPath(scriptPath)
	, uniqueScriptName(uniqueName)
	, customAI(NULL) {
	++instanceCount;
}

CustomAI::~CustomAI() {
	--instanceCount;
	if (instanceCount == 0) {
		delete executionEngine;
		executionEngine = NULL;
		baseModule = NULL;
	}
}

void CustomAI::prepare() {
	// 1. Make sure we have a loaded LLVM module.
	loadBaseModule();

	// 2. Make sure we have an LLVM execution engine.
	createExecutionEngine();

	// 3. Parse and JIT compile the script.
	if (baseModule) {
		// 3.1. Parse the script file and add the function's definition to the base module.
		try {
			compiler.run(baseModule, playerScriptPath, uniqueScriptName);
		} catch (const CellError &e) {
			// There was a problem with the parsing or code generation.
			printf("%s\n", e.what());
			return;
		}

		// 3.2. Now that we have the function's definition in the base module run optimization passes on the whole thing.
		runtimeOptimizeModule();

		// 3.3. Get a pointer to the function's definition in the base module.
		Function *llvmCustomAIFunction = baseModule->getFunction(uniqueScriptName);
		if (llvmCustomAIFunction && executionEngine) {
			// 3.4. JIT compile the retrieved function definition and aquire an invokable C++ pointer to the compiled image.
			customAI = reinterpret_cast<CustomAIFuncion>(executionEngine->getPointerToFunction(llvmCustomAIFunction));
		}
	}
}

void CustomAI::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	// 4. Invoke the custom AI function.
	if (customAI && !cells.empty()) {
		customAI(&(cells[0]), liveCellCount, arenaRadius, &force);
	}
}

void CustomAI::loadBaseModule() {
	// If we already have an LLVM base module do nothing.
	if (!baseModule) {
		// 1.1. We don't have an LLVM base module so load it now.
		baseModule = loadModule(baseModulePath.c_str());
		
		if (baseModule) {
			// 1.2. Verify that everything is OK with the loaded module.
			string errorMessage;
			verifyModule(*baseModule, PrintMessageAction, &errorMessage);
			if (!errorMessage.empty()) {
				// There is a problem with the base module.
				printf("Failed to verify the base module!\n%s\n", errorMessage.c_str());
			}
		}
	}
}

void CustomAI::createExecutionEngine() {
	// If we already have an LLVM execution engine do nothing.
	if (baseModule && !executionEngine) {
		// 2.1. Initialize the native target so we can JIT compile code for it.
		InitializeNativeTarget();

		// 2.2. Create the LLVM execution engine.
		string errorMessage;
		executionEngine = EngineBuilder(baseModule).setEngineKind(EngineKind::JIT).setErrorStr(&errorMessage).create();
		if (!errorMessage.empty()) {
			// There is a problem with the execution engine.
			printf("Failed to create an execution engine!\n%s\n", errorMessage.c_str());
		}
	}
}

void CustomAI::runtimeOptimizeModule() {
	if (baseModule) {
		// Set up the optimizer pipeline.
		PassManager pm;
	
		// Start with registering info about how the target lays out data structures.
		pm.add(new DataLayout(baseModule->getDataLayout()));

		// Provide basic AliasAnalysis support for GVN.
		pm.add(createBasicAliasAnalysisPass());
		// Promote allocas to registers.
		pm.add(createPromoteMemoryToRegisterPass());
		// Do simple "peephole" optimizations and bit-twiddling optimizations.
		pm.add(createInstructionCombiningPass());
		// Reassociate expressions.
		pm.add(createReassociatePass());
		// Eliminate Common SubExpressions.
		pm.add(createGVNPass());
		// Simplify the control flow graph (deleting unreachable blocks, etc).
		pm.add(createCFGSimplificationPass());
		// Constant propagation pass.
		pm.add(createConstantPropagationPass());
		// Dead instruction elimination pass.
		pm.add(createDeadInstEliminationPass());
		// Loop unroll pass.
		pm.add(createLoopUnrollPass());
		// Function inlining pass.
		pm.add(createFunctionInliningPass());

		pm.run(*baseModule);
	}
}

void CustomAI::reload() {
	static int invokeCount = 0;

	stringstream ss;
	ss << uniqueScriptName;
	ss << ++invokeCount;

	uniqueScriptName = ss.str();

	prepare();
}

////////////////////////////////////////////////////////////
// DefaultAI implementation

DefaultAI::~DefaultAI() {
}

void DefaultAI::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	force.x = 0.0f;
	force.y = 0.0f;
}

////////////////////////////////////////////////////////////
// Moth implementation

Moth::~Moth() {
}

void Moth::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	if (liveCellCount < 2) {
		return;
	}

	const Cell &i = cells[0];
	const Cell &closestCell = cells[1];

	Vector directionToClosestCell = closestCell.position - i.position;
	if (closestCell.radius < i.radius) {
		force = directionToClosestCell;
	} else {
		force = -directionToClosestCell;
	}
}

////////////////////////////////////////////////////////////
// Tom implementation

Tom::~Tom() {
}

void Tom::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	if (liveCellCount < 2) {
		return;
	}

	const Cell &i = cells[0];
	const Cell &closestCell = cells[1];

	Vector directionToClosestCell = closestCell.position - i.position;
	Vector currentMovingDirection = i.velocity;

	if (closestCell.radius < i.radius) {
		force = directionToClosestCell - currentMovingDirection;
	} else {
		force = -directionToClosestCell;
	}
}

////////////////////////////////////////////////////////////
// Daredevil implementation

Daredevil::~Daredevil() {
}

void Daredevil::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	if (liveCellCount < 2) {
		return;
	}

	static const float CORRECTION_SIGNIFICANCE_THRESHOLD = 0.2f;

	const Cell &i = cells[0];
	const Cell &closestCell = cells[1];

	Vector directionToClosestCell = closestCell.position - i.position;
	Vector currentMovingDirection = i.velocity;

	if (closestCell.radius < i.radius) {
		force = directionToClosestCell - currentMovingDirection;
		if (force.length() < CORRECTION_SIGNIFICANCE_THRESHOLD) {
			force = directionToClosestCell;
		}
	} else {
		force = -directionToClosestCell;
	}
}

////////////////////////////////////////////////////////////
// Drifter implementation

Drifter::~Drifter() {
}

void Drifter::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	if (liveCellCount < 2) {
		return;
	}

	const Cell &i = cells[0];
	const Cell &closestCell = cells[1];

	Vector directionToClosestCell = closestCell.position - i.position;
	Vector currentMovingDirection = i.velocity;

	if (closestCell.radius < i.radius) {		
		Vector unitDirectionToClosestCell = directionToClosestCell;
		unitDirectionToClosestCell.normalize();
		
		Vector unitCurrentMovingDirection = currentMovingDirection;
		unitCurrentMovingDirection.normalize();
		
		float angleToClosestCell = dot(unitDirectionToClosestCell, unitCurrentMovingDirection);
		
		force = directionToClosestCell - (1.0f - angleToClosestCell) * currentMovingDirection;
	} else {
		force = -directionToClosestCell;
	}
}

////////////////////////////////////////////////////////////
// ChaserAI4 implementation

ChaserAI4::~ChaserAI4() {
}

void ChaserAI4::calculateForce(vector<Cell> &cells, const int liveCellCount, const float arenaRadius, Vector &force) const {
	if (liveCellCount == 0) {
		return;
	}

	const Cell &i = cells[0];
	
	// Find the closest larger and smaller cells
	const Cell *closestSmallerCell = NULL;
	const Cell *closestLargerCell = NULL;
	for (int cellIndex = 1; cellIndex < liveCellCount; ++cellIndex) {
		if (cells[cellIndex].radius < i.radius) {
			if (closestSmallerCell == NULL) {
				closestSmallerCell = &cells[cellIndex];
			}
		} else {
			if (closestLargerCell == NULL) {
				closestLargerCell = &cells[cellIndex];
			}
		}

		if (closestSmallerCell && closestLargerCell) {
			break;
		}
	}

	Vector directionToSmallerCell;
	float distanceToSmallerCell = 0.0f;
	
	if (closestSmallerCell) {
		directionToSmallerCell = closestSmallerCell->position - i.position;
		distanceToSmallerCell = directionToSmallerCell.length();
		directionToSmallerCell += closestSmallerCell->velocity;
	}

	Vector directionToLargerCell;
	float distanceToLargerCell = 0.0f;
	
	if (closestLargerCell) {
		directionToLargerCell = closestLargerCell->position - i.position;
		distanceToLargerCell = directionToLargerCell.length();
	}

	float sum = distanceToSmallerCell + distanceToLargerCell;
	if (EPSILON < sum) {
		distanceToSmallerCell /= sum;
		distanceToLargerCell /= sum;
	}
	
	force = (distanceToSmallerCell * directionToSmallerCell - distanceToLargerCell * directionToLargerCell) - i.velocity;

	/*
	// There are no cells to eat. Wait for death.
	if (!closestSmallerCell) {
		force.x = 0.0f;
		force.y = 0.0f;
		return;
	}
	
	Vector directionToSmallerCell = closestSmallerCell->position - i.position;
	Vector smallerCellVelocity = closestSmallerCell->velocity;

	// Chase the smaller cell but this time see where it is going.
	force = (directionToSmallerCell + smallerCellVelocity) - i.velocity;

	// If there is a bad guy
	if (closestLargerCell) {
		Vector largerCellVelocity = closestLargerCell->velocity;
		largerCellVelocity.normalize();
		
		Vector velocity = i.velocity;
		velocity.normalize();
		
		Vector directionToLargerCell = closestLargerCell->position - i.position;
		directionToLargerCell.normalize();

		float angleToLargerCell = chaos::cell::max(dot(velocity, directionToLargerCell), 0.0f);
		float angleToMe = chaos::cell::max(dot(largerCellVelocity, -directionToLargerCell), 0.0f);

		// If I am going straight into the larger cell - go exactly in the opposite direction.
		force = (1.0f - angleToLargerCell) * force - angleToLargerCell * directionToLargerCell;
		
		// dot(largerCellVelocity, inverseDirectionToLargerCell) * inverseDirectionToLargerCell + (closestSmallerCell ? closestSmallerCell->position - i.position : Vector());
	}
	*/
}