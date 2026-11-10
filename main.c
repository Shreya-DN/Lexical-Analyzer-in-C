/*
Project Name : Lexical Analyzer

 * Description  :
 *This project implements a Lexical Analyzer using C. It reads a C source code file, identifies individual tokens,
 and classifies them into different categories.
 
 * Token Categories:
  1. Keywords
  2. Identifiers
  3. Operators
  4. Special Symbols
  5. Integer Constants
  6. Floating-Point Constants
  7. Character Constants
  8. String Literals
  9. Preprocessor Directives
 
 * Input : C source code file (input.c)
 
 * Expected Output:
 *Displays each recognized token along with its line number and token category.
 
 * Example:
Line 1: #include <stdio.h> -> Preprocessor Directive
Line 2: #define SIZE 100 -> Preprocessor Directive
Line 4: int -> Keyword
Line 4: main -> Identifier
Line 4: ( -> Special Symbol
Line 4: ) -> Special Symbol
Line 5: { -> Special Symbol
Line 7: int -> Keyword
Line 7: decimal -> Identifier
Line 7: = -> Operator
Line 7: 25 -> Integer Constant
Line 7: ; -> Special Symbol
Line 8: int -> Keyword
Line 8: octal -> Identifier
Line 8: = -> Operator
Line 8: 075 -> Integer Constant
Line 8: ; -> Special Symbol
Line 9: int -> Keyword
Line 9: hexadecimal -> Identifier
Line 9: = -> Operator
Line 9: 0x1A -> Integer Constant
Line 9: ; -> Special Symbol
Line 10: int -> Keyword
Line 10: suffix -> Identifier
Line 10: = -> Operator
Line 10: 100ULL -> Integer Constant
Line 10: ; -> Special Symbol
Line 13: float -> Keyword
Line 13: a -> Identifier
Line 13: = -> Operator
Line 13: 12.5f -> Floating-point Constant
Line 13: ; -> Special Symbol
Line 14: double -> Keyword
Line 14: b -> Identifier
Line 14: = -> Operator
Line 14: 1.5e3 -> Floating-point Constant
Line 14: ; -> Special Symbol
Line 15: double -> Keyword
Line 15: c -> Identifier
Line 15: = -> Operator
Line 15: 0x1.fp3 -> Floating-point Constant
Line 15: ; -> Special Symbol
Line 18: char -> Keyword
Line 18: normal -> Identifier
Line 18: = -> Operator
Line 18: 'A' -> Character Constant
Line 18: ; -> Special Symbol
Line 19: wchar_t -> Identifier
Line 19: wide_char -> Identifier
Line 19: = -> Operator
Line 19: L'A' -> Character Constant
Line 19: ; -> Special Symbol
Line 20: char -> Keyword
Line 20: * -> Operator
Line 20: wide_string -> Identifier
Line 20: = -> Operator
Line 20: L"Hello" -> String Literal
Line 20: ; -> Special Symbol
Line 21: char -> Keyword
Line 21: * -> Operator
Line 21: unicode_string -> Identifier
Line 21: = -> Operator
Line 21: u8"Hello" -> String Literal
Line 21: ; -> Special Symbol
Line 24: int -> Keyword
Line 24: x -> Identifier
Line 24: = -> Operator
Line 24: 10 -> Integer Constant
Line 24: ; -> Special Symbol
Line 25: x -> Identifier
Line 25: ++ -> Operator
Line 25: ; -> Special Symbol
Line 26: x -> Identifier
Line 26: += -> Operator
Line 26: 5 -> Integer Constant
Line 26: ; -> Special Symbol
Line 27: x -> Identifier
Line 27: <<= -> Operator
Line 27: 1 -> Integer Constant
Line 27: ; -> Special Symbol
Line 28: x -> Identifier
Line 28: >>= -> Operator
Line 28: 1 -> Integer Constant
Line 28: ; -> Special Symbol
Line 30: if -> Keyword
Line 30: ( -> Special Symbol
Line 30: x -> Identifier
Line 30: >= -> Operator
Line 30: 10 -> Integer Constant
Line 30: && -> Operator
Line 30: x -> Identifier
Line 30: != -> Operator
Line 30: 0 -> Integer Constant
Line 30: ) -> Special Symbol
Line 31: { -> Special Symbol
Line 32: printf -> Identifier
Line 32: ( -> Special Symbol
Line 32: "Valid token testing\n" -> String Literal
Line 32: ) -> Special Symbol
Line 32: ; -> Special Symbol
Line 33: } -> Special Symbol
Line 35: return -> Keyword
Line 35: 0 -> Integer Constant
Line 35: ; -> Special Symbol
Line 36: } -> Special Symbol
Line 39: void -> Keyword
Line 39: display -> Identifier
Line 39: ( -> Special Symbol
Line 39: int -> Keyword
Line 39: count -> Identifier
Line 39: , -> Special Symbol
Line 39: ... -> Special Symbol
Line 39: ) -> Special Symbol
Line 40: { -> Special Symbol
Line 41: printf -> Identifier
Line 41: ( -> Special Symbol
Line 41: "Testing ellipsis" -> String Literal
Line 41: ) -> Special Symbol
Line 41: ; -> Special Symbol
Line 42: } -> Special Symbol
 
 */

#include "lexer.h"

int main()
{
    lexer("input.c");

    return 0;
}