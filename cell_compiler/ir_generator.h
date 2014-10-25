/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ir_generator_H
#define __CELL_ir_generator_H

#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "boost/unordered_map.hpp"
#include "ast_visitor.h"

namespace chaos { namespace cell {

//! Loads a module from a .bc file.
llvm::Module* loadModule(const char* modulePath);

//! Dumps a module to a file.
//void dumpModule(const llvm::Module& module, const char* path);

//! Propagates data during AST traversal.
struct IRContext : ASTContext
{
	IRContext()
	{ reset(); }

	virtual ~IRContext()
	{}

	void reset()
	{
		value = nullptr;
		wantsAddress = 0;
		isGlobal = 0;
		nElements = 0;
		name.clear();
		writeIndex = nullptr;
	}

	llvm::Value* value; //! The evaluation result.
	unsigned wantsAddress : 1; //! Return the alloca of the evaluated expression instead of its value
	unsigned isGlobal : 1; //! Set if marked with 'global'
	unsigned unused : 30; // ! Reserved bits.
	int nElements; //! Number of array elements.
	llvm::Value* writeIndex; //! Index of the element to store into.
	std::string name; //! Optional instruction name.
};

//! Holds information associated with variables.
struct IRSymbol
{
	IRSymbol(llvm::Value* a = nullptr) : allocA(a)
	{}

	IRSymbol(const IRSymbol& other) : allocA(other.allocA)
	{}

	~IRSymbol()
	{}

	IRSymbol& operator=(const IRSymbol& other)
	{
		allocA = other.allocA;
		return *this;
	}

// Public attributes

	llvm::Value* allocA; //! Address of the variable.
};

//! The symbol table is just a simple hash-map.
typedef boost::unordered_map<std::string, IRSymbol*> IRSymbolMap;

//! Specialization for IRBuilder.
#ifdef _DEBUG
	typedef llvm::IRBuilder</*preserveNames*/true> MyBuilder;
#else
	typedef llvm::IRBuilder</*preserveNames*/false> MyBuilder;
#endif

//! Generates LLVM IR code from the AST.
class IRGenerator : public ASTVisitorMix<IRContext>
{
public:
	IRGenerator(llvm::Module& module, const std::string& functionName);
	virtual ~IRGenerator();

	virtual bool visit(IntegerLiteralNode& node, ASTContext* ctx);
	virtual bool visit(RealLiteralNode& node, ASTContext* ctx);

	virtual bool visit(TypeModifierNode& node, ASTContext* ctx);

	virtual bool preVisit(VariableDeclarationNode& node, ASTContext* ctx);
	virtual bool visit(VariableDeclarationNode& node, ASTContext* ctx);
	virtual bool preVisit(VariableDeclaratorNode& node, ASTContext* ctx);

	virtual bool visit(IdentifierNode& node, ASTContext* ctx);
	virtual bool visit(QualifiedIdentifierNode& node, ASTContext* ctx);
	virtual bool visit(SystemIdentifierNode& node, ASTContext* ctx);

	virtual bool preVisit(MemberAccessNode& node, ASTContext* ctx);
	virtual bool preVisit(ElementAccessNode& node, ASTContext* ctx);

	virtual bool visit(BlockNode& node, ASTContext* ctx);

	virtual bool preVisit(InvocationNode& node, ASTContext* ctx);

	virtual bool preVisit(IfStatementNode& node, ASTContext* ctx);
	virtual bool preVisit(WhileStatementNode& node, ASTContext* ctx);
	virtual bool visit(QuitStatementNode& node, ASTContext* ctx);

	virtual bool preVisit(AssignmentNode& node, ASTContext* ctx);

	virtual bool preVisit(MultiplicativeExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(AdditiveExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(RelationalExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(EqualityExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(AndExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(ExclusiveOrExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(InclusiveOrExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(ConditionalAndExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(ConditionalOrExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(ShiftExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(UnaryExpressionNode& node, ASTContext* ctx);
	virtual bool preVisit(PostfixExpressionNode& node, ASTContext* ctx);

private:
	bool traverseStatement(ASTNode& node, llvm::BasicBlock** blockToUpdate);
	llvm::Value* evalExpression(ASTNode& node);
	llvm::Value* evalAddress(ASTNode& node, llvm::Value** writeIndex = nullptr);
	bool visitIdentifier(IdentifierNode& node, ContextType& ctx);
	bool visitLogicalExpression(BinaryExpressionBase& node, ContextType& ctx);
	bool visitBitwiseExpression(BinaryExpressionBase& node, ContextType& ctx);
	bool visitRelationalExpression(BinaryExpressionBase& node, ContextType& ctx);
	bool visitBinaryExpression(BinaryExpressionBase& node, ContextType& ctx);
	bool visitUnaryExpression(UnaryExpressionBase& node, ContextType& ctx);

private:
	IRSymbolMap _symbols; //! the symbol table
	MyBuilder& _builder;
	llvm::Module& _module; //! the module to be populated
	llvm::Function* _main; //! points to 'void cell_main(Cell* all, int count, int arenaSize)'
	llvm::Argument* _pCells; //! points to the 'all' parameter
	llvm::Argument* _cellCount; //! points to the 'count' parameter
	llvm::Argument* _arenaSize; //! points to the 'arenaSize' parameter
	llvm::Argument* _force; //! The output from the main function.
};

}} // chaos::cell

#endif // __CELL_ir_generator_H

