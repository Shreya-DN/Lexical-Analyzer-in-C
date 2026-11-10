#include <stdio.h>
#define SIZE 100

int main()
{
    // Integer constants
    int decimal = 25;
    int octal = 075;
    int hexadecimal = 0x1A;
    int suffix = 100ULL;

    // Floating-point constants
    float a = 12.5f;
    double b = 1.5e3;
    double c = 0x1.fp3;

    // Character and string literals
    char normal = 'A';
    wchar_t wide_char = L'A';
    char *wide_string = L"Hello";
    char *unicode_string = u8"Hello";

    // Operators
    int x = 10;
    x++;
    x += 5;
    x <<= 1;
    x >>= 1;

    if (x >= 10 && x != 0)
    {
        printf("Valid token testing\n");
    }

    return 0;
}

// Function with ellipsis
void display(int count, ...)
{
    printf("Testing ellipsis");
}
