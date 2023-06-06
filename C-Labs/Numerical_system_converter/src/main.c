#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct Number {
    int64_t intPart;
    int64_t numerator;
    int64_t denominator;
} Number;

int BadInput() {
    printf("bad input");
    exit(0);
}

void ValidateBase(int n) {
    if (n < 2 || n > 16){
        BadInput();
    }
}

void ValidateDigit(int digit, int base) {
    if (digit >= base) {
        BadInput();
    }
}

void ReverseString(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len/2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int CharToDigit(int chr) {
    if (isdigit(chr)) {
        return chr - '0';
    }
    if (isalpha(chr)) {
        chr = tolower(chr);
        return 10 + chr - 'a';
    }
    return BadInput();
}

int DigitToChar(int digit) {
    if (0 <= digit && digit <= 9) {
        return '0' + digit;
    }
    if (digit >= 10) {
        return 'a' + digit - 10;
    }
    return BadInput();
}

Number ParseString(char* str, int originalBase) {
    Number num = {0, 0, 1};
    char *ptr = &str[0];

    if (*ptr == '.') {
        // If dot is in the beginning
        BadInput();
    }

    // Parse integer part
    while (*ptr != '.' && *ptr != '\0') {
        int digit = CharToDigit(*ptr);
        ValidateDigit(digit, originalBase);
        num.intPart = num.intPart * originalBase + digit;
        ++ptr;
    }

    // Parse fractional part if integer ended by '.'
    if (*ptr == '.') {
        ++ptr;
        if (*ptr == '\0') {
            // If there is no digits
            BadInput();
        }
        while (*ptr != '\0') {
            int digit = CharToDigit(*ptr);
            ValidateDigit(digit, originalBase);
            num.numerator = num.numerator * originalBase + digit;
            num.denominator *= originalBase;
            ++ptr;
        }
    }
    return num;
}

void Convert(Number num, int targetBase, char* output) {
    char* ptr = &output[0];

    // Convert integer part
    // Need to write '0' if intPart is 0
    do {
        *ptr = DigitToChar(num.intPart % targetBase);
        num.intPart /= targetBase;
        ++ptr;
    } while (num.intPart > 0);

    ReverseString(output);

    // Convert fractional part
    if (num.numerator > 0) {
        *ptr = '.';
        int fractionalDigits = 0;
        while (num.numerator > 0 && fractionalDigits < 12) {
            ++ptr;
            ++fractionalDigits;
            num.numerator *= targetBase;
            *ptr = DigitToChar(num.numerator / num.denominator);
            num.numerator %= num.denominator;
        }
    }
}

int main(void) {
    int originalBase;
    int targetBase;
    char input[14];

    if (2 != scanf("%i %i", &originalBase, &targetBase)) {
        BadInput();
    }
    ValidateBase(originalBase);
    ValidateBase(targetBase);

    if (1 != scanf("%13s", input)) {
        BadInput();
    }

    char output[60] = {0};
    Number num = ParseString(input, originalBase);
    Convert(num, targetBase, output);

    printf("%s\n", output);
    return EXIT_SUCCESS;
}
