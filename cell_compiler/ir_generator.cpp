/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Bitcode/BitstreamReader.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Support/system_error.h" // error_code, not the same as std::error_code

#include "ir_generator.h"
#include "error.h"

namespace chaos { namespace cell {

using namespace std;

//! Name-mangling prefix used by all functions in the base module.
const string kFunctionPrefix = "cell_";

//! Casts the generic ASTContext to the specific ContextType.
#define MC (*contextFrom(ctx))

////////////////////////////////////////////////////////////////////////////////

llvm::Module* loadModule(const char* modulePath)
{
	if (modulePath == nullptr || *modulePath == '\0')
		return nullptr;

	llvm::OwningPtr<llvm::MemoryBuffer> buffer;
	llvm::MemoryBuffer::getFile(modulePath, buffer);

	if (!buffer)
		return nullptr;

	string error;
	auto module = llvm::ParseBitcodeFile(buffer.get(), llvm::getGlobalContext(), &error);

	if (!module)
	{
		trace("Error while parsing bitcode file %s: %s\n", modulePath, error.c_str());
		return nullptr;
	}

	return module;
}

//! Obtain an integer constant
inline llvm::ConstantInt* makeConstant(int value)
{
	return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(/*bits*/32, value, /*isSigned*/true));
}

//! Obtain a real constant.
inline llvm::ConstantFP* makeConstant(float value)
{
	return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(value));
}

//! Makes a LLVM type from the given type specifier.
inline llvm::Type* makeType(TypeSpecifier type, int nElements = 0)
{
	switch (type)
	{
	case TS_INT:
		return llvm::Type::getInt32Ty( llvm::getGlobalContext() );

	case TS_REAL:
		return llvm::Type::getFloatTy( llvm::getGlobalContext() );

	case TS_VECTOR:
		{
			auto realType = llvm::Type::getFloatTy( llvm::getGlobalContext() );
			return llvm::VectorType::get( realType, 2 ); // create a <2 x float>
		}
	}

	CellError::raise("type not supported");
	return nullptr;
}

//! The one and only IRBuilder
MyBuilder theBuilder(llvm::getGlobalContext());

////////////////////////////////////////////////////////////////////////////////
// IRGenerator

IRGenerator::IRGenerator(llvm::Module& module, const std::string& functionName)
	: _builder(theBuilder)
	, _module(module)
	, _main(nullptr) // find the 'cell_main' function
	, _pCells(nullptr)
	, _cellCount(nullptr)
	, _arenaSize(nullptr)
{
	if (functionName.empty())
		CellError::raise("main name not specified");

	auto mainTemplate = module.getFunction("cell_main_template");
	if (!mainTemplate)
		CellError::raise("cannot find main");

	// create a copy of the main template
	_main = llvm::Function::Create(mainTemplate->getFunctionType(),
		llvm::GlobalValue::ExternalLinkage, functionName, &_module);

	// prepare argument map for cloning
	llvm::ValueToValueMapTy map;
	for (auto& arg : mainTemplate->getArgumentList())
		map[&arg] = &arg;

	// map arguments
	for (auto arg = mainTemplate->arg_begin(), newArg = _main->arg_begin();
		arg != mainTemplate->arg_end(); ++arg, ++newArg)
	{
		map[arg] = newArg;
		newArg->setName(arg->getName());
	}

	// clone the template's body into the new function
	llvm::SmallVector<llvm::ReturnInst*, 10> returns;
	llvm::CloneFunctionInto(_main, mainTemplate, map, false, returns);

	// obtain the global LLVM context
	auto& llvmCtx = llvm::getGlobalContext();

	// get the body of the function
	auto& mainBlock = _main->getEntryBlock();

	// delete the 'ret void' instruction; we'll emit our terminator later
	mainBlock.getTerminator()->eraseFromParent();

	// attach the builder to the function's body
	_builder.SetInsertPoint(&mainBlock);

	// get the first parameter
	auto parameter = _main->arg_begin();
	
	// save the function's parameters for later use
	_pCells = parameter;
	_cellCount = ++parameter;
	_arenaSize = ++parameter;
	_force = ++parameter;
}

IRGenerator::~IRGenerator()
{
	for (auto p : _symbols) // clear the symbol table
		delete p.second;
}

