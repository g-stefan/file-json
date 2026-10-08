# API reference

Namespace `XYO::FileJSON` unless noted. Everything is declared by
`#include <XYO/FileJSON.hpp>`.

## Macros

| Macro | Meaning |
|-------|---------|
| `XYO_FILEJSON_EXPORT` | dllexport while building the DLL (`XYO_FILEJSON_INTERNAL`), dllimport for users, empty with `XYO_FILEJSON_LIBRARY` |
| `XYO_FILEJSON_INTERNAL` | defined by the build when compiling this library (`FILE_JSON_INTERNAL` is accepted too) |
| `XYO_FILEJSON_LIBRARY` | static linking, set for users of `file-json.static` |
| `XYO_FILEJSON_MAX_DEPTH` | maximum nesting of arrays / objects for reader and writer, default `256`; takes effect where the library is compiled |
| `XYO_FILEJSON_NO_FLOAT_CHARCONV` | when compiling the library: use `snprintf` / `strtod` instead of floating point `std::to_chars` / `std::from_chars` |

## Type aliases

| Name | Type | Header |
|------|------|--------|
| `BooleanT` | `bool` | `VBoolean.hpp` |
| `NumberT` | `double` | `VNumber.hpp` |
| `StringT` | `String` (`XYO::Encoding::TString<char>`) | `VString.hpp` |
| `ArrayT` | `TDynamicArray<TPointerX<Value>, 4, TMemoryPoolActive>` | `VArray.hpp` |
| `AssociativeArrayT` | `TAssociativeArray<String, TPointerX<Value>, 4, TMemoryPoolActive>` | `VAssociativeArray.hpp` |

## enum struct ValueType

`Unknown = 0`, `Null`, `Boolean`, `Number`, `String`, `Array`,
`AssociativeArray`.

## enum struct Mode

| Value | Output |
|-------|--------|
| `IndentationTab = 0` | one item per line, tab per level, CRLF |
| `Indentation4Spaces = 1` | one item per line, 4 spaces per level, CRLF |
| `Minified = 2` | no whitespace |

## Reading and writing

| Function | Description |
|----------|-------------|
| `bool load(const char *fileName, TPointer<Value> &document)` | parse a file; `false` and `document = nullptr` on error |
| `bool loadFromString(const char *value, TPointer<Value> &document)` | parse a 0 terminated string; `false` and `document = nullptr` on error (also for `nullptr`) |
| `bool save(const char *fileName, Value *document, Mode mode = Mode::IndentationTab)` | write a file (created / truncated); empty (`nullptr`) slots are written as `null`; `false` on error, file content then unspecified |
| `bool saveToString(String &output, Value *document, Mode mode = Mode::IndentationTab)` | **replace** `output` with the JSON text; `false` on error, `output` then unchanged |
| `void initMemory()` | create the pools of all value classes for the current thread, in a fixed order |

See [Reading JSON](reading.md) and [Writing JSON](writing.md).

## class Value : public DynamicObject

Base class of all nodes. Not copyable, assignable or movable. Allocated
from `TMemoryPoolActive` (per thread).

| Member | Description |
|--------|-------------|
| `Value()` | type `ValueType::Unknown`; not useful by itself |
| `ValueType getValueType() const` | the type tag |
| `XYO_DYNAMIC_TYPE_DEFINE` members | runtime type info for `TDynamicCast`, `TIsType` |

## class VNull : public Value

| Member | Description |
|--------|-------------|
| `VNull()` | type `Null`; create with `TMemory<VNull>::newMemory()` |
| `String toString()` | `"null"` |

## class VBoolean : public Value

| Member | Description |
|--------|-------------|
| `BooleanT value` | the value, `false` by default |
| `String toString()` | `"true"` / `"false"` |
| `static TPointer<VBoolean> fromBoolean(BooleanT value)` | new node |
| `static TPointer<VBoolean> fromString(const String &value)` | `true` only for `"true"`, otherwise `false` |

## class VNumber : public Value

| Member | Description |
|--------|-------------|
| `NumberT value` | the value, `0` by default |
| `String toString()` | same text as the writer for finite values; `"NaN"`, `"Infinity"`, `"-Infinity"` otherwise |
| `static TPointer<VNumber> fromNumber(NumberT value)` | new node |
| `static TPointer<VNumber> fromString(const String &value)` | JSON number text, or `"NaN"` / `"Infinity"` / `"-Infinity"`; `0` if the text cannot be parsed |
| `static constexpr size_t NumberBufferSize = 64` | buffer size for `toChars` |
| `static size_t toChars(NumberT value, char *buffer)` | shortest round trip text, locale independent, not 0 terminated; returns the length, `0` for NaN / infinity |
| `static bool fromChars(const char *text, size_t length, NumberT &value)` | parse, locale independent; `false` if the text is not entirely consumed |

## class VString : public Value

| Member | Description |
|--------|-------------|
| `StringT value` | the text, UTF-8, may contain 0 bytes |
| `String toString()` | `value` |
| `static TPointer<VString> fromString(const String &value)` | new node |

## class VArray : public Value

| Member | Description |
|--------|-------------|
| `TPointerX<ArrayT> value` | the elements (`TDynamicArray`); use it for `push`, `insert`, `remove`, `set`, `setLength`, `empty` |
| `size_t length() const` | number of elements |
| `TPointerX<Value> &index(size_t idx)` | element reference; **grows** the array with empty (`nullptr`) slots when `idx >= length()` |
| `TPointerX<Value> &operator[](int idx)` | same as `index` |
| `bool get(size_t idx, TPointer<Value> &x)`, `bool get(size_t idx, TPointerX<Value> &x)` | element, `false` if `idx >= length()` |

## class VAssociativeArray : public Value

| Member | Description |
|--------|-------------|
| `TPointerX<AssociativeArrayT> value` | the members (`TAssociativeArray`); use it for `length`, `remove`, `empty` and iteration |
| `void set(const String &key, const Value *x)` | add at the end, or replace an existing key in place |
| `bool get(const String &key, TPointer<Value> &x)`, `bool get(const String &key, TPointerX<Value> &x)` | member, `false` if missing |

`AssociativeArrayT` members used for iteration: `length()`,
`arrayKey->index(k)` (key `k`, a `String`), `arrayValue->index(k)` (value
`k`, a `TPointerX<Value>`); lookup structure: `mapKey` (red black tree from
key to index).

## Metadata

| Function | Returns |
|----------|---------|
| `XYO::FileJSON::Version::version()` | `"6.0.0"` style version |
| `XYO::FileJSON::Version::build()` | build number |
| `XYO::FileJSON::Version::versionWithBuild()` | version and build |
| `XYO::FileJSON::Version::datetime()` | build date and time |
| `XYO::FileJSON::Copyright::copyright()`, `publisher()`, `company()`, `contact()` | copyright strings |
| `XYO::FileJSON::License::license()`, `shortLicense()` | license text (`std::string`) |

## Internal classes

`Input` (buffered character input over an `IRead`) and `Token` (the lexer)
are compiled into the library but not included by `<XYO/FileJSON.hpp>`; they
are implementation details and may change.
