// © 2021 Vlad-Stefan Harbuz <vlad@vladh.net>
// SPDX-License-Identifier: blessing

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "pstr.h"

#include "pstr.c"


static uint32_t n_tests_total = 0;
static uint32_t n_tests_passed = 0;
static uint32_t n_tests_failed = 0;


static void print_test_group(char const *name) {
  printf("\n%s\n", name);
  printf("--------------------\n");
}


static void run_test(char const *message, bool const result) {
  n_tests_total++;
  if (result) {
    n_tests_passed++;
    printf("✓ pass | %s\n", message);
  } else {
    n_tests_failed++;
    printf("× FAIL | %s\n", message);
  }
}


static void print_test_statistics() {
  printf("\n");
  printf("Total tests: %d\n", n_tests_total);
  printf("Passed: %d\n", n_tests_passed);
  printf("Failed: %d\n", n_tests_failed);
}


static void test_pstr_is_valid() {
  size_t const size = 6;
  char const valid[] = {'h', 'e', 'l', 'l', 'o', 0};
  char const valid_shorter[] = {'h', 'e', 0};
  char const invalid[] = {'h', 'e', 'l', 'l', 'o', 0xcc, 0xcc, 0xcc};

  print_test_group("pstr_is_valid()");
  run_test(
    "Valid string is recognised as such",
    pstr_is_valid(valid, size)
  );
  run_test(
    "Valid (but sligtly shorter) string is recognised as valid",
    pstr_is_valid(valid_shorter, size)
  );
  run_test(
    "Invalid string is recognised as such",
    !pstr_is_valid(invalid, size)
  );
}


static void test_pstr_len() {
  print_test_group("pstr_len()");
  run_test(
    "Length of \"Magpie\" is 6",
    pstr_len("Magpie") == 6
  );
  run_test(
    "Length of \"pie\" is 3",
    pstr_len("pie") == 3
  );
}


static void test_pstr_is_empty() {
  print_test_group("pstr_is_empty()");
  run_test(
    "Empty string is empty",
    pstr_is_empty("")
  );
  run_test(
    "Non-empty string is non-empty",
    !pstr_is_empty("Magpie")
  );
}


static void test_pstr_eq() {
  print_test_group("pstr_eq()");
  run_test(
    "Equal strings are equal",
    pstr_eq("Magpie", "Magpie")
  );
  run_test(
    "Different strings are not equal",
    !pstr_eq("Magpie", "Magpin")
  );
}


static void test_pstr_starts_with_char() {
  print_test_group("pstr_starts_with_char()");
  run_test(
    "\"Magpie\" starts with 'M'",
    pstr_starts_with_char("Magpie", 'M')
  );
  run_test(
    "\"Magpie\" does not start with 'm'",
    !pstr_starts_with_char("Magpie", 'm')
  );
  run_test(
    "\"Magpie\" does not start with '\\0'",
    !pstr_starts_with_char("Magpie", '\0')
  );
}


static void test_pstr_starts_with() {
  print_test_group("pstr_starts_with()");
  run_test(
    "\"Magpie\" starts with \"Mag\"",
    pstr_starts_with("Magpie", "Mag")
  );
  run_test(
    "\"Magpie\" does not start with \"ag\"",
    !pstr_starts_with("Magpie", "ag")
  );
  run_test(
    "\"Magpie\" does not start with \"Mississippi\"",
    !pstr_starts_with("Magpie", "Mississippi")
  );
  run_test(
    "\"Magpie\" does not start with \"\"",
    !pstr_starts_with("Magpie", "")
  );
}


static void test_pstr_ends_with_char() {
  print_test_group("pstr_ends_with_char()");
  run_test(
    "\"Magpie\" ends with 'e'",
    pstr_ends_with_char("Magpie", 'e')
  );
  run_test(
    "\"Magpie\" does not end with 'x'",
    !pstr_ends_with_char("Magpie", 'x')
  );
  run_test(
    "\"Magpie\" does not end with '\\0'",
    !pstr_ends_with_char("Magpie", '\0')
  );
}


static void test_pstr_ends_with() {
  print_test_group("pstr_ends_with()");
  run_test(
    "\"Magpie\" ends with \"pie\"",
    pstr_ends_with("Magpie", "pie")
  );
  run_test(
    "\"Magpie\" does not end with \"pi\"",
    !pstr_ends_with("Magpie", "pi")
  );
  run_test(
    "\"Magpie\" does not end with \"Mississippi\"",
    !pstr_ends_with("Magpie", "Mississippi")
  );
  run_test(
    "\"Magpie\" does not end with \"\\0\"",
    !pstr_ends_with("Magpie", "\\0")
  );
}


static void test_pstr_copy() {
  print_test_group("test_pstr_copy()");
  bool did_succeed;
  size_t const dest_size = 6;
  char dest[dest_size];
  char const src_short[] = {'h', 'e', 'y', 0};
  char const src_snug[] = {'h', 'e', 'l', 'l', 'o', 0};
  char const src_a_bit_long[] = {'h', 'e', 'l', 'l', 'o', '!', 0};
  char const src_long[] = {'h', 'e', 'y', 'y', 'y', 'y', 'y', 'y', 0};

  memset(dest, 0, dest_size);
  did_succeed = pstr_copy(dest, dest_size, src_short);
  run_test(
    "A buffer with enough space receives a short string",
    did_succeed && memcmp(dest, "hey\0\0\0", dest_size) == 0
  );

  memset(dest, 0, dest_size);
  did_succeed = pstr_copy(dest, dest_size, src_snug);
  run_test(
    "A buffer with just enough space receives a snug string",
    did_succeed && memcmp(dest, "hello\0", dest_size) == 0
  );

  memset(dest, 0, dest_size);
  did_succeed = pstr_copy(dest, dest_size, src_a_bit_long);
  run_test(
    "A buffer does not receive a string that is one byte too long to fit",
    !did_succeed && memcmp(dest, "\0\0\0\0\0\0", dest_size) == 0
  );

  memset(dest, 0, dest_size);
  did_succeed = pstr_copy(dest, dest_size, src_long);
  run_test(
    "A buffer does not receive a string that is much too long to fit",
    !did_succeed && memcmp(dest, "\0\0\0\0\0\0", dest_size) == 0
  );
}


static void test_pstr_copy_n() {
  print_test_group("test_pstr_copy_n()");
  bool did_succeed;
  size_t const dest_size = 6;
  char dest[dest_size];
  char const src_short[] = {'h', 'e', 'y', 0};

  memset(dest, 0, dest_size);
  did_succeed = pstr_copy_n(dest, dest_size, src_short, 2);
  run_test(
    "The first two characters are successfully copied",
    did_succeed && memcmp(dest, "he\0", 3) == 0
  );
}


static void test_pstr_cat() {
  print_test_group("test_pstr_cat()");
  bool did_succeed;
  size_t const dest_size = 8;
  char dest[dest_size];
  char const src_short[] = {'n', 't', 0};
  char const src_snug[] = {'t', 'h', 'e', 'r', 'e', 0};
  char const src_a_bit_long[] = {'t', 'h', 'e', 'r', 'e', '!', 0};
  char const src_long[] = {'h', 'e', 'y', 'y', 'y', 'y', 'y', 'y', 0};

  memcpy(dest, "hi\0\0\0\0\0\0", dest_size);
  did_succeed = pstr_cat(dest, dest_size, src_short);
  run_test(
    "A string with enough remaining space receives a string that fits comfortably",
    did_succeed && memcmp(dest, "hint\0\0\0\0", dest_size) == 0
  );

  memcpy(dest, "hi\0\0\0\0\0\0", dest_size);
  did_succeed = pstr_cat(dest, dest_size, src_snug);
  run_test(
    "A string with just enough remaining space receives a string that fits snugly",
    did_succeed && memcmp(dest, "hithere\0", dest_size) == 0
  );

  memcpy(dest, "hi\0\0\0\0\0\0", dest_size);
  did_succeed = pstr_cat(dest, dest_size, src_a_bit_long);
  run_test(
    "A string does not receive a string that is one byte too long to fit in the "
    "remaining space",
    !did_succeed && memcmp(dest, "hi\0\0\0\0\0\0", dest_size) == 0
  );

  memcpy(dest, "hi\0\0\0\0\0\0", dest_size);
  did_succeed = pstr_cat(dest, dest_size, src_long);
  run_test(
    "A string does not receive a string that is much too long to fit in the "
    "remaining space",
    !did_succeed && memcmp(dest, "hi\0\0\0\0\0\0", dest_size) == 0
  );
}


static void test_pstr_vcat() {
  print_test_group("test_pstr_vcat()");
  bool did_succeed;
  size_t const dest_size = 20;
  char dest[dest_size];

  memcpy(dest, "hi\0", 3);
  did_succeed = pstr_vcat(dest, dest_size, " there\0", NULL);
  run_test(
    "A single short string is concatenated successfully",
    did_succeed && memcmp(dest, "hi there\0", 9) == 0
  );

  memcpy(dest, "hi\0", 3);
  did_succeed = pstr_vcat(dest, dest_size, " there\0", " dear", " pal!!", NULL);
  run_test(
    "Multiple short strings are concatenated successfully with or without "
    "NULL terminators",
    did_succeed && memcmp(dest, "hi there dear pal!!\0", 20) == 0
  );

  memcpy(dest, "hi\0", 3);
  did_succeed = pstr_vcat(dest, dest_size, "12345678901234567890", NULL);
  run_test(
    "A string that's too long is not concatenated and the string is unchanged",
    !did_succeed && memcmp(dest, "hi\0", 3) == 0
  );

  memcpy(dest, "hi\0", 3);
  did_succeed = pstr_vcat(dest, dest_size, "12345", "12345", "12345", "12345", NULL);
  run_test(
    "Multiple small strings that add up to too much are not concatenated",
    !did_succeed && memcmp(dest, "hi\0", 3) == 0
  );
}


static void test_pstr_split_on_first_occurrence() {
  print_test_group("test_pstr_split_on_first_occurrence()");
  bool did_succeed;
  size_t part1_size = 6;
  char part1[part1_size];
  size_t part2_size = 7;
  char part2[part2_size];
  char const src_good_fit[] = "hi,thar";
  char const src_snug_fit[] = "hello,there!";
  char const src_sep_at_start[] = ",there!";
  char const src_sep_at_end[] = "hello,";
  char const src_double_sep[] = "hello,,there";
  char const src_no_sep[] = "hello";
  char const src_empty[] = "";
  char const src_part1_too_long[] = "hello!,there!";
  char const src_part2_too_long[] = "hello,there!!";
  char const src_part1_much_too_long[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx,there!";
  char const src_part2_much_too_long[] = "hello,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_good_fit, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "A string is split when there is enough space",
    did_succeed && memcmp(part1, "hi\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "thar\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_snug_fit, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "A string is split when there is just enough space",
    did_succeed && memcmp(part1, "hello\0", part1_size) == 0 &&
      memcmp(part2, "there!\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_sep_at_start, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the separator is at the start, the first part is empty and the second part is "
    "correctly copied",
    did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "there!\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_sep_at_end, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the separator is at the end, the second part is empty and the first part is "
    "correctly copied",
    did_succeed && memcmp(part1, "hello\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_double_sep, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the separator appears twice, only the first occurrence is used",
    did_succeed && memcmp(part1, "hello\0", part1_size) == 0 &&
      memcmp(part2, ",there\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_no_sep, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When there is no separator, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_empty, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the source is empty, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_part1_too_long, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the first part is too long, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_part2_too_long, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the second part is too long, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_part1_much_too_long, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the first part is much too long, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );

  memcpy(part1, "\0\0\0\0\0\0", part1_size);
  memcpy(part2, "\0\0\0\0\0\0\0", part2_size);
  did_succeed = pstr_split_on_first_occurrence(
    src_part2_much_too_long, part1, part1_size, part2, part2_size, ','
  );
  run_test(
    "When the second part is much too long, nothing is copied",
    !did_succeed && memcmp(part1, "\0\0\0\0\0\0", part1_size) == 0 &&
      memcmp(part2, "\0\0\0\0\0\0\0", part2_size) == 0
  );
}


static void test_pstr_clear() {
  print_test_group("test_pstr_clear()");
  char str[] = "hello!";
  pstr_clear(str);
  run_test(
    "A string is successfully cleared",
    str[0] == 0
  );
}


static void test_pstr_slice_from() {
  print_test_group("test_pstr_slice_from()");
  bool did_succeed;
  char str[9];

  memcpy(str, ",,hello!\0", 9);
  did_succeed = pstr_slice_from(str, 2);
  run_test(
    "A slice is correctly performed",
    did_succeed && memcmp(str, "hello!\0", 7) == 0
  );

  memcpy(str, ",,hello!\0", 9);
  did_succeed = pstr_slice_from(str, 7);
  run_test(
    "The last character is returned when slicing at the last index",
    did_succeed && str[0] == '!'
  );

  memcpy(str, ",,hello!\0", 9);
  did_succeed = pstr_slice_from(str, 8);
  run_test(
    "A slice is not performed if the position is past the end of the string",
    !did_succeed && memcmp(str, ",,hello!\0", 9) == 0
  );
}


static void test_pstr_slice_to() {
  print_test_group("test_pstr_slice_to()");
  bool did_succeed;
  char str[6];

  memcpy(str, "hello\0", 6);
  did_succeed = pstr_slice_to(str, 2);
  run_test(
    "A slice is correctly performed",
    did_succeed && memcmp(str, "he\0", 3) == 0
  );

  memcpy(str, "hello\0", 6);
  did_succeed = pstr_slice_to(str, 4);
  run_test(
    "The last character can be sliced off",
    did_succeed && memcmp(str, "hell\0", 5) == 0
  );

  memcpy(str, "hello\0", 6);
  did_succeed = pstr_slice_to(str, 5);
  run_test(
    "A slice is not performed if the position is past the end of the string",
    !did_succeed && memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_slice() {
  print_test_group("test_pstr_slice()");
  bool did_succeed;
  char str[9];

  memcpy(str, "hi there\0", 9);
  did_succeed = pstr_slice(str, 1, 7);
  run_test(
    "A start-end slice is correctly performed",
    did_succeed && memcmp(str, "i ther\0", 7) == 0
  );

  memcpy(str, "hi there\0", 9);
  did_succeed = pstr_slice(str, 1, 8);
  run_test(
    "A slice is not performed if the end is too far",
    !did_succeed && memcmp(str, "hi there\0", 9) == 0
  );


  memcpy(str, "hi there\0", 9);
  did_succeed = pstr_slice(str, 1, 1);
  run_test(
    "A slice is not performed if start == end",
    !did_succeed && memcmp(str, "hi there\0", 9) == 0
  );
}


static void test_pstr_ltrim() {
  print_test_group("test_pstr_ltrim()");
  char str[9];

  memcpy(str, " hello\0\0\0", 9);
  pstr_ltrim(str);
  run_test(
    "Leading whitespace is successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "\n\nhello\0\0", 9);
  pstr_ltrim(str);
  run_test(
    "Leading newlines are successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_ltrim(str);
  run_test(
    "Nothing is trimmed if there are no leading spaces",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_rtrim() {
  print_test_group("test_pstr_rtrim()");
  char str[9];

  memcpy(str, "hello \0\0\0", 9);
  pstr_rtrim(str);
  run_test(
    "Trailing whitespace is successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\n\n\0\0", 9);
  pstr_rtrim(str);
  run_test(
    "Trailing newlines are successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_rtrim(str);
  run_test(
    "Nothing is trimmed if there are no trailing spaces",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_trim() {
  print_test_group("test_pstr_trim()");
  char str[9];

  memcpy(str, " hello\t\0\0", 9);
  pstr_trim(str);
  run_test(
    "Trailing and leading whitespace is successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_trim(str);
  run_test(
    "Nothing is trimmed if there are no trailing or leading spaces",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_ltrim_char() {
  print_test_group("test_pstr_ltrim_char()");
  char str[9];

  memcpy(str, "2hello\0\0\0", 9);
  pstr_ltrim_char(str, '2');
  run_test(
    "Leading characters are successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_ltrim_char(str, '2');
  run_test(
    "Nothing is trimmed if there are no leading characters",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_rtrim_char() {
  print_test_group("test_pstr_rtrim_char()");
  char str[9];

  memcpy(str, "hello2\0\0\0", 9);
  pstr_rtrim_char(str, '2');
  run_test(
    "Trailing characters are successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_rtrim_char(str, '2');
  run_test(
    "Nothing is trimmed if there are no trailing characters",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_trim_char() {
  print_test_group("test_pstr_trim_char()");
  char str[9];

  memcpy(str, "2hello2\0\0", 9);
  pstr_trim_char(str, '2');
  run_test(
    "Trailing and leading characters are successfully trimmed",
    memcmp(str, "hello\0", 6) == 0
  );

  memcpy(str, "hello\0\0\0\0", 9);
  pstr_trim_char(str, '2');
  run_test(
    "Nothing is trimmed if there are no trailing or leading spaces",
    memcmp(str, "hello\0", 6) == 0
  );
}


static void test_pstr_from_int64() {
  print_test_group("test_pstr_from_int64()");
  bool did_succeed;
  char str[16];
  size_t new_str_len;

  did_succeed = pstr_from_int64(str, 16, 2048, &new_str_len);
  run_test(
    "An average number is rendered correctly",
    did_succeed && memcmp(str, "2048\0", 5) == 0
  );

  did_succeed = pstr_from_int64(str, 16, -5, &new_str_len);
  run_test(
    "A negative number is rendered correctly",
    did_succeed && memcmp(str, "-5\0", 3) == 0
  );

  did_succeed = pstr_from_int64(str, 16, 12345678901234567890ULL, &new_str_len);
  run_test(
    "The function fails for a number that does not fit in our string",
    !did_succeed
  );
}


int main(int argc, char **argv) {
  test_pstr_is_valid();
  test_pstr_len();
  test_pstr_is_empty();
  test_pstr_eq();
  test_pstr_starts_with_char();
  test_pstr_starts_with();
  test_pstr_ends_with_char();
  test_pstr_ends_with();
  test_pstr_copy();
  test_pstr_copy_n();
  test_pstr_cat();
  test_pstr_vcat();
  test_pstr_split_on_first_occurrence();
  test_pstr_clear();
  test_pstr_slice_from();
  test_pstr_slice_to();
  test_pstr_slice();
  test_pstr_ltrim();
  test_pstr_rtrim();
  test_pstr_trim();
  test_pstr_ltrim_char();
  test_pstr_rtrim_char();
  test_pstr_trim_char();
  test_pstr_from_int64();
  print_test_statistics();
}
