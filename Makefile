# © 2021 Vlad-Stefan Harbuz <vlad@vladh.net>
# SPDX-License-Identifier: blessing

.PHONY: test run

test:
	mkdir -p bin && gcc pstr_test.c -o bin/pstr_test -g -Wall -Werror -std=c99

run-test: test
	./bin/pstr_test
