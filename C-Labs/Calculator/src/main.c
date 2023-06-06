#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "stack.h"

#define SYNTAX_ERROR "syntax error"

enum ElementType {
    Operand,
    Operator
};

void ExitWithMessage(const char* msg) {
    printf("%s\n", msg);
    exit(0);
}

int CharToDigit(char ch) {
    assert('0' <= ch && ch <= '9');
    return ch - '0';
}

void ApplyOperator(Stack operandsStack, char operator) {
    assert(strchr("*/+-", operator));

    // Get operands from stack. If stack is empty, expression is invalid
    if (IsEmpty(operandsStack)) ExitWithMessage(SYNTAX_ERROR);
    int o2 = Pop(operandsStack);
    if (IsEmpty(operandsStack)) ExitWithMessage(SYNTAX_ERROR);
    int o1 = Pop(operandsStack);

    switch (operator) {
    case '+':
        Push(operandsStack, o1 + o2);
        break;
    case '-':
        Push(operandsStack, o1 - o2);
        break;
    case '*':
        Push(operandsStack, o1 * o2);
        break;
    case '/':
        if (o2 == 0) {
            ExitWithMessage("division by zero");
        }
        Push(operandsStack, o1 / o2);
        break;
    }
}

void ApplyTopOperator(Stack operandsStack, Stack operatorsStack, const char* typesToApply) {
    // applies one operator from stack if it is present in typesToApply string
    if (!IsEmpty(operatorsStack)) {
        if (strchr(typesToApply, GetTop(operatorsStack))) {
            ApplyOperator(operandsStack, Pop(operatorsStack));
        }
    }
}

int getNext(const char** ptr, int* value, enum ElementType* type) {
    if (!**ptr) return 0; // end of expression reached

    if (**ptr == '(' && *(*ptr + 1) == ')') { // check "()" situation
        ExitWithMessage(SYNTAX_ERROR);
    }

    if (isdigit(**ptr)) {
        // if digit is found, parse all following digits and return a number
        *value = 0;
        do {
            *value = *value * 10 + CharToDigit(**ptr);
            ++(*ptr);
        } while (isdigit(**ptr));

        *type = Operand;
        return 1;
    }

    if (strchr("+-*/()", **ptr)) {
        *value = **ptr;
        ++(*ptr);
        *type = Operator;
        return 1;
    }

    ExitWithMessage(SYNTAX_ERROR);
    return 0;
}

void ApplyBrackets(Stack operands, Stack operators) {
        while (!IsEmpty(operators)) {
            char op = Pop(operators);
            if (op == '(') return;
            ApplyOperator(operands, op);
        }
        // syntax error is opening bracket was not found
        ExitWithMessage(SYNTAX_ERROR);
}

int CalculateExpression(const char* expr) {
    Stack operatorsStack = MakeStack();
    Stack operandsStack = MakeStack();

    int value;
    enum ElementType type;

    while (getNext(&expr, &value, &type)) {
        if (type == Operand) {
            Push(operandsStack, value);
        }
        else if (type == Operator) {
            if (strchr("*/", value)) {
                // apply previous operator, only same or higher priority
                ApplyTopOperator(operandsStack, operatorsStack, "*/");
                Push(operatorsStack, value);
            }
            else if (strchr("+-", value)) {
                // apply previous operator, only same or higher priority
                ApplyTopOperator(operandsStack, operatorsStack, "+-*/");
                Push(operatorsStack, value);
            }
            else if (value == '(') {
                Push(operatorsStack, value);
            }
            else if (value == ')') {
                ApplyBrackets(operandsStack, operatorsStack);
            }
        }
    }

    // Apply remainded operators
    while (!IsEmpty(operatorsStack)) {
        char op = Pop(operatorsStack);
        if (!strchr("+-/*", op)) ExitWithMessage(SYNTAX_ERROR);
        ApplyOperator(operandsStack, op);
    }

    if (IsEmpty(operandsStack)) ExitWithMessage(SYNTAX_ERROR);
    int result = Pop(operandsStack);

    DestroyStack(operandsStack);
    DestroyStack(operatorsStack);
    return result;
}

int main(void) {
    char input[1000 + 1];
    int status = scanf("%1000[^\n]s", input);
    if (status == 0) {
        ExitWithMessage(SYNTAX_ERROR);
    }

    printf("%i\n", CalculateExpression(input));
    return EXIT_SUCCESS;
}
