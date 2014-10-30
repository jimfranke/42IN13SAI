#pragma once
#include "SymbolTable.h"
#include "SubroutineTable.h"
#include "CompilerNode.h"
#include "FunctionCaller.h"

class VirtualMachine
{
public:
	VirtualMachine(SymbolTable symboltable, SubroutineTable subroutine, std::vector<CompilerNode> compiler_nodes);
	virtual ~VirtualMachine();
	typedef CompilerNode(VirtualMachine::*MFP)(CompilerNode);
	void ExecuteCode();
	CompilerNode ExecuteAddOperation(CompilerNode compilerNode);
	
private:
	SymbolTable _symboltable;
	SubroutineTable _subroutine;
	CompilerNode* PeekNext();
	CompilerNode GetNext();
	FunctionCaller* function_caller;

	std::vector<CompilerNode> _compilernodes;
	std::vector<CompilerNode> _received_compilernodes;

	int currentIndex = 0;

	//bool isAction(MFP);
};

