#!/bin/bash
set -xe
clang -Wall -Wextra -Werror -std=c99 -pedantic -o bfi bfi.c