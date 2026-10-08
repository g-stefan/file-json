# Reading JSON

```cpp
bool load(const char *fileName, TPointer<Value> &document);
bool loadFromString(const char *value, TPointer<Value> &document);
```

Both parse one complete JSON text into a tree (see
[Document model](document-model.md)).

- On success they return `true` and `document` holds the root value.
- On any error (file cannot be opened, read error, invalid JSON, nesting too
  deep, `value == nullptr`) they return `false` and set `document` to
  `nullptr`. A partially parsed tree is never returned.
- `fileName` is a UTF-8 path, opened with `XYO::System::File::openRead`.
- `loadFromString` reads a 0 terminated string. A `String` converts
  implicitly (`loadFromString(text, document)`); the text ends at its first
  0 byte.
- The whole document is built in memory. A file is streamed through a 4 KB
  buffer; its text is not loaded into memory first.

```cpp
TPointer<Value> document;
if (!load("settings.json", document)) {
	printf("settings.json: missing or invalid\n");
	return false;
};
VAssociativeArray *settings = TDynamicCast<VAssociativeArray *>(document);
if (!settings) {
	printf("settings.json: an object is expected\n");
	return false;
};
```

There is no error position or message: the result is valid or not. When a
user needs to locate an error, validate the file with another tool.

## Accepted syntax

Strict JSON as defined by RFC 8259:

- **Any value at the top level**: `{...}`, `[...]`, `"text"`, `12`, `true`,
  `false`, `null`. Check the root type if your format requires an object.
- **Whitespace**: space, tab, CR and LF, between any tokens.
- **Byte order mark**: an optional UTF-8 BOM (`EF BB BF`) at the very start
  is skipped. A BOM anywhere else is an error. UTF-16 / UTF-32 input is not
  supported.
- **Nothing after the value** but whitespace: `[1] [2]` and `truex` are
  errors.

Not accepted (each one makes the whole load fail):

| Input | Example |
|-------|---------|
| empty or whitespace only | `""`, `"   "` |
| comments | `// x`, `/* x */` |
| trailing commas | `[1,]`, `{"a":1,}` |
| missing or doubled separators | `[,1]`, `["a" "b"]`, `{"a" 1}` |
| unquoted or single quoted keys / strings | `{a:1}`, `['a']` |
| unterminated containers or strings | `[1,2`, `{"a":"x"`, `["abc` |
| `NaN`, `Infinity`, hex numbers | `[NaN]`, `[0x10]` |
| leading `+`, leading zeros, bare `.` | `[+1]`, `[007]`, `[.5]`, `[1.]`, `[1e]`, `[-]` |
| truncated literals | `tru`, `nul` |
| raw control characters (< 0x20) inside strings | a literal tab or newline in `"..."` |
| unknown escapes | `"\x41"`, `"\u12zz"` |
| unpaired surrogates | `"\uD83D"`, `"\uDE00"` |
| nesting deeper than `XYO_FILEJSON_MAX_DEPTH` | 257 nested `[` with the default limit |

## Numbers

Grammar: `-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?`.

Every number becomes a `double` (`NumberT`):

- parsing uses `std::from_chars` and does **not** depend on the C locale (a
  `de_DE` locale with `,` as decimal separator reads `1.5` correctly).
  Standard libraries without floating point `from_chars` fall back to
  `strtod` with the separator swapped; define
  `XYO_FILEJSON_NO_FLOAT_CHARCONV` when building the library to force that
  path;
- the value is the nearest `double`: integers are exact up to 2^53
  (`9007199254740993` reads as `9007199254740992`);
- numbers too large for a `double` (`1e400`) become infinity, which the
  writer outputs as `null`; numbers too small become 0 or a subnormal.

Store 64 bit identifiers and exact decimals as JSON strings if they must
survive unchanged.

## Strings

- Escapes: `\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t` and `\uXXXX`
  (hex digits in either case).
- `\uXXXX` is converted to UTF-8; a surrogate pair (`😀`) becomes
  one 4 byte code point.
- `\u0000` becomes a 0 byte inside the `String` (binary safe, counted by
  `length()`).
- Other bytes (>= 0x20) are copied as they are. **UTF-8 is not validated**:
  invalid sequences reach the `String` unchanged. Use the strict UTF
  functions of `xyo-encoding` if you need to check them.
- Keys follow the same rules as string values.

## Objects

- Keys are kept in file order.
- A duplicate key replaces the earlier value, at the earlier position:
  `{"a":1,"b":2,"a":3}` reads as `{"a":3,"b":2}`.

## Nesting limit

`XYO_FILEJSON_MAX_DEPTH` (default `256`, in `Dependency.hpp`) is the maximum
number of nested arrays / objects. The reader fails cleanly above it,
including on hostile input such as a million `[`, instead of overflowing the
stack; the writer uses the same limit.

The macro is used inside the compiled library: to change it, define it when
**building the library** (or when compiling `FileJSON.Amalgam.cpp` into your
program), not only in your own sources.

## Other sources

Only files and C strings are read directly. For other data, collect the text
first, for example with `XYO::System::Shell::fileGetContents` or a
`StringWrite`, then call `loadFromString`.
