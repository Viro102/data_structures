#! /bin/bash

valgrind --leak-check=yes --show-leak-kinds=all ./build/cli/main