llvm::Value* IRGenerator::evalExpression(ASTNode& node)
{
	ContextType newContext;
	node.accept(*this, &newContext);
	return newContext.value;
}

llvm::Value* IRGenerator::evalAddress(ASTNode& node, llvm::Value** writeIndex)
{
	ContextType newContext;
	newContext.wantsAddress = 1;
	node.accept(*this, &newContext);
	if (writeIndex)
		*writeIndex = newContext.writeIndex;
	return newContext.value;
}

bool IRGenerator::visitIdentifier(IdentifierNode& node, ContextType& ctx)
{
	if (ASTNode::instanceof( node.parent(), RID_MEMBER_ACCESS ))
	{ // accessing a member
	}
	else // referencing a normal variable
	{
		auto& it = _symbols.find(node.id());

		if (it == _symbols.end())
			CellError::raise(node.parsePosition(), "identifier not found: %s", node.id().c_str());

		if (ctx.wantsAddress)
			ctx.value = it->second->allocA;
		else // just load it
			ctx.value = _builder.CreateLoad(it->second->allocA, node.id());
	}
	return true;
}
// && ||
bool IRGenerator::visitLogicalExpression(BinaryExpressionBase& node, ContextType& ctx)
{
	auto left  = evalExpression(*node.leftOperand());
	auto right = evalExpression(*node.rightOperand());

	if (left == nullptr || right == nullptr)
		CellError::raise(node.parsePosition(), "null operand");

	auto leftTy  = left->getType();
	auto rightTy = right->getType();

	if (leftTy->isArrayTy() || rightTy->isArrayTy())
		CellError::raise(node.parsePosition(), "cannot combine arrays");

	if (!(leftTy->isIntegerTy() && rightTy->isIntegerTy()))
		CellError::raise(node.parsePosition(), "int expected");

	auto zero = makeConstant(0);

	switch (node.getOperator())
	{
	case OP_AND: // &&
		left  = _builder.CreateICmpNE(left, zero);
		right = _builder.CreateICmpNE(right, zero);
		left  = _builder.CreateAnd(left, right);
		ctx.value = _builder.CreateZExt(left, zero->getType(), "c_and");
		break;

	case OP_OR: // ||
		left  = _builder.CreateICmpNE(left, zero);
		right = _builder.CreateICmpNE(right, zero);
		left  = _builder.CreateOr(left, right);
		ctx.value = _builder.CreateZExt(left, zero->getType(), "c_or");
		break;
	}

	return false;
}

// & ^ | << >>
bool IRGenerator::visitBitwiseExpression(BinaryExpressionBase& node, ContextType& ctx)
{
	auto left  = evalExpression(*node.leftOperand());
	auto right = evalExpression(*node.rightOperand());

	if (left == nullptr || right == nullptr)
		CellError::raise(node.parsePosition(), "null operand");

	auto leftTy  = left->getType();
	auto rightTy = right->getType();

	if (leftTy->isArrayTy() || rightTy->isArrayTy())
		CellError::raise(node.parsePosition(), "cannot combine arrays");

	auto op = node.getOperator();
	auto makeDot = false;

	if (op == OP_BITXOR)
	{
		if (!leftTy->isVectorTy() || !rightTy->isVectorTy())
			CellError::raise(node.parsePosition(), "vec expected");
		makeDot = true;
	}
	else if (!leftTy->isIntegerTy() || !rightTy->isIntegerTy())
		CellError::raise(node.parsePosition(), "int expected");

	auto zero = makeConstant(0);

	switch (op)
	{
	case OP_BITAND: // &
		ctx.value = _builder.CreateAnd(left, right, "b_and");
		break;

	case OP_BITOR: // |
		ctx.value = _builder.CreateOr(left, right, "b_or");
		break;

	case OP_BITXOR: // ^
		if (makeDot)
		{
			auto dotFunction = _module.getFunction("cell_dot");
			ctx.value = _builder.CreateCall2(dotFunction, left, right, "v_dot");
		}
		else
		{
			ctx.value = _builder.CreateXor(left, right, "b_xor");
		}
		break;

	case OP_LSHIFT: // <<
		ctx.value = _builder.CreateShl(left, right, "b_shl");
		break;

	case OP_RSHIFT: // >>
		ctx.value = _builder.CreateAShr(left, right, "b_sar");
		break;

	default:
		CellError::raise(node.parsePosition(), "operation not supported");
	}

	return false;
}

// == != > >= < <=
bool IRGenerator::visitRelationalExpression(BinaryExpressionBase& node, ContextType& ctx)
{
	auto left  = evalExpression(*node.leftOperand());
	auto right = evalExpression(*node.rightOperand());

	if (left == nullptr || right == nullptr)
		CellError::raise(node.parsePosition(), "null operand");

	auto leftTy  = left->getType();
	auto rightTy = right->getType();

	if (leftTy->isArrayTy() || rightTy->isArrayTy())
		CellError::raise(node.parsePosition(), "cannot compare arrays");

	if (leftTy != rightTy)
		CellError::raise(node.parsePosition(), "cannot compare operands of different types");

	auto intTy = makeType(TS_INT);

	switch (node.getOperator())
	{
	case OP_EQ: // ==
		if (leftTy->isIntegerTy()) // int
		{
			left = _builder.CreateICmpEQ(left, right, "i_eq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_eq_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpOEQ(left, right, "f_eq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_eq_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpOEQ(left, right, "v_eq");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_eq_to_bool");
		}
		break;

	case OP_NOTEQ: // !=
			if (leftTy->isIntegerTy()) // int
		{
			left = _builder.CreateICmpNE(left, right, "i_neq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_neq_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpONE(left, right, "f_neq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_neq_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpONE(left, right, "v_neq");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_neq_to_bool");
		}
		break;

	case OP_GT: // >
		if (leftTy->isIntegerTy()) // int
		{
			left = _builder.CreateICmpSGT(left, right, "i_gt");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_gt_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpOGT(left, right, "f_gt");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_gt_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpOGT(left, right, "v_gt");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_gt_to_bool");
		}
		break;

	case OP_GTEQ: // >=
			if (leftTy->isIntegerTy()) // int
		{
		left = _builder.CreateICmpSGE(left, right, "i_gteq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_gteq_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpOGE(left, right, "f_gteq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_gteq_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpOEQ(left, right, "v_gteq");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_gteq_to_bool");
		}
		break;

	case OP_LT: // <
		if (leftTy->isIntegerTy()) // int
		{
			left = _builder.CreateICmpSLT(left, right, "i_lt");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_lt_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpOLT(left, right, "f_lt");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_lt_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpOLT(left, right, "v_lt");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_lt_to_bool");
		}
		break;

	case OP_LTEQ: // <=
		if (leftTy->isIntegerTy()) // int
		{
			left = _builder.CreateICmpSLE(left, right, "i_lteq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "i_lteq_to_bool");
		}
		else if (leftTy->isFloatTy()) // real
		{
			left = _builder.CreateFCmpOLE(left, right, "f_lteq");
			// extend i1 to i32
			ctx.value = _builder.CreateZExt(left, intTy, "f_lteq_to_bool");
		}
		else // assume vector
		{
			left = _builder.CreateFCmpOLE(left, right, "v_lteq");
			// left is now <2 x i1> but we need i32
			// extract the two vector elements
			auto e0 = _builder.CreateExtractElement(left, makeConstant(0), "e0");
			auto e1 = _builder.CreateExtractElement(left, makeConstant(1), "e1");
			// make e0 & e1 and extend it to i32
			e0 = _builder.CreateAnd(e0, e1);
			ctx.value = _builder.CreateZExt(e0, intTy, "v_lteq_to_bool");
		}
		break;

	default:
		CellError::raise(node.parsePosition(), "operation not supported");
	}

	return false;
}

// * / % + -
bool IRGenerator::visitBinaryExpression(BinaryExpressionBase& node, ContextType& ctx)
{
	auto left  = evalExpression(*node.leftOperand());
	auto right = evalExpression(*node.rightOperand());

	if (left == nullptr || right == nullptr)
		CellError::raise(node.parsePosition(), "null operand");

	auto leftTy  = left->getType();
	auto rightTy = right->getType();

	if (leftTy->isArrayTy() || rightTy->isArrayTy())
		CellError::raise(node.parsePosition(), "cannot combine arrays");

	if (leftTy->isVectorTy() && rightTy->isFloatingPointTy())
	{
		right = _builder.CreateVectorSplat(2, right, "splat_r");
	}
	else if (leftTy->isFloatingPointTy() && rightTy->isVectorTy())
	{
		left = _builder.CreateVectorSplat(2, left, "splat_l");
	}
	else if (leftTy->getTypeID() != rightTy->getTypeID())
		CellError::raise(node.parsePosition(), "operation not permitted");

	switch (node.getOperator())
	{
	case OP_MUL: // *
		if (leftTy->isIntegerTy()) // int
			ctx.value = _builder.CreateMul(left, right, "i_mul");
		else // real or vec
			ctx.value = _builder.CreateFMul(left, right, "f_mul");
		break;

	case OP_DIV: // /
		if (leftTy->isIntegerTy()) // int
			ctx.value = _builder.CreateSDiv(left, right, "i_div");
		else // real or vec
			ctx.value = _builder.CreateFDiv(left, right, "f_div");
		break;

	case OP_MOD: // %
		if (leftTy->isIntegerTy()) // int
			ctx.value = _builder.CreateSRem(left, right, "i_mod");
		else // real or vec
			ctx.value = _builder.CreateFRem(left, right, "f_rem");
		break;

	case OP_PLUS: // +
		if (leftTy->isIntegerTy()) // int
			ctx.value = _builder.CreateAdd(left, right, "i_add");
		else // real or vec
			ctx.value = _builder.CreateFAdd(left, right, "f_add");
		break;

	case OP_MINUS: // -
		if (leftTy->isIntegerTy()) // int
			ctx.value = _builder.CreateSub(left, right, "i_sub");
		else // real or vec
			ctx.value = _builder.CreateFSub(left, right, "f_sub");
		break;

	default:
		CellError::raise(node.parsePosition(), "operation not supported");
	}

	return false;
}

bool IRGenerator::visitUnaryExpression(UnaryExpressionBase& node, ContextType& ctx)
{
	auto value = evalExpression(*node.operand());

	if (!value)
		CellError::raise(node.operand()->parsePosition(), "null operand");

	auto type = value->getType();
	auto zero = makeConstant(0);

	switch (node.getOperator())
	{
	case OP_PLUS: // +
		// do nothing, use the same value
		ctx.value = value;
		break;

	case OP_MINUS: // -
		if (type->isIntegerTy()) // int
			ctx.value = _builder.CreateNeg(value, "i_neg");
		else // real or vec
			ctx.value = _builder.CreateFNeg(value, "f_neg");
		break;

	case OP_NOT: // !
		if (type->isIntegerTy())
		{
			value = _builder.CreateICmpEQ(value, zero, "i_not"); // i1
			ctx.value = _builder.CreateZExt(value, zero->getType(), "i_not_to_bool"); // extend to i32
		}
		// Here
		else
		{
			CellError::raise(node.parsePosition(), "int or vec expected");
		}
		break;

	case OP_BITNOT:
		ctx.value = _builder.CreateNot(value, "b_not");
		break;

	default:
		CellError::raise(node.parsePosition(), "operation not supported");
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Declarations

bool IRGenerator::visit(TypeModifierNode& node, ASTContext* ctx)
{
	MC.isGlobal = 1; // the only possible modifier is 'global'
	return true;
}

bool IRGenerator::preVisit(VariableDeclarationNode& node, ASTContext* ctx)
{
	MC.reset();
	return true;
}

bool IRGenerator::visit(VariableDeclarationNode& node, ASTContext* ctx)
{
	auto& block = _main->getEntryBlock();
	MyBuilder allocaBuilder(&block, block.begin());
	auto type = makeType(node.type());
	auto allocA = allocaBuilder.CreateAlloca(type, MC.value, MC.name);

	auto& it = _symbols.find(MC.name);
	if (it != _symbols.end())
		it->second->allocA = allocA; // attach the value to the symbol

	if (MC.isGlobal)
	{
		// TODO: Take care of global variables
	}

	return true;
}

bool IRGenerator::preVisit(VariableDeclaratorNode& node, ASTContext* ctx)
{
	if (_symbols.find(node.id()) != _symbols.end())
		CellError::raise(node.parsePosition(), "variable redefenition", node.id());

	_symbols.insert( make_pair(node.id(), new IRSymbol()) );
	MC.name = node.id();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Literals

bool IRGenerator::visit(IntegerLiteralNode& node, ASTContext* ctx)
{
	MC.value = makeConstant(node.value());
	return true;
}

bool IRGenerator::visit(RealLiteralNode& node, ASTContext* ctx)
{
	MC.value = makeConstant(node.value());
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Identifiers

bool IRGenerator::visit(IdentifierNode& node, ASTContext* ctx)
{
	return visitIdentifier(node, MC);
}

bool IRGenerator::visit(QualifiedIdentifierNode& node, ASTContext* ctx)
{
	return visitIdentifier(node, MC);
}

bool IRGenerator::visit(SystemIdentifierNode& node, ASTContext* ctx)
{
	auto& id = node.id();

	if (id == "CellCount")
	{
		MC.value = _cellCount;
	}
	else if (id == "ArenaRadius")
	{
		MC.value = _arenaSize;
	}
	else if (id == "Radius")
	{
		MC.value = _module.getFunction("read_radius");
	}
	else if (id == "Position")
	{
		MC.value = _module.getFunction("read_position");
	}
	else if (id == "Velocity")
	{
		MC.value = _module.getFunction("read_velocity");
	}
	else if (id == "Force")
	{
		if (!MC.wantsAddress)
			CellError::raise("write-only variable");
		MC.value = _force;
	}
	else
	{
		CellError::raise("unknown system variable");
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Statements

bool IRGenerator::traverseStatement(ASTNode& node, llvm::BasicBlock** blockToUpdate)
{
	ContextType newContext;
	bool continueTraversal = node.accept(*this, &newContext);
	if (blockToUpdate)
	{
		if (_builder.GetInsertBlock() == nullptr) {
			return false;
		}
		assert(_builder.GetInsertBlock() && "null basic block");
		*blockToUpdate = _builder.GetInsertBlock();
	}
	return continueTraversal;
}

bool IRGenerator::visit(BlockNode& node, ASTContext* ctx)
{
	if (_main && ASTNode::instanceof(node.parent(), RID_START_SYMBOL))
	{ // we are exiting form the 'main' block
		auto basicBlock = _builder.GetInsertBlock();
		
		// terminate the basic block if needed
		if (basicBlock && basicBlock->getTerminator() == nullptr)
			_builder.CreateRetVoid();

		// make sure the populated function is well-formed
		llvm::verifyFunction(*_main, llvm::PrintMessageAction);

		// detach the builder
		_builder.ClearInsertionPoint();
	}

	return true;
}

bool IRGenerator::preVisit(IfStatementNode& node, ASTContext* ctx)
{
	auto condition = evalExpression(*node.condition());
	auto conditionType = llvm::dyn_cast_or_null<llvm::IntegerType>(condition->getType());
	
	if (!conditionType)
		CellError::raise(node.parsePosition(), "invalid 'if' condition type");

	if (conditionType->getBitWidth() != 1) // LLVM expects i1, so make a conversion if needed
		condition = _builder.CreateICmpNE(condition, makeConstant(0), "if_condition");

	bool hasElse = (node.elseBody() != nullptr);

	auto& llvmCtx = llvm::getGlobalContext();
	auto& blocks  = _main->getBasicBlockList(); // the list of all function blocks

	auto mergeBlock = llvm::BasicBlock::Create(llvmCtx, "IF_MERGE");
	auto thenBlock  = llvm::BasicBlock::Create(llvmCtx, "IF_THEN", _main);
	auto elseBlock  = hasElse ? llvm::BasicBlock::Create(llvmCtx, "IF_ELSE") : mergeBlock;

	_builder.CreateCondBr(condition, thenBlock, elseBlock);

	// the then block
	_builder.SetInsertPoint(thenBlock);
	traverseStatement( *node.thenBody(), &thenBlock );
	if (thenBlock->getTerminator() == nullptr)
		_builder.CreateBr(mergeBlock);

	// the else block
	if (hasElse)
	{
		blocks.push_back(elseBlock);
		_builder.SetInsertPoint(elseBlock);
		traverseStatement( *node.elseBody(), &elseBlock );
		if (elseBlock->getTerminator() == nullptr)
			_builder.CreateBr(mergeBlock);
	}

	// the merge block
	blocks.push_back(mergeBlock);
	_builder.SetInsertPoint(mergeBlock);

	return false;
}

bool IRGenerator::preVisit(WhileStatementNode& node, ASTContext* ctx)
{
	auto& llvmCtx = llvm::getGlobalContext();
	
	auto conditionBlock = llvm::BasicBlock::Create(llvmCtx, "WHILE_CONDITION", _main);
	auto loopBlock = llvm::BasicBlock::Create(llvmCtx, "WHILE_BODY");
	auto endBlock = llvm::BasicBlock::Create(llvmCtx, "WHILE_END");

	_builder.CreateBr(conditionBlock); // jump right to the condition

	// the condition

	_builder.SetInsertPoint(conditionBlock);
	auto condition = evalExpression(*node.condition());
	auto conditionType = llvm::dyn_cast_or_null<llvm::IntegerType>(condition->getType());

	if (!conditionType)
		CellError::raise(node.parsePosition(), "invalid 'while' condition type");

	if (conditionType->getBitWidth() != 1) // LLVM expects i1, so make a conversion if needed
		condition = _builder.CreateICmpNE(condition, makeConstant(0), "while_condition");
	
	// select between the body and the end
	_builder.CreateCondBr(condition, loopBlock, endBlock);

	// the loop body
	auto& blocks = _main->getBasicBlockList(); // the list of all function blocks
	blocks.push_back(loopBlock); // add the while body to the function's block list
	_builder.SetInsertPoint(loopBlock);
	traverseStatement( *node.body(), &loopBlock );
	
	// jump back to the condition block
	if (loopBlock->getTerminator() == nullptr)
		_builder.CreateBr(conditionBlock);

	// the loop end
	blocks.push_back(endBlock);
	_builder.SetInsertPoint(endBlock);

	return false;
}

bool IRGenerator::visit(QuitStatementNode& node, ASTContext* ctx)
{
	auto basicBlock = _builder.GetInsertBlock();
	if (basicBlock->getTerminator() == nullptr)
		_builder.CreateRetVoid(); // return from the main function
	_builder.ClearInsertionPoint();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Expressions

bool IRGenerator::preVisit(MultiplicativeExpressionNode& node, ASTContext* ctx)
{
	return visitBinaryExpression(node, MC);
}

bool IRGenerator::preVisit(AdditiveExpressionNode& node, ASTContext* ctx)
{
	return visitBinaryExpression(node, MC);
}

bool IRGenerator::preVisit(RelationalExpressionNode& node, ASTContext* ctx)
{
	return visitRelationalExpression(node, MC);
}

bool IRGenerator::preVisit(EqualityExpressionNode& node, ASTContext* ctx)
{
	return visitRelationalExpression(node, MC);
}

bool IRGenerator::preVisit(ShiftExpressionNode& node, ASTContext* ctx)
{
	return visitBitwiseExpression(node, MC);
}

bool IRGenerator::preVisit(AndExpressionNode& node, ASTContext* ctx)
{
	return visitBitwiseExpression(node, MC);
}

bool IRGenerator::preVisit(ExclusiveOrExpressionNode& node, ASTContext* ctx)
{
	return visitBitwiseExpression(node, MC);
}

bool IRGenerator::preVisit(InclusiveOrExpressionNode& node, ASTContext* ctx)
{
	return visitBitwiseExpression(node, MC);
}

bool IRGenerator::preVisit(ConditionalAndExpressionNode& node, ASTContext* ctx)
{
	return visitLogicalExpression(node, MC);
}

bool IRGenerator::preVisit(ConditionalOrExpressionNode& node, ASTContext* ctx)
{
	return visitLogicalExpression(node, MC);
}

bool IRGenerator::preVisit(UnaryExpressionNode& node, ASTContext* ctx)
{
	return visitUnaryExpression(node, MC);
}

bool IRGenerator::preVisit(PostfixExpressionNode& node, ASTContext* ctx)
{
	return visitUnaryExpression(node, MC);
}

bool IRGenerator::preVisit(AssignmentNode& node, ASTContext* ctx)
{
	llvm::Value* writeIndex = nullptr;
	auto right = evalExpression(*node.rightOperand());
	auto left = evalAddress(*node.leftOperand(), &writeIndex);

	if (llvm::dyn_cast_or_null<llvm::AllocaInst>(left) == nullptr &&
		!left->getType()->isPointerTy()
		)
		CellError::raise(node.parsePosition(), "cannot store in r-value");

	auto leftTy = left->getType();
	auto rightTy = right->getType();

	if (leftTy->isArrayTy() || rightTy->isArrayTy())
		CellError::raise("cannot assign arrays");

	if (writeIndex) // we have insert element
	{
		llvm::Value* instr = _builder.CreateLoad(left, "v_load");
		instr = _builder.CreateInsertElement(instr, right, writeIndex, "v_insert");
		MC.value = _builder.CreateStore(instr, left, "v_store");
	}
	else
	{
		MC.value = _builder.CreateStore(right, left);
	}

	return false;
}

bool IRGenerator::preVisit(MemberAccessNode& node, ASTContext* ctx)
{
//	if (!(MC.value->getType()->isVectorTy()))
//		CellError::raise(node.parsePosition(), "only vectors have memebers");

	auto first = node.firstChild();
	auto member = ASTNode::instanceof(first, RID_QUALIFIED_IDENTIFIER) ? static_cast<QualifiedIdentifierNode*>(first) : nullptr;
	auto& id = member->id();

	if (MC.wantsAddress)
	{
		if (id == "x")
			MC.writeIndex = makeConstant(0);
		else if (id == "y")
			MC.writeIndex = makeConstant(1);
		else
			CellError::raise(node.parsePosition(), "unknown member");
	}
	else
	{
		if (id == "x")
		{
			MC.value = _builder.CreateExtractElement(MC.value, makeConstant(0), "extract_x");
		}
		else if (id == "y")
		{
			MC.value = _builder.CreateExtractElement(MC.value, makeConstant(1), "extract_y");
		}
		else if (id == "length")
		{
			auto lengthFunction = _module.getFunction("cell_length");
			MC.value = _builder.CreateCall(lengthFunction, MC.value, "v_length");
		}
		else if (id == "normalized")
		{
			auto normalizeFunction = _module.getFunction("cell_normalize");
			MC.value = _builder.CreateCall(normalizeFunction, MC.value, "v_normalized");
		}
		else
		{
			CellError::raise(node.parsePosition(), "unknown member");
		}
	}

	return false;
}

bool IRGenerator::preVisit(ElementAccessNode& node, ASTContext* ctx)
{
	auto index = evalExpression(*node.firstChild());
	auto leftTy = MC.value->getType();

	if (leftTy->isVectorTy()) // vector access
	{
		if (llvm::dyn_cast_or_null<llvm::Constant>(index))
			CellError::raise(node.parsePosition(), "constant index expected");
		MC.value = _builder.CreateExtractElement(MC.value, index, "v_element");
	}
	else if (leftTy->isArrayTy()) // array access
	{
		MC.value = _builder.CreateGEP(MC.value, index, "a_element");
	}
	else if (llvm::dyn_cast_or_null<llvm::Function>(MC.value)) // some of the 'read_XXX' functions
	{
		MC.value = _builder.CreateCall2(MC.value, _pCells, index, "read_call");
	}
	else
	{
		CellError::raise(node.parsePosition(), "array expected");
	}

	 return false;
}

bool IRGenerator::preVisit(InvocationNode& node, ASTContext* ctx)
{
	auto calleeName = static_cast<QualifiedIdentifierNode*>(node.invocationName())->id();
	auto callee = _module.getFunction(kFunctionPrefix + calleeName);

	if (!callee)
		CellError::raise(node.parsePosition(), "function not found %s", calleeName.c_str());

	vector<llvm::Value*> args;

	for (auto arg = node.invocationArguments()->firstChild(); arg != nullptr; arg = arg->nextSibling())
		args.push_back( evalExpression(*arg) );

	MC.value = _builder.CreateCall(callee, args, calleeName);
	return false;
}

}} // chaos::cell

