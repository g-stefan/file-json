# File JSON — Documentation

`file-json` is the JSON library of the XYO C++ stack. It sits on top of
`xyo-system` and turns a JSON file or string into a tree of managed objects,
and a tree back into JSON text:

- **Read.** `load(fileName, document)` and `loadFromString(text, document)`
  parse strict JSON (RFC 8259) into a `TPointer<Value>`. They return `bool`;
  no exceptions, no partial documents.
- **A small document model.** One class per JSON type: `VNull`, `VBoolean`,
  `VNumber` (`double`), `VString` (`String`, UTF-8), `VArray` and
  `VAssociativeArray` (JSON object, keeps the insertion order of its keys).
  All of them derive from `Value`, carry a `ValueType` tag and the XYO
  runtime type info, so `TDynamicCast<VString *>(value)` works.
- **Write.** `save(fileName, document, mode)` and
  `saveToString(output, document, mode)` write the tree back, indented with
  tabs (default), with 4 spaces, or minified.
- **Safe on hostile input.** Nesting is limited (`XYO_FILEJSON_MAX_DEPTH`,
  256 by default), so deep or cyclic documents fail cleanly instead of
  overflowing the stack. Numbers are parsed and printed independently of the
  C locale and round trip exactly.

```
xyo-version, xyo-cc, fabricare, applications ...
file-json             <-- this library
xyo-system            (File, MemoryRead, StringWrite, Shell, ...)
xyo-encoding          (String, UTF)
xyo-multithreading
xyo-data-structures   (TDynamicArray, TAssociativeArray, DynamicObject, IRead / IWrite)
xyo-managed-memory    (Object, TPointer, TPointerX, pools)
xyo-platform          (macros, TAtomic, CriticalSection, Thread)
```

## Why use it

- **It is the JSON of the XYO stack.** Configuration files, `version.json`,
  `fabricare.json` and compiler option files of the XYO tools are read and
  written with it.
- **It speaks the XYO types.** Strings are `String`, arrays are
  `TDynamicArray`, objects are `TAssociativeArray<String, ...>`, values are
  `Object`s held by `TPointer` / `TPointerX`. No conversion layer between the
  parser and your code.
- **Order preserving.** Objects keep their keys in insertion (file) order,
  so a load / change / save cycle produces a minimal diff.
- **Strict and predictable.** Only standard JSON is accepted (no comments,
  no trailing commas, no single quotes, no `NaN`), and the output is always
  standard JSON (`NaN` / `Infinity` are written as `null`).

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Parse a file | `load("file.json", document)` | `TPointer<Value> document`; `false` on I/O or syntax error, `document` becomes `nullptr` |
| Parse text | `loadFromString(text, document)` | 0 terminated `const char *` |
| Write a file | `save("file.json", document, Mode::IndentationTab)` | CRLF line endings, no final newline |
| Write to a string | `saveToString(output, document, Mode::Minified)` | replaces `output`; unchanged on error |
| What is this value? | `value->getValueType()` or `TDynamicCast<VString *>(value)` | `ValueType::Null`, `Boolean`, `Number`, `String`, `Array`, `AssociativeArray` |
| Object member | `object->get("key", value)` / `object->set("key", x)` | `set` replaces in place, keeps the key position |
| Array element | `array->length()`, `array->index(k)`, `array->value->push(x)` | `index` past the end **grows** the array |
| New value | `VString::fromString("x")`, `VNumber::fromNumber(1)`, `VBoolean::fromBoolean(true)`, `TMemory<VNull>::newMemory()` | |
| New container | `TPointer<VArray> a; a.newMemory();` | or `TMemory<VAssociativeArray>::newMemory()` |
| Output format | `Mode::IndentationTab`, `Mode::Indentation4Spaces`, `Mode::Minified` | |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build it, depend on it (DLL or static), first program, threads, building without fabricare |
| [Document model](document-model.md) | `Value` and its subclasses, inspecting, building and changing documents, memory rules, pitfalls |
| [Reading JSON](reading.md) | `load` / `loadFromString`: accepted syntax, numbers, strings, errors, limits |
| [Writing JSON](writing.md) | `save` / `saveToString`: modes, exact output format, numbers, escaping, errors |
| [API reference](reference.md) | Every public symbol on one page |

The memory model (`Object`, `TPointer`, `TPointerX`, pools, threads) is
documented in the `xyo-managed-memory` repository, `docs/`; the containers
(`TDynamicArray`, `TAssociativeArray`) and `DynamicObject` in the
`xyo-data-structures` repository, `docs/`; `String` in the `xyo-encoding`
repository, `docs/`.

## Source map

```
source/XYO/FileJSON.hpp                  umbrella header, include this
source/XYO/FileJSON.Amalgam.cpp          the whole library in one translation unit
source/XYO/FileJSON/
    Dependency.hpp                       xyo-system, export macros, XYO_FILEJSON_MAX_DEPTH, C++17 check
    Value[.cpp]                          Value base class, ValueType
    VNull / VBoolean / VNumber / VString value classes, fromX / toString helpers
    VArray / VAssociativeArray           container classes (ArrayT, AssociativeArrayT)
    Mode.hpp                             output formats
    Library[.cpp]                        initMemory() for all value pools
    Input[.cpp]                          buffered character input over an IRead (internal)
    Token[.cpp]                          JSON lexer (internal)
    Reader[.cpp]                         load, loadFromString
    Writer[.cpp]                         save, saveToString
    Copyright / License / Version        library metadata
input/simple.json                        sample document used by test.01
test/test.01.cpp                         load a file, save it in the three modes
test/test.02.cpp                         reader conformance, depth limits, writer output, locale
```

## AI assistant skill

A Claude Code skill describing how to use this library lives in
[`.claude/skills/file-json/`](../.claude/skills/file-json/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that depend on
`file-json`.
