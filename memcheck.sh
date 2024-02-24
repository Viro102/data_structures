#! /bin/bash

valgrind --leak-check=full --show-leak-kinds=all ./build/cli/main
