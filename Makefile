.PHONY: test run

test:
	mkdir -p bin && gcc pstr_test.c -o bin/pstr_test -g -Wall -Werror

run-test: test
	./bin/pstr_test
