#ifndef NETPARSE_EVAL_H
#define NETPARSE_EVAL_H

#include <cstring>
#include "tokens.h"
// type defined for flexibility
typedef long long integer;

bool test_tokenizer();
bool test_reverse_polish_sort();
bool test_evaluate();

integer evaluate(string expression);

#endif //NETPARSE_EVAL_H
