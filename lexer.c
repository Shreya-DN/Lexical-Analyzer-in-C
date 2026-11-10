
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

#define MAX_TOKEN 100

// Check whether a token is a C keyword
int is_keyword(const char *token)
{
    const char *keywords[] = {
        "auto", "break", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if",
        "int", "long", "register", "return",
        "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while",
        "_Alignas", "_Alignof", "_Atomic",
        "_Bool", "_Complex", "_Generic",
        "_Imaginary", "_Noreturn", "_Static_assert",
        "_Thread_local"
    };

    int i;
    int count = sizeof(keywords) / sizeof(keywords[0]);

    for (i = 0; i < count; i++)
    {
        if (strcmp(token, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

// Check whether a character is an operator
int is_operator_char(int ch)
{
    return (ch == '+' || ch == '-' || ch == '*' ||
            ch == '/' || ch == '%' || ch == '=' ||
            ch == '<' || ch == '>' || ch == '!' ||
            ch == '&' || ch == '|' || ch == '^' ||
            ch == '~');
}

// Check whether a character is a special symbol
int is_special_symbol(int ch)
{
    return (ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '{' || ch == '}' ||ch == '[' || ch == ']' ||ch == '.' || ch == ':' || ch == '?');
}

// Convert a hexadecimal character into its numeric value
int hex_value(int ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;

    return -1;
}

// Count digits belonging to a particular number base
int count_digits(const char **p, int base)
{
    int count = 0;
    int value;

    while (**p)
    {
        if (base == 16)
            value = hex_value((unsigned char)**p);
        else
            value = isdigit((unsigned char)**p) ? **p - '0' : -1;

        if (value < 0 || value >= base)
            break;

        count++;
        (*p)++;
    }

    return count;
}

// Validate integer suffixes such as U, L, UL and ULL
int valid_integer_suffix(const char *suffix)
{
    char temp[4];
    int i;
    int len = strlen(suffix);

    if (len > 3)
        return 0;

    for (i = 0; i < len; i++)
        temp[i] = (char)tolower((unsigned char)suffix[i]);

    temp[len] = '\0';

    return (
        strcmp(temp, "") == 0 ||
        strcmp(temp, "u") == 0 ||
        strcmp(temp, "l") == 0 ||
        strcmp(temp, "ll") == 0 ||
        strcmp(temp, "ul") == 0 ||
        strcmp(temp, "lu") == 0 ||
        strcmp(temp, "ull") == 0 ||
        strcmp(temp, "llu") == 0
    );
}

// Validate integer and floating-point constants
int is_valid_number(const char *token, int *is_float)
{
    const char *p = token;
    int before = 0;
    int after = 0;
    int exponent_digits = 0;
    int has_dot = 0;
    int has_exponent = 0;

    *is_float = 0;

    // Check hexadecimal constants
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
    {
        p += 2;

        before = count_digits(&p, 16);

        if (*p == '.')
        {
            has_dot = 1;
            p++;
            after = count_digits(&p, 16);
        }

        if (before + after == 0)
            return 0;

        // Check hexadecimal floating-point exponent
        if (*p == 'p' || *p == 'P')
        {
            has_exponent = 1;
            p++;

            if (*p == '+' || *p == '-')
                p++;

            exponent_digits = count_digits(&p, 10);

            if (exponent_digits == 0)
                return 0;

            if (*p == 'f' || *p == 'F' ||*p == 'l' || *p == 'L')
            {
                p++;
            }

            *is_float = 1;
            return *p == '\0';
        }

        // Hexadecimal numbers without an exponent are integers
        if (has_dot)
            return 0;

        return valid_integer_suffix(p);
    }

    // Check decimal and octal constants
    before = count_digits(&p, 10);

    if (*p == '.')
    {
        has_dot = 1;
        p++;
        after = count_digits(&p, 10);
    }

    if (before + after == 0)
        return 0;

    // Check decimal exponent
    if (*p == 'e' || *p == 'E')
    {
        has_exponent = 1;
        p++;

        if (*p == '+' || *p == '-')
            p++;

        exponent_digits = count_digits(&p, 10);

        if (exponent_digits == 0)
            return 0;
    }

    // Validate floating-point constants
    if (has_dot || has_exponent)
    {
        if (*p == 'f' || *p == 'F' ||*p == 'l' || *p == 'L')
        {
            p++;
        }

        *is_float = 1;
        return *p == '\0';
    }

    // Check octal constants beginning with zero
    if (token[0] == '0' && strlen(token) > 1)
    {
        const char *q = token;

        while (*q >= '0' && *q <= '9')
        {
            if (*q == '8' || *q == '9')
                return 0;

            q++;
        }

        return valid_integer_suffix(q);
    }

    return valid_integer_suffix(p);
}

// Validate escape sequences in character constants
int valid_character_body(const char *token, int prefix_len)
{
    int i = prefix_len + 1;
    int end = strlen(token) - 1;

    // Reject empty character constants
    if (i >= end)
        return 0;

    // Check escape sequences
    if (token[i] == '\\')
    {
        i++;

        if (i >= end)
            return 0;

        // Check standard escape characters
        if (strchr("abfnrtv\\'\"?", token[i]) != NULL)
        {
            i++;
        }
        // Check hexadecimal escape sequences
        else if (token[i] == 'x')
        {
            i++;

            if (i >= end || hex_value(token[i]) < 0)
                return 0;

            while (i < end && hex_value(token[i]) >= 0)
                i++;
        }
        // Check octal escape sequences
        else if (token[i] >= '0' && token[i] <= '7')
        {
            int count = 0;

            while (i < end &&
                   token[i] >= '0' && token[i] <= '7' && count < 3)
            {
                i++;
                count++;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        // Accept one normal character
        i++;
    }

    return i == end;
}

// Check whether an identifier is a literal prefix
int is_literal_prefix(const char *token)
{
    return (
        strcmp(token, "L") == 0 ||
        strcmp(token, "u") == 0 ||
        strcmp(token, "U") == 0 ||
        strcmp(token, "u8") == 0
    );
}

// Check three-character operators
int is_three_char_operator(int a, int b, int c)
{
    return (
        (a == '<' && b == '<' && c == '=') ||
        (a == '>' && b == '>' && c == '=') ||
        (a == '.' && b == '.' && c == '.')
    );
}

// Check two-character operators
int is_two_char_operator(int a, int b)
{
    return (
        (a == '+' && (b == '+' || b == '=')) ||
        (a == '-' && (b == '-' || b == '=' || b == '>')) ||
        (a == '*' && b == '=') ||
        (a == '/' && b == '=') ||
        (a == '%' && b == '=') ||
        (a == '=' && b == '=') ||
        (a == '!' && b == '=') ||
        (a == '<' && (b == '<' || b == '=')) ||
        (a == '>' && (b == '>' || b == '=')) ||
        (a == '&' && (b == '&' || b == '=')) ||
        (a == '|' && (b == '|' || b == '=')) ||
        (a == '^' && b == '=') ||
        (a == '#' && b == '#')
    );
}

// Read and classify a string or character literal
void read_literal(FILE *fp, int quote, const char *prefix, int *line, int start_line)
{
    char token[MAX_TOKEN];
    int i = 0;
    int ch;
    int escaped = 0;
    int closed = 0;
    int too_long = 0;
    int prefix_len = strlen(prefix);

    // Copy the literal prefix into the token
    while (prefix[i] && i < MAX_TOKEN - 1)
    {
        token[i] = prefix[i];
        i++;
    }

    if (i < MAX_TOKEN - 1)
        token[i++] = quote;

    // Read characters until the closing quote or end of line
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n' || ch == '\r')
        {
            if (ch == '\r')
            {
                int next = fgetc(fp);

                if (next != '\n' && next != EOF)
                    ungetc(next, fp);
            }

            (*line)++;
            break;
        }

        if (i < MAX_TOKEN - 1)
            token[i++] = ch;
        else
            too_long = 1;

        if (ch == quote && !escaped)
        {
            closed = 1;
            break;
        }

        if (ch == '\\' && !escaped)
            escaped = 1;
        else
            escaped = 0;
    }

    token[i] = '\0';

    // Display an error if the literal is not closed
    if (!closed)
    {
        printf("Line %d: Error - Unterminated String/Character Constant\n", start_line);
    }
    else if (too_long)
    {
        printf("Line %d: Error - Constant Too Long\n", start_line);
    }
    else if (quote == '"')
    {
        printf("Line %d: %s -> String Literal\n", start_line, token);
    }
    else if (valid_character_body(token, prefix_len))
    {
        printf("Line %d: %s -> Character Constant\n",start_line, token);
    }
    else
    {
        printf("Line %d: Error - Invalid Character Constant %s\n", start_line, token);
    }
}

// Read and classify numeric constants
void read_number(FILE *fp, int first, int *line)
{
    char token[MAX_TOKEN];
    int i = 0;
    int ch = first;
    int last = 0;
    int too_long = 0;
    int is_float = 0;

    // Collect all characters belonging to the number
    while (isalnum((unsigned char)ch) || ch == '_' || ch == '.' || ((ch == '+' || ch == '-') &&(last == 'e' || last == 'E' || last == 'p' || last == 'P')))
    {
        if (i < MAX_TOKEN - 1)
            token[i++] = ch;
        else
            too_long = 1;

        last = ch;
        ch = fgetc(fp);
    }

    token[i] = '\0';

    if (ch != EOF)
        ungetc(ch, fp);

    // Check token length and validate the number
    if (too_long)
    {
        printf("Line %d: Error - Numeric Constant Too Long\n", *line);
    }
    else if (is_valid_number(token, &is_float))
    {
        if (is_float)
            printf("Line %d: %s -> Floating-point Constant\n",*line, token);
        else
            printf("Line %d: %s -> Integer Constant\n", *line, token);
    }
    else
    {
        printf("Line %d: Error - Invalid Numeric Constant %s\n",*line, token);
    }
}

// Read and classify an identifier or keyword
void read_identifier(FILE *fp, int first, int *line)
{
    char token[MAX_TOKEN];
    int i = 0;
    int ch = first;
    int too_long = 0;
    int start_line = *line;

    // Collect identifier characters
    while (isalnum((unsigned char)ch) || ch == '_')
    {
        if (i < MAX_TOKEN - 1)
            token[i++] = ch;
        else
            too_long = 1;

        ch = fgetc(fp);
    }

    token[i] = '\0';

    // Check for wide and Unicode literal prefixes
    if (!too_long && is_literal_prefix(token) &&
        (ch == '"' || ch == '\''))
    {
        read_literal(fp, ch, token, line, start_line);
        return;
    }

    if (ch != EOF)
        ungetc(ch, fp);

    // Classify the token
    if (too_long)
    {
        printf("Line %d: Error - Identifier Too Long\n", start_line);
    }
    else if (is_keyword(token))
    {
        printf("Line %d: %s -> Keyword\n", start_line, token);
    }
    else
    {
        printf("Line %d: %s -> Identifier\n", start_line, token);
    }
}

// Main lexical analyzer function
void lexer(const char *filename)
{
    FILE *fp;
    int ch, next, third;
    int line = 1;
    int directive_line;
    int i, too_long;

    char token[MAX_TOKEN];

    // Open the source file
    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    // Read the source file character by character
    while ((ch = fgetc(fp)) != EOF)
    {
        // Identify preprocessor directives
        if (ch == '#')
        {
            directive_line = line;
            i = 0;
            too_long = 0;

            token[i++] = '#';

            while ((ch = fgetc(fp)) != EOF)
            {
                if (ch == '\n' || ch == '\r')
                {
                    if (ch == '\r')
                    {
                        next = fgetc(fp);

                        if (next != '\n' && next != EOF)
                            ungetc(next, fp);
                    }

                    line++;

                    // Continue the directive if the line ends with backslash
                    if (i > 0 && token[i - 1] == '\\')
                    {
                        i--;

                        if (i < MAX_TOKEN - 1)
                            token[i++] = ' ';
                        else
                            too_long = 1;

                        continue;
                    }

                    break;
                }

                if (i < MAX_TOKEN - 1)
                    token[i++] = ch;
                else
                    too_long = 1;
            }

            token[i] = '\0';

            if (too_long)
                printf("Line %d: Error - Preprocessor Directive Too Long\n", directive_line);
            else
                printf("Line %d: %s -> Preprocessor Directive\n", directive_line, token);

            continue;
        }

        // Ignore whitespace characters
        if (isspace((unsigned char)ch))
        {
            if (ch == '\n')
                line++;

            continue;
        }

        // Read string and character literals
        if (ch == '"' || ch == '\'')
        {
            read_literal(fp, ch, "", &line, line);
            continue;
        }

        // Read integer and floating-point constants
        if (isdigit((unsigned char)ch))
        {
            read_number(fp, ch, &line);
            continue;
        }

        // Handle floating-point numbers and ellipsis
        if (ch == '.')
        {
            next = fgetc(fp);

            // Check whether the token is an ellipsis
            if (next == '.')
            {
                third = fgetc(fp);

                if (third == '.')
                {
                    printf("Line %d: ... -> Special Symbol\n", line);
                    continue;
                }

                // Put back characters when it is not an ellipsis
                if (third != EOF)
                    ungetc(third, fp);

                ungetc(next, fp);
            }
            else if (isdigit(next))
            {
                // Read floating-point numbers such as .56
                ungetc(next, fp);
                read_number(fp, ch, &line);
                continue;
            }
            else if (next != EOF)
            {
                ungetc(next, fp);
            }
        }

        // Read keywords and identifiers
        if (isalpha((unsigned char)ch) || ch == '_')
        {
            read_identifier(fp, ch, &line);
            continue;
        }

        // Identify operators and comments
        if (is_operator_char(ch))
        {
            next = fgetc(fp);

            // Ignore single-line comments
            if (ch == '/' && next == '/')
            {
                while ((ch = fgetc(fp)) != EOF && ch != '\n')
                {
                    // Skip comment characters
                }

                if (ch == '\n')
                    line++;

                continue;
            }

            // Ignore multi-line comments
            if (ch == '/' && next == '*')
            {
                int prev = 0;
                int closed = 0;
                int comment_line = line;

                while ((ch = fgetc(fp)) != EOF)
                {
                    if (ch == '\n')
                        line++;

                    if (prev == '*' && ch == '/')
                    {
                        closed = 1;
                        break;
                    }

                    prev = ch;
                }

                if (!closed)
                {
                    printf("Line %d: Error - Unterminated Multi-line Comment\n",
                           comment_line);
                }

                continue;
            }

            third = fgetc(fp);

            // Check three-character operators first
            if (is_three_char_operator(ch, next, third))
            {
                if (ch == '.')
                    printf("Line %d: ... -> Special Symbol\n", line);
                else
                    printf("Line %d: %c%c%c -> Operator\n",
                           line, ch, next, third);

                continue;
            }

            // Check two-character operators
            if (is_two_char_operator(ch, next))
            {
                printf("Line %d: %c%c -> Operator\n",
                       line, ch, next);

                if (third != EOF)
                    ungetc(third, fp);

                continue;
            }

            // Process one-character operators
            printf("Line %d: %c -> Operator\n", line, ch);

            if (third != EOF)
                ungetc(third, fp);

            if (next != EOF)
                ungetc(next, fp);

            continue;
        }

        // Identify special symbols
        if (is_special_symbol(ch))
        {
            printf("Line %d: %c -> Special Symbol\n", line, ch);
            continue;
        }

        // Report characters that do not belong to known categories
        printf("Line %d: %c -> Unknown Character\n", line, ch);
    }

    // Close the source file
    fclose(fp);
}
