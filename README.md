# libstrings

A small C string library with string construction helpers and a cyclic-shift
animation helper.

## Build and test

```sh
make
make test
make clean
```

`make` builds `libstrings.so` and the `library_test` executable. The test
executable is linked with an rpath of `$ORIGIN`, so it can find the shared
library in the repository root.

## API

```c
string_t from_str(const char* str);
string_t repeat_char(char c, size_t r_times);
string_t repeat_str(const char* str, size_t r_times);
void free_string(string_t string);

bool cyclic_shift(string_t string);
```

- `from_str` copies a NUL-terminated string. Passing `NULL` returns `NULL`.
- `repeat_char` and `repeat_str` accept a `size_t` count, so they are not
  limited to 65535 repetitions. Passing `NULL` to `repeat_str` returns `NULL`.
- `cyclic_shift` rotates the string one byte to the left. Empty and one-byte
  strings are unchanged. It returns `false` for a `NULL` string.
- `free_string(NULL)` is a no-op.

## Error handling

Allocation failures and invalid arguments are reported by returning `NULL`
(or `false` for `cyclic_shift`) instead of dereferencing an invalid pointer.
