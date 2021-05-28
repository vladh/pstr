![pstr string functions](images/saihou_itodama_sm.png)

# pstr — a set of cautious and easy-to-use C string functions

This is a set of functions that allows you to more easily work with static C strings
while preventing buffer overflows and truncation, so you don't have to worry worrying
about safety as much. pstr has two principles:

**1. If a string doesn't fit, it stops what it's doing and tells you**

Functions like `strcat`, `strncat` and `strlcat` try to fit as much of the source string
into the destination buffer as possible. This means that you either end up with safety
issues like buffer overflows, or in the best case scenario, truncated strings, which can
also present problems.

```c
strlcat(dest, "this string is way too big", 10); 
// concatenated `dest` is truncated, and you have to do an extra check to detect this
```

pstr never overflows (hopefully), never truncates strings, and always adds the `"\0"`
terminator. Most functions return a `bool` to represent whether the operation succeeded.
For example, in `pstr_cat`, if the strings don't fit into the buffer, it returns `false`
without changing anything.

```c
pstr_cat(dest, 10, "this string is way too big"); 
// returns false without changing `dest`
```

**2. It's easier to work with (than `<string.h>` functions)**

Even when everything fits inside your buffer and you don't have to worry about safety,
doing simple things such as concatenating multiple strings can be a pain in C. pstr
functions make this a little bit simpler. For example:

```c
pstr_vcat(dest, dest_size, " Hello", " there ", name, "!", NULL);
```

pstr also comes with [tests](#running-the-tests), for what that's worth.

## Documentation

pstr is very small, so I would recommend directly copying `pstr.h` and `pstr.c` into your
project.

It works with standard C strings, so you can simply create a stack-allocated string as
you normally would, e.g. `char address[30];`.

Usage instructions are included below, and you can find documentation of all functions in
[pstr.h](pstr/pstr.h).

### Concatenation

Concatenating strings is probably the most common operation, for me at least. The nicest
way to do it using pstr is by using `pstr_vcat()`, which takes a destination buffer and
its size, as well as a list of strings, followed by `NULL`. Remember to write the `NULL`
at the end or bad things will happen. As with all pstr functions, the `"\0"` terminator
is added automatically to the string.

```c
char address[35];
char street[] = "Rheingasse 5";
char city[] = "Basel";
char country[] = "Switzerland";

pstr_vcat(address, 35, street, ", ", city, ", ", country, NULL);

// `address` now contains "Rheingasse 5, Basel, Switzerland\0"
```

Like most pstr methods, `pstr_vcat()` returns a `bool` which tells you whether the
operation succeeded or not. `pstr_vcat()` returns `false` if there wasn't enough space,
and you should check for this.

```c
char address[25];
char street[] = "Rheingasse 5";
char city[] = "Basel";
char country[] = "Switzerland";

if (!pstr_vcat(address, 25, street, ", ", city, ", ", country, NULL)) {
  // `address` is not big enough, so we'll end up here, and you can do something to handle
  // the problem
  // `address` is now "\0"
}

```

Most pstr methods leave the destination buffer unchanged if an operation failed.
`pstr_vcat()` will make the destination buffer an empty string if it fails.

If you'd like the standard, old-fashioned two-string concatenation, there's also
`pstr_cat()`.

```c
char message[13];
char greeting[] = "Hello";

assert(pstr_cat(message, 13, greeting, " there!"));

// `message` now contains "Hello there!\0"
// We had just enough space to fit the "\0" in
```

### Copying

`pstr_copy()` works much like `strcpy()`, but does not overflow, does not truncate, and
only performs the copy if there is enough space, otherwise it leaves the destination
buffer untouched.

```c
char message[15];
memset(message, "hello\0", 6);

if (!pstr_copy(message, 15, "welcome!")) {
  // Whoops, couldn't make the message
}

// We had enough space, so `message` now contains `"welcome!\0"`
```

### Splitting

`pstr_split_on_first_occurrence()` will split a string into two parts, on the first
occurrence of a separator.

```c
char list[15];
memcpy(list, "cats,seashells\0", 15);

char first_item[15];
char second_item[15];

if (!pstr_split_on_first_occurrence(
  list,
  first_item, 15,
  second_item, 15,
  ','
)) {
  // Not enough room...
};

// `first_item` now contains "cats"
// `second_item` now contains "seashells"
```

### Slicing

You can slice from an index to the end with `pstr_slice_from()`, from the start to an
index with `pstr_slice_to()`, and between two indices with `pstr_slice()`. End indices
are exclusive, so slices take the form `[start, end)`. The string is modified in-place.

These methods can fail if you give them indices that are out of range, in which case they
return `false`, so please check the returned value.

```c
char message[15];
memcpy(message, ",,Mallard!\0", 11);

pstr_slice_to(message, 9); // ",,Mallard\0"
pstr_slice_from(message, 2); // "Mallard!\0"
pstr_slice(message, 2, 9); // "Mallard\0"
```

### Trimming

You can trim whitespace, or any specific character, from the start, end, or start and end
of a string. The string is changed in-place. These methods cannot fail so they return
nothing.

```c
char message[15];

memcpy(message, " Strasbourg\n");
pstr_ltrim(message) // "Strasbourg\n";
pstr_rtrim(message) // " Strasbourg";
pstr_trim(message) // "Strasbourg";

memcpy(message, ",,Strasbourg,,");
pstr_ltrim_char(message, ','); // "Strasbourg,,"
pstr_rtrim_char(message, ','); // ",,Strasbourg"
pstr_trim_char(message, ','); // "Strasbourg"
```

### `int64` to string

`pstr_from_int64()` can be used to easily convert a number to a string. The length of
this new string will be returned in the `size_t` variable pointed to by the last
parameter. Your destination buffer should be at least 21 characters long, to fit all
possible 64-bit strings. If there isn't enough space in your destination buffer, it will
return false, and set your destination buffer to `"\0"`.

```c
char number[21];
size_t number_length;

pstr_from_int64(number, 21, -4815162342, &number_length);

// `number` is now "-4815162342"
```

### Comparisons

You can easily check whether two strings are equal.

```c
char name[] = "Bobby";
if (pstr_eq(name, "Bobby")) {
  // ...
}
```

You can also check if a string is empty.

```c
char name[] = "";
if (pstr_is_empty(name)) {
  // ...
}
```

And there's also a method to get the length of a string. It just calls `strlen()`, but
it's included for visual consistency.

```c
size_t len = pstr_len("Locarno");
```

### Starts/ends with

You can easily check if a string starts or ends with a character or another string.

```c
pstr_starts_with_char("Magpie!", 'M'); // true
pstr_starts_with("Magpie!", "Mag"); // true
pstr_ends_with_char("Magpie!", 'e'); // false
pstr_ends_with("Magpie!", "pie!"); // true
```

### Other utilities

There are a few utility methods.

`pstr_clear(str)` sets a string to `"\0"`, effectively emptying it.

`pstr_is_valid(str, str_buffer_size)` check if `str` is valid by checking whether or not
it has a null terminator within `str_buffer_size` bytes.

## FAQ

**I've found a safety issue with one of the functions!**

Issues and pull requests are welcome! :)

**Why not use a dynamic string library such as [sds](https://github.com/antirez/sds)?**

sds is great, and is a better solution in many cases, but it's sometimes nice to be able
to work with static strings --- they're easy to serialize to disk, don't need a
`malloc()` call, are better in embedded environments and so on. Plus, for a lot of simple
string handling situations, you don't need your strings to be able to grow to any length.
It's nice to be able to work with static strings without dealing with awful `<string.h>`
functions with `strcat`.

**Why is it called pstr?**

I wrote it for my game engine [Peony](https://vladh.net).

## Running the tests

You can easily run the tests using `make run-test`.

## License

```
pstr string functions

Copyright 2021 Vlad-Stefan Harbuz <vlad@vladh.net>

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

Icon by [irasutoya](https://www.irasutoya.com)
