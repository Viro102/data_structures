#! /usr/bin/env bash

valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind_output.txt ./build/cli/main
