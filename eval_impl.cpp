#include "eval/eval.h"
#include "eval/tokens.h"
#include <cmath>
#include <vector>
#include <stack>
#include <list>


typedef long double scalar;

using namespace std;

// Some code to ease RPL testing
#ifdef DEBUG
void DBG_PRINTER(string s, list<Token> q) {
    cout<<s<<": ";
    for (auto x :q) cout<<x.getContent()<<",";
    cout<<endl<<flush;
}
#define DEBUG_ONLY(statement) statement
#define DEBUG_ONLY_PRINT_TOKENS(tokenlist) DBG_PRINTER(#tokenlist,tokenlist)
#else
#define DEBUG_ONLY(statement)
    #define DEBUG_ONLY_PRINT_TOKENS(tokenlist)

#endif

list<Token> sort_reverse_polish(list<Token> inputQueue)
{
    list<Token> outQueue;
    list<Token> opsStack;

    // Adaptation of https://en.wikipedia.org/wiki/Shunting_yard_algorithm

    /* while there are tokens to be read: */
    while (!inputQueue.empty())
    {
        DEBUG_ONLY_PRINT_TOKENS(inputQueue);
        DEBUG_ONLY_PRINT_TOKENS(opsStack);
        DEBUG_ONLY_PRINT_TOKENS(outQueue);
        DEBUG_ONLY(cout<<endl);

        Token curToken = inputQueue.front();
        inputQueue.pop_front();

        if(curToken.isNumber())
        {
            outQueue.push_back(curToken);
            continue;
        }
        else if(curToken.isLeftBracket())
        {
            opsStack.push_back(curToken);
            continue;
        }
        else if(curToken.isRightBracket())
        {
            if(opsStack.empty())
            {
                throw std::invalid_argument("Opening bracket missing. Operation stack empty.");
            }
            while(!opsStack.empty() && !opsStack.back().isLeftBracket())
            {
                outQueue.push_back(opsStack.back());
                opsStack.pop_back();
            }
            if(opsStack.empty())
            {
                throw std::invalid_argument("Opening bracket missing. Operation stack exhausted.");
            }
            opsStack.pop_back(); // Discard opening bracket
            continue;
        }
        else if(curToken.isOperator())
        {
            while(true){
                if(opsStack.empty() || opsStack.back().isLeftBracket()) break;
                if(!curToken.isBracket()){
                    auto operPrecedence = opsStack.back().getOperatorPrecedence();
                    auto currentOper = curToken.getOperatorPrecedence();
                    if(operPrecedence < currentOper) break;
                    outQueue.push_back(opsStack.back());
                    opsStack.pop_back();
                }
            }
            opsStack.push_back(curToken);
            continue;
        }
    }
    while(!opsStack.empty())
    {
        if(opsStack.back().isLeftBracket())
        {
            throw std::invalid_argument("Opening bracket missing. Operation stack exhausted.");
        }
        outQueue.push_back(opsStack.back());
        opsStack.pop_back();
    }

    DEBUG_ONLY_PRINT_TOKENS(inputQueue);
    DEBUG_ONLY_PRINT_TOKENS(opsStack);
    DEBUG_ONLY_PRINT_TOKENS(outQueue);
    DEBUG_ONLY(cout<<endl);

    return outQueue;
}

scalar evalOperator(scalar op1, scalar op2, Token t){
    if(t.isPlus()) return op2 + op1;
    if(t.isMinus()) return op2 - op1;
    if(t.isMultiplication()) return op2 * op1;
    if(t.isDivision()) return op2 / op1;

    return NAN;
}
scalar reverse_polish_sorted_evaluate(list<Token> input){
    scalar accumulatorA, accumulatorB;
    stack<scalar> calcStack;
    for(auto curToken : input){
        if(curToken.isOperator()){
            accumulatorA = calcStack.top();
            calcStack.pop();
            if(curToken.isMinus() && calcStack.size() == 0){
                // Address and edge case for minus sign for sign of numbers
                // so -1 is modeled as 0-1
                calcStack.push(evalOperator(accumulatorA, 0, curToken));
                continue;
            }
            accumulatorB = calcStack.top();
            calcStack.pop();
            calcStack.push(evalOperator(accumulatorA, accumulatorB, curToken));
        }
        else if(curToken.isNumber()){
            calcStack.push(curToken.getNumberVal());
        }
    }
    return calcStack.top();
}

integer evaluate(string expression){
    auto aritmeticExpressionTokens = Token::Tokenise(expression);
    if(aritmeticExpressionTokens.begin() == aritmeticExpressionTokens.end()){
        return 0; // An undefined case
    }
    auto reversePolishSortedTokens = sort_reverse_polish(aritmeticExpressionTokens);
    scalar result = reverse_polish_sorted_evaluate(reversePolishSortedTokens);
    return (integer) result;
}


bool test_tokenizer(){
    auto tokensList = Token::Tokenise("333 + 4444 * ( 22- 1 )");
    std::vector<Token> tokens{ begin(tokensList), end(tokensList) };

    if(!tokens[0].isNumber()) return false;
    if(tokens[0].lenght() !=3) return false;
    if(!tokens[1].isOperator()) return false;
    if(tokens[1].lenght() !=1) return false;

    //todo: more test cases
    return true;
}

bool test_reverse_polish_sort(){
    auto x = Token::Tokenise("333 + 4444 * ( 22- 1 )");
    auto res = sort_reverse_polish(x); // expected 333 4444 22 1 - * +

    //todo compare against known values of res
    return true;
}

bool test_evaluate(){
    //provided examples
    if(evaluate("(222* 4 )/ 7-(3 * (4 * 5))") != 66) return false;
    if(evaluate("1 + 2 * ( 3 -2 * (5 - 1))") !=-9) return false;
    //additional tests

    if(evaluate("-1*(10-11)*(5*((5+5)-3)/(1+1))*2") != 35) return false;
    if(evaluate("333 + 4444 * ( 22- 1 )") != 93657) return false;
    if(evaluate("   ") != 0) return false; //edge case
    if(evaluate("  -1") != -1) return false;
    //todo: more tests
    return true;
}