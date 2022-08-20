#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define UNUSED(x) (void)(x)
#define TODO(x) fprintf(stderr, "NOT IMPLEMENTED: %s\n", x)
#define UNREACHABLE() assert(!"UNREACHABLE")
#define TRACE(x) fprintf(stderr, x)

#define READ_MAX_LEN (1 << 20)

char* readfile(FILE *f) {
    if (f == NULL || fseek(f, 0, SEEK_END)) {
        return NULL;
    }
    long length = ftell(f);
    rewind(f);

    if (length == -1 || (unsigned long) length >= READ_MAX_LEN) {
        return NULL;
    }
    size_t ulength = (size_t) length;
    char *buffer = (char*) malloc(ulength + sizeof(char));
    buffer[ulength] = '\0';

    if (buffer == NULL) {
        return NULL;
    }
    size_t readlen = fread(buffer, sizeof(char), ulength, f);

    if (readlen != ulength) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

#define BF_TAPE_SIZE 30000
static char BF_TAPE[BF_TAPE_SIZE] = {0};

#define BF_SUCCESS 0
#define BF_ERROR_OVERFLOW 1
#define BF_ERROR_UNDERFLOW 2
#define BF_ERROR_UNBALANCED_BRACKETS 3

int run_program(char *program) {
    size_t program_len = strlen(program);
    size_t pc = 0;
    size_t tape_ptr = 0;
    int bracket_depth = 0;
    while (pc < program_len) {
        char inst = program[pc];
        switch (inst) {
            case '>':
                if (tape_ptr == BF_TAPE_SIZE - 1) {
                    return BF_ERROR_OVERFLOW;
                }
                tape_ptr++;
                pc++;
                break;
            case '<':
                if (tape_ptr == 0) {
                    return BF_ERROR_UNDERFLOW;
                }
                tape_ptr--;
                pc++;
                break;
            case '+':
                BF_TAPE[tape_ptr]++;
                pc++;
                break;
            case '-':
                BF_TAPE[tape_ptr]--;
                pc++;
                break;
            case '.':
                putchar(BF_TAPE[tape_ptr]);
                pc++;
                break;
            case ',':
                BF_TAPE[tape_ptr] = getchar();
                pc++;
                break;
            case '[':
                if (BF_TAPE[tape_ptr] != 0) {
                    pc++;
                    break;
                }
                bracket_depth = 1;
                while (bracket_depth > 0) {
                    pc++;
                    if (pc == program_len) {
                        return BF_ERROR_UNBALANCED_BRACKETS;
                    }
                    if (program[pc] == '[')      bracket_depth++;
                    else if (program[pc] == ']') bracket_depth--;
                }
                break;
            case ']':
                if (BF_TAPE[tape_ptr] == 0) {
                    pc++;
                    break;
                }
                bracket_depth = 1;
                while (bracket_depth > 0) {
                    pc--;
                    if (pc == 0) {
                        return BF_ERROR_UNBALANCED_BRACKETS;
                    }
                    if (program[pc] == ']')      bracket_depth++;
                    else if (program[pc] == '[') bracket_depth--;
                }
                
            default:
                pc++;
                break;
        }
    }
    return BF_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    FILE *fp = NULL;
    fopen_s(&fp, filename, "rb");

    if (fp == NULL) {
        fprintf(stderr, "Error: Reading file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char *program = readfile(fp);
    fclose(fp);

    if (program == NULL) {
        fprintf(stderr, "Error: Reading file contents from '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    errno_t ret = run_program(program);
    errno_t exit_code = EXIT_SUCCESS;
    switch (ret) {
        case BF_SUCCESS:
            break;
        case BF_ERROR_OVERFLOW:
            fprintf(stderr, "Error: Overflow\n");
            break;
        case BF_ERROR_UNDERFLOW:
            fprintf(stderr, "Error: Underflow\n");
            break;
        case BF_ERROR_UNBALANCED_BRACKETS:
            fprintf(stderr, "Error: Unbalanced brackets\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error\n");
            break;
    }

    if (ret != BF_SUCCESS) {
        exit_code = EXIT_FAILURE;
    }

    exit(exit_code);
}
