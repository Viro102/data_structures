#! /bin/bash

valgrind --leak-check=full --show-leak-kinds=all --log-file=bozo.txt ./build/cli/main
