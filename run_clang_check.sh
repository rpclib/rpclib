#!/bin/sh
find ./lib -name '*.cc' | xargs clang-check-3.7 -analyze -p .
find ./tests -name '*.cc' | xargs clang-check-3.7 -analyze -p .
