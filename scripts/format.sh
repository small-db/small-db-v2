# run clang-format on all .cc and .h files
find . -name '*.cc' -o -name '*.h' | xargs clang-format -i
