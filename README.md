# File JSON

C++ library
- Read JSON files and strings into a tree of managed values (`load`, `loadFromString`),
strict RFC 8259 parsing with a nesting limit for hostile input.
- One class per JSON type: `VNull`, `VBoolean`, `VNumber`, `VString`, `VArray` and
`VAssociativeArray` (objects keep the order of their keys).
- Write the tree back (`save`, `saveToString`) indented with tabs, with 4 spaces, or minified;
numbers are locale independent and round trip exactly.

Built on `xyo-system`; used by `xyo-version`, `xyo-cc`, `fabricare`
and the rest of the XYO C++ tools.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, depend on it, first program, threads
- [Document model](docs/document-model.md) - `Value` classes: inspect, build and change documents, pitfalls
- [Reading JSON](docs/reading.md) - `load` / `loadFromString`: accepted syntax, numbers, strings, errors, limits
- [Writing JSON](docs/writing.md) - `save` / `saveToString`: modes, output format, numbers, escaping
- [API reference](docs/reference.md)

A Claude Code skill for this library is in
[.claude/skills/file-json](.claude/skills/file-json/SKILL.md).

## License

Copyright (c) 2020-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
