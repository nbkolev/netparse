#ifndef NETPARSE_TOKENS_H
#define NETPARSE_TOKENS_H

#include "config.h"

#include <list>
#include <string>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstdlib>


using namespace std;

class Token 
{
private:
    bool TokenizeLiteralNumber(const string expression, unsigned int beginPos)
    {
        unsigned int lastPos = beginPos;
        
        // Greedy number look ahead
        for(unsigned int curPos= beginPos;curPos < expression.length(); curPos++)
        {
            char charAtPos = expression.at(curPos);
            if(!isdigit(charAtPos)){
                break;
            }
            // Advance if able to consume
            lastPos = curPos + 1;
        }

        if(lastPos == beginPos) 
        {
            // Recognition was unsuccessful
            return false;
        }
     
        // Recognition was successful
        this->content = expression.substr(beginPos,lastPos - beginPos);
        type = LITERAL_NUMBER;
        return true;
    }

    bool TokeniseSignOrParentheses(const string expression, unsigned int begin_pos)
    {
        char charAtPos = expression.at(begin_pos);
        this->content = charAtPos;
        switch(charAtPos)
        {
        case '+':
            type = PlUS_SIGN;
            return true;
        case '-':
            type = MINUS_SIGN;
            return true;
        case '*':
            type = MULTIPLY_SIGN;
            return true;
        case '/':
            type = DIVISION_SIGN;
            return true;
        case '\\':
            type = DIVISION_SIGN;
            return true;
        case '(':
            type = LEFT_BRACKET;
            return true;
        case ')':
            type = RIGHT_BRACKET;
            return true;
        default:
            throw std::invalid_argument("Unknown operator.");
        }
    }
    enum TokenType
    {
        PlUS_SIGN,
        MINUS_SIGN,
        MULTIPLY_SIGN,
        DIVISION_SIGN,
        LITERAL_NUMBER,
        LEFT_BRACKET,
        RIGHT_BRACKET,
    };
    string content;
    TokenType type;

public:
    bool isBracket(){
        return isLeftBracket() || isRightBracket();
    }
    bool isPlus(){
        return type ==PlUS_SIGN;
    }
    bool isMinus(){
        return type == MINUS_SIGN;
    }
    bool isMultiplication(){
        return type == MULTIPLY_SIGN;
    }
    bool isDivision(){
        return type == DIVISION_SIGN;
    }

    bool isLeftBracket(){
        return type == LEFT_BRACKET;
    }

    bool isRightBracket(){
        return type == RIGHT_BRACKET;
    }

    bool isNumber() {
        return type == LITERAL_NUMBER;
    }

    double getNumberVal(){
        return stoi(content);
    }

    int getOperatorPrecedence(){
        switch(type){
        case PlUS_SIGN:
            return 2;
        case MINUS_SIGN:
            return 2;
        case MULTIPLY_SIGN:
            return 3;
        case DIVISION_SIGN:
            return 3;
        default:
            throw std::invalid_argument("Invalid operator.");
        }
    }

    bool isOperator(){
        switch(type){
            case PlUS_SIGN:
            case MINUS_SIGN:
            case MULTIPLY_SIGN:
            case DIVISION_SIGN:
            case LEFT_BRACKET:
            case RIGHT_BRACKET:
                return true;
            default:
                return false;
        }
    }

    unsigned int lenght(){
        return content.length();
    }

    string getContent(){
        return content;
    }
    
    Token(const string expression, unsigned int begin_pos)
    {
        // Try matching different token types
        TokenizeLiteralNumber(expression, begin_pos) ||
            TokeniseSignOrParentheses(expression, begin_pos);
    }

    static list<Token> Tokenise(const string expression){
        list<Token> tokens;
    
        for(unsigned int pos = 0; pos < expression.length(); )
        {
            // Ignore whitespaces
            if(expression.at(pos) == ' ') 
            {
                pos++;
                continue;
            }
            // Token object recognises the immediate token
            Token immediate = Token(expression, pos);
            
            pos += immediate.lenght();
            tokens.push_back(immediate);
        }

        return tokens;
    }
};

#endif