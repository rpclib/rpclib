#!/bin/sh
find ./src -name '*.cc' | xargs clang-check -analyze -p .
find ./tests -name '*.cc' | xargs clang-check -analyze -p .
