#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(SymbolTable* symboltable, SubroutineTable* subroutine, std::vector<CompilerNode> compiler_nodes)
	: _symboltable(symboltable), _subroutine(subroutine), _compilernodes(compiler_nodes)
{
	function_caller = new FunctionCaller(this);
}

VirtualMachine::~VirtualMachine(){}

CompilerNode* VirtualMachine::PeekNext()
{
	CompilerNode *node = &_compilernodes.at(currentIndex + 1);
	return node;
}

CompilerNode VirtualMachine::GetNext()
{
	CompilerNode node;
    currentIndex++;
	if (currentIndex <= _compilernodes.size()-1 && _compilernodes.size() > 0)
	{
		node = _compilernodes.at(currentIndex);
	}
	else
	{
		throw std::runtime_error("Compilernode missing");
	}

	return node;
}

void VirtualMachine::ExecuteCode()
{
	// Only when there are compilernodes
	if (_compilernodes.size() > 0)
	{
		// First check all compilernodes for global variables
		do 
		{
			if (PeekNext() != nullptr)
			{
				CompilerNode node = VirtualMachine::GetNext();
				std::string function_call = node.get_expression();
				function_caller->Call(function_call, node);
			}
		} while (currentIndex < _compilernodes.size()-1);

		// Find main subroutine
		_subroutine->GetSubroutine("main");
	}
}

CompilerNode* VirtualMachine::CallFunction(CompilerNode node)
{
    std::string function_call = node.get_expression();
    // call the compilernode function
    return function_caller->Call(function_call, node);
}

#pragma region VariableOperations
CompilerNode* VirtualMachine::ExecuteAssignment(CompilerNode compilerNode)
{
	// Check if params is not empty
	if (compilerNode.get_nodeparamters().empty())
		throw ParameterException(2, ParameterExceptionType::NoParameters);
	
    // Get the Node parameters
	std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than two parameters
    if (parameters.size() > 2)
        throw ParameterException(2, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
	CompilerNode* param1 = parameters.at(0);
	CompilerNode* param2 = parameters.at(1);
    
	// Only go through when param is identifier
	if (param1->get_expression() == "$identifier")
	{
		// Get the value of the node -> variable
		std::string variableName = param1->get_value();
		if (param2->get_expression() != "$value")
			param2 = CallFunction(*param2);
        
        // Get the variable from symboltable
		Symbol* current_symbol = _symboltable->GetSymbol(variableName);
        
        // Get the param value and set in temp var
        float valueToSet = atof(param2->get_value().c_str());
		current_symbol->SetValue(valueToSet);
	}
    
    return nullptr;
}

CompilerNode* VirtualMachine::ExecuteGetVariable(CompilerNode compilerNode)
{
    // Check if params is not empty
    if (compilerNode.get_value().empty())
        throw ParameterException(1, ParameterExceptionType::NoParameters);
    
    // Get the Node parameter
    std::string parameter = compilerNode.get_value();
    
    // Get the symbol
    Symbol* current_symbol = _symboltable->GetSymbol(parameter);
    
    // Create the return node
    CompilerNode* returnNode = new CompilerNode("$value", std::to_string(current_symbol->GetValue()));
    
    return returnNode;
}
#pragma endregion VariableOperations

#pragma region SimpleMath
CompilerNode *VirtualMachine::ExecuteAddOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(2, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than two parameters
    if (parameters.size() > 2)
        throw ParameterException(2, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    CompilerNode* param2 = parameters.at(1);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    if (param2->get_expression() != "$value")
        param2 = CallFunction(*param2);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float num2 = atof(param2->get_value().c_str());
    float sum = num1 + num2;
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
     
	return rNode;
}

CompilerNode* VirtualMachine::ExecuteMinusOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(2, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than two parameters
    if (parameters.size() > 2)
        throw ParameterException(2, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    CompilerNode* param2 = parameters.at(1);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    if (param2->get_expression() != "$value")
        param2 = CallFunction(*param2);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float num2 = atof(param2->get_value().c_str());
    float sum  = num1 - num2;
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

CompilerNode* VirtualMachine::ExecuteMultiplyOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(2, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than two parameters
    if (parameters.size() > 2)
        throw ParameterException(2, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    CompilerNode* param2 = parameters.at(1);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    if (param2->get_expression() != "$value")
        param2 = CallFunction(*param2);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float num2 = atof(param2->get_value().c_str());
    float sum = num1 * num2;
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

CompilerNode* VirtualMachine::ExecuteDivideOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(2, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than two parameters
    if (parameters.size() > 2)
        throw ParameterException(2, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    CompilerNode* param2 = parameters.at(1);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    if (param2->get_expression() != "$value")
        param2 = CallFunction(*param2);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float num2 = atof(param2->get_value().c_str());
    float sum = num1 / num2;
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

#pragma endregion SimpleMath

#pragma region ComplexMath

CompilerNode* VirtualMachine::ExecuteSinOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(1, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than one parameter
    if (parameters.size() > 1)
        throw ParameterException(1, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float sum = std::sin(num1);
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

CompilerNode* VirtualMachine::ExecuteCosOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(1, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than one parameter
    if (parameters.size() > 1)
        throw ParameterException(1, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float sum = std::cos(num1);
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

CompilerNode* VirtualMachine::ExecuteTanOperation(CompilerNode compilerNode)
{
    if (compilerNode.get_nodeparamters().empty())
        throw ParameterException(1, ParameterExceptionType::NoParameters);
    
    // Get the Node parameters
    std::vector<CompilerNode *> parameters = compilerNode.get_nodeparamters();
    
    // Check if there aren't more than one parameter
    if (parameters.size() > 1)
        throw ParameterException(1, parameters.size(), ParameterExceptionType::IncorrectParameters);
    
    CompilerNode* param1 = parameters.at(0);
    
    // Check if the parameters are a value or another function call
    // if function call, execute function
    if (param1->get_expression() != "$value")
        param1 = CallFunction(*param1);
    
    // Parse the parameters to a float for mathmatic operation
    float num1 = atof(param1->get_value().c_str());
    float sum = std::tan(num1);
    
    // Create a new value compilernode to return
    CompilerNode* rNode = new CompilerNode("$value", std::to_string(sum));
    
    return rNode;
}

#pragma endregion ComplexMath


