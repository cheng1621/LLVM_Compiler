./main "$@"
clang output.s runtime.o -o a.out -lm