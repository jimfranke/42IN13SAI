#pragma once

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>

#include "TokenDefinition.h"
#include "Token.h"
#include "TokenPartner.h"
#include "PartnerNotFoundException.h"
#include "ParseException.h"

class Tokenizer
{
private:
	int lineNumber;
	int linePosition;
	int level;
	std::string lineRemaining;

    std::ifstream file;
    std::list<TokenDefinition> tokenDefinitions;
    std::vector<TokenPartner> tokenPartners;
    
    std::vector<Token> tokenVector;
    
    void NextLine();
    Token* FindPartner(MyTokenType &myTokenType, int level);
	Token* FindPartnerR(MyTokenType &myTokenType, int level);
	bool ShouldFindPartner(MyTokenType type);
	bool ShouldFindPartnerR(MyTokenType type);
    
public:
    Tokenizer(std::string fileLocation, std::list<TokenDefinition> definitions, std::vector<TokenPartner> partners);
	virtual ~Tokenizer();

    void Tokenize();
    std::vector<Token> GetTokenList();
};