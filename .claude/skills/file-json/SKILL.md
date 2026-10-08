---
name: file-json
description: >-
  How to use the file-json C++ library (namespace XYO::FileJSON), the JSON
  reader / writer of the XYO C++ stack on top of xyo-system: load /
  loadFromString (strict RFC 8259 parsing into a TPointer<Value> tree),
  save / saveToString with Mode::IndentationTab / Indentation4Spaces /
  Minified, the document model Value / ValueType with VNull, VBoolean,
  VNumber (double), VString (String), VArray (ArrayT = TDynamicArray of
  TPointerX<Value>) and VAssociativeArray (AssociativeArrayT = insertion
  ordered TAssociativeArray<String, TPointerX<Value>>), the fromBoolean /
  fromNumber / fromString factories, VNumber::toChars / fromChars,
  XYO_FILEJSON_MAX_DEPTH, initMemory. Use when writing or reviewing code
  that includes <XYO/FileJSON.hpp>, depends on "file-json" in
  fabricare.json, reads or writes JSON in XYO C++ code (version.json,
  configuration files), uses any of these names, or when working inside the
  file-json repository or tools built on it (xyo-version, xyo-cc,
  fabricare).
---

# file-json

JSON library of the XYO C++ libraries, on top of `xyo-system` (see the
`xyo-system`, `xyo-encoding`, `xyo-data-structures` and `xyo-managed-memory`
skills; their rules apply). Purpose: **parse JSON into a tree of managed
objects, change it, write it back**, strict and order preserving.

Full documentation: `docs/` in the file-json repository
(`X:\Storage\XYO\Gitea\CPP\file-json\docs` on this machine): README,
getting-started, **document-model** (inspect / build / change, memory rules,
pitfalls), reading, writing, reference. Read the matching page when you need
more than this summary. When in doubt read the header in
`source/XYO/FileJSON/`.

## API

```cpp
#include <XYO/FileJSON.hpp>
using namespace XYO::FileJSON;   // also brings in String, TPointer, TDynamicCast, ...

TPointer<Value> document;
bool ok = load("file.json", document);          // false -> document == nullptr
bool ok = loadFromString(text, document);       // 0 terminated const char *
bool ok = save("file.json", document);          // Mode::IndentationTab by default
bool ok = save("file.json", document, Mode::Indentation4Spaces);
String out;
bool ok = saveToString(out, document, Mode::Minified);  // replaces out; unchanged on error
```

| JSON | Class | `ValueType` | Data | Create |
|------|-------|-------------|------|--------|
| null | `VNull` | `Null` | - | `TMemory<VNull>::newMemory()` |
| bool | `VBoolean` | `Boolean` | `bool value` | `VBoolean::fromBoolean(b)` |
| number | `VNumber` | `Number` | `double value` | `VNumber::fromNumber(n)` |
| string | `VString` | `String` | `String value` | `VString::fromString(s)` |
| array | `VArray` | `Array` | `TPointerX<ArrayT> value` | `TPointer<VArray> a; a.newMemory();` |
| object | `VAssociativeArray` | `AssociativeArray` | `TPointerX<AssociativeArrayT> value` | `TMemory<VAssociativeArray>::newMemory()` |

All derive from `Value : DynamicObject` (managed, pooled per thread, not
copyable). Inspect with `TDynamicCast<VString *>(value)` (nullptr on
mismatch or nullptr input) or `value->getValueType()` + `static_cast` in a
switch. `TIsType<T>(p)` dereferences `p`.

## Reading a document

```cpp
VAssociativeArray *root = TDynamicCast<VAssociativeArray *>(document);
if (!root) { /* not an object */ };

TPointer<Value> member;
if (root->get("name", member)) {                     // false if missing
	VString *name = TDynamicCast<VString *>(member); // nullptr if not a string
	if (name) printf("%s\n", name->value.value());
};

VArray *list = TDynamicCast<VArray *>(member);      // check for nullptr
for (size_t k = 0; k < list->length(); ++k) {
	Value *item = list->index(k);                    // never nullptr in a loaded document
};
TPointer<Value> item;
if (list->get(7, item)) { /* bounds checked */ };

AssociativeArrayT *map = root->value;                // iterate in key order of the file
for (size_t k = 0; k < map->length(); ++k) {
	const String &key = map->arrayKey->index(k);
	Value *value = map->arrayValue->index(k);
};
```

## Building / changing

```cpp
TPointer<VAssociativeArray> config;
config.newMemory();
config->set("port", VNumber::fromNumber(8080));   // new key -> appended
config->set("port", VNumber::fromNumber(9090));   // existing key -> replaced in place
config->value->remove("port");                    // bool
TPointer<VArray> hosts;
hosts.newMemory();
hosts->value->push(VString::fromString("alpha"));
hosts->value->insert(0) = TMemory<VNull>::newMemory();
hosts->value->remove(1);                          // bool
hosts->index(0) = VBoolean::fromBoolean(true);    // replace element
config->set("hosts", hosts);
static_cast<VNumber *>(node)->value = 42;         // change a scalar in place
```

## Hard rules

1. **Check both lookup steps**: `get` returns `bool` (missing key / index),
   `TDynamicCast` returns `nullptr` (wrong type). Never assume the shape of
   input JSON.
2. **`VArray::index(k)` / `operator[]` grow the array** with `nullptr`
   slots when `k >= length()` (no bounds check, no error). Read with `get(k, out)` or check
   `length()` first.
3. **Empty (`nullptr`) slots are saved as `null`.** `set(k, x)` past the
   end, `index` past the end, growing `setLength` and `set(key, nullptr)`
   leave `nullptr` slots; the writer outputs `null` for them, and reading
   back gives `VNull` nodes. Code walking a hand built tree must check for
   `nullptr` before `getValueType()`. Prefer `TMemory<VNull>::newMemory()`
   for an explicit JSON null. Empty containers save as `[]` / `{}`.
4. **`saveToString` replaces `out`** on success and leaves it unchanged on
   failure. Saving fails (`false`) only for a `nullptr` root, a plain
   `Value`, nesting over the limit (cycles) or I/O errors; a failed `save`
   may leave a partial file.
5. **Numbers are `double`**: integers exact only up to 2^53; keep 64 bit ids
   and exact decimals as strings. NaN / Infinity are written as `null`;
   `-0` as `0`. Number text is locale independent and round trips exactly.
6. **Strict input**: no comments, trailing commas, single quotes, unquoted
   keys, NaN, leading zeros / `+`, raw control characters in strings. Any top
   level value is allowed: check the root type yourself. Optional UTF-8 BOM
   at the start only. No error position is reported.
7. **Ownership**: `TPointer<Value>` for locals / parameters / returns;
   raw `Value *` into the tree is valid only while the tree holds that node
   (replacing or removing it can free it); class members are
   `TPointerX<Value>` with `pointerLink(this)` in the constructor.
8. **One thread per document** (per-thread pools, plain refcounts). Move a
   document between threads as text: `saveToString` -> copy characters ->
   `loadFromString` on the other thread. Call
   `XYO::ManagedMemory::Registry::registryInit()` first in `main` when
   threads are used; `XYO::FileJSON::initMemory()` pre-creates the value
   pools of the current thread.
9. Strings: `\uXXXX` and surrogate pairs decode to UTF-8; `\u0000` gives a 0
   byte inside `String` (comparisons and C functions stop there); other
   bytes are **not UTF-8 validated**, in either direction. Writer escapes
   `"`, `\` and control characters only (`\u00XX` uppercase), keeps `/`,
   DEL and UTF-8 raw.
10. Duplicate keys in input: last value wins, at the first key's position.
11. `XYO_FILEJSON_MAX_DEPTH` (256) limits nesting for reader and writer
    (cycles fail on save); it takes effect where the library is compiled.
12. `Version`, `Copyright`, `License` exist in every XYO library: qualify
    them (`XYO::FileJSON::Version::version()`).
13. `VBoolean::fromString` / `VNumber::fromString` are lenient converters
    (`"NaN"` / `"Infinity"` accepted, unparsable -> `0`); `VNumber::toString`
    returns `"NaN"` / `"Infinity"`, the writer `null`. Strict parsing of a
    number: `VNumber::fromChars(text, length, out)`; formatting:
    `VNumber::toChars(value, buffer[NumberBufferSize])` (0 = not finite).

## Output format

Indented modes: one item per line, tab or 4 spaces per level, `"key": value`,
**CRLF**, no final newline, empty containers as `[]` / `{}`. Minified: no
whitespace. Keys in object order, so load -> change -> save keeps diffs
small.

## Using it

- `#include <XYO/FileJSON.hpp>`. C++17 (checked by the header).
- fabricare consumer: `"dependency": ["file-json"]` (DLL) or
  `["file-json.static"]` (exports `XYO_FILEJSON_LIBRARY`, static CRT).
  Install `xyo-platform`, `xyo-managed-memory`, `xyo-data-structures`,
  `xyo-multithreading`, `xyo-encoding`, `xyo-system`, then this library to
  the SDK before building dependents (see the `fabricare` skill).
- Without fabricare: compile the seven amalgams (`Platform`,
  `ManagedMemory`, `DataStructures`, `Multithreading`, `Encoding`, `System`,
  `FileJSON` `.Amalgam.cpp`) with every `XYO_*_LIBRARY` defined (MSVC: also
  `/DXYO_PLATFORM_COMPILE_STATIC`), the `source/` dirs on the include path,
  `-pthread` on Linux. Or link the SDK `*.static.lib` files with `/MT`.

## Code style (match the repository)

- Tabs (width 8), `.clang-format` in the repo, CRLF line endings; statements
  and blocks end with `};`.
- camelCase methods, `retV` for return values, trailing `_` for protected
  members (`valueType_`). Value classes expose a public `value`.
- Headers: include guard `XYO_FILEJSON_<NAME>_HPP`, guarded includes
  (`#ifndef XYO_FILEJSON_VALUE_HPP #include ...`), a `TMemory<T>`
  specialization to `TMemoryPoolActive<T>` for each value class,
  `XYO_DYNAMIC_TYPE_DEFINE` / `IMPLEMENT` (with a new GUID) / `PUSH`. Add new
  headers to `source/XYO/FileJSON.hpp`, new `.cpp` files to
  `source/XYO/FileJSON.Amalgam.cpp`, new value pools to `initMemory()` in
  `Library.cpp`. Exported symbols use `XYO_FILEJSON_EXPORT`.
- Functions report errors by returning `bool` and leave outputs empty /
  `nullptr` on failure; no exceptions.
- SPDX header: MIT for `source/`, Unlicense for `test/`.
- Tests: `test/test.NN.cpp` (see the `check` / `parse` pattern in
  `test.02.cpp`), plus a `"category": "test"` project in `fabricare.json`;
  run `fabricare make` then `fabricare test`.
