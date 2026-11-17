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
executable has no rpath; `make test` runs it with `LD_LIBRARY_PATH=.` so it
can find the shared library in the repository root. To run the test binary
directly:

```sh
LD_LIBRARY_PATH=. ./library_test
```

## API

```c
string_t from_str(const char* str);
string_t from_strn(const char* str, size_t len);
string_t repeat_char(char c, size_t r_times);
string_t repeat_str(const char* str, size_t r_times);
string_t repeat_strn(const char* str, size_t str_len, size_t r_times);
void free_string(string_t string);

bool cyclic_shift(string_t string);
```

- `from_str` copies a NUL-terminated string. Passing `NULL` returns `NULL`.
- `from_strn` copies exactly `len` bytes and always NUL-terminates the result.
  This is the safe choice for binary data or data with embedded NUL bytes.
  `from_strn(NULL, 0)` returns an empty string; `from_strn(NULL, len > 0)`
  returns `NULL`.
- `repeat_char` and `repeat_str` accept a `size_t` count, so they are not
  limited to 65535 repetitions. Passing `NULL` to `repeat_str` returns `NULL`.
- `repeat_strn` repeats exactly `str_len` bytes. `repeat_strn(NULL, 0, r_times)`
  returns an empty string; `repeat_strn(NULL, str_len > 0, ...)` returns `NULL`.
- `cyclic_shift` rotates the string one byte to the left. Empty and one-byte
  strings are unchanged. It returns `false` for a `NULL` string or an invalid
  `len` value.
- `free_string(NULL)` is a no-op.

## Error handling

Constructors return `NULL` on failure and set `errno`:

- `EINVAL`: `NULL` pointer with a non-zero length, or a `NULL` argument to
  `from_str`/`repeat_str`.
- `ENOMEM`: allocation failure.
- `ERANGE`: size overflow or an invalid `len` in `cyclic_shift`.

`cyclic_shift` returns `false` and sets `errno` on failure. On success, these
functions leave `errno` unchanged.

## Memory and thread safety

Each string returned by this library owns one allocation containing both the
`struct string` and its character buffer. Always release it with
`free_string()`; do not free `string->str` separately.

The constructors allocate their own memory and are thread-safe. `cyclic_shift`
mutates the string in place, so callers must synchronize access if the same
string is shared between threads.

## Performance

- One `malloc`/`free` pair per string.
- `repeat_str`/`repeat_strn` use exponential doubling to fill the result,
  reducing the number of copy operations for large repetition counts.
- `cyclic_shift` uses `memmove` and the stored `len`, avoiding a scan for the
  terminating NUL.

## Security

- Length-aware APIs (`from_strn`, `repeat_strn`) avoid relying on `strlen` for
  untrusted or binary input.
- `cyclic_shift` validates `len <= size` before accessing the buffer.
- The build enables `-D_FORTIFY_SOURCE=2`, `-fstack-protector-strong`, RELRO,
  `-z now`, non-executable stacks, PIE, and a library soname on Linux.
- The test binary does not embed an rpath.

## ABI

Existing function signatures and the layout of `struct string` are unchanged.
`from_strn` and `repeat_strn` are additive API.