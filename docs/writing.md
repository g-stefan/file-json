# Writing JSON

```cpp
enum struct Mode {
	IndentationTab = 0,
	Indentation4Spaces = 1,
	Minified = 2
};

bool save(const char *fileName, Value *document, Mode mode = Mode::IndentationTab);
bool saveToString(String &output, Value *document, Mode mode = Mode::IndentationTab);
```

`document` can be any value of a tree (`TPointer<Value>`,
`TPointer<VAssociativeArray>`, ... convert implicitly), including a scalar.

- `save` creates or truncates `fileName` (UTF-8 path, opened with
  `XYO::System::File::openWrite`) and writes the document.
- `saveToString` **replaces** the content of `output` with the document.
  The text is built in a separate string and assigned only on success: **on
  failure `output` is left unchanged**.
- Empty slots inside the tree (`nullptr` array elements or member values)
  are written as `null`.
- Both return `false` when `document` itself is `nullptr`, when the tree
  contains a plain `Value` (type `Unknown`), when nesting exceeds
  `XYO_FILEJSON_MAX_DEPTH` (also the case for cyclic trees), or when the
  file cannot be opened or written. **On failure `save` may leave part of
  the document in the file.** Write to a temporary file and rename it when a
  half written file would hurt, or render with `saveToString` first and
  write the text.

```cpp
String text;
if (!saveToString(text, document, Mode::Minified)) {
	return false;
};
fwrite(text.value(), 1, text.length(), stdout);
```

## Output format

The three modes write the same tokens; only whitespace differs.

`Mode::IndentationTab` (default) and `Mode::Indentation4Spaces`:

- one member / element per line, indented by one tab or 4 spaces per level;
- `": "` between a key and its value;
- **CRLF** line endings, no newline after the last `}` / `]`;
- empty containers on one line: `[]`, `{}` (in every mode).

```
{
	"a": 1,
	"b": [],
	"c": [
		true,
		null
	]
}
```

`Mode::Minified`: no whitespace at all.

```
{"a":1,"b":[],"c":[true,null]}
```

A scalar document is written alone: `"text"`, `12`, `true`.

Members are written in the object's order (file order for loaded documents,
insertion order for built ones), so load / change / save keeps a file
stable.

## Numbers

- Integers with absolute value below 2^53 are written without fraction or
  exponent: `8080`, `123456789012`, `-7`. `-0` is written as `0`.
- Other finite values use the shortest text that reads back to the same
  `double` (`std::to_chars`): `0.30000000000000004`, `3.141592653589793`,
  `1e-07`, `1.5e+300`. The result does not depend on the C locale.
- `NaN`, `Infinity` and `-Infinity` have no JSON form and are written as
  `null`. (`VNumber::toString()` does return `"NaN"` / `"Infinity"` /
  `"-Infinity"`; it is a display helper, not the writer.)

Every number written by `save` reads back to the identical `double` with
`load`.

## Strings

- `"` and `\` are escaped as `\"` and `\\`.
- Control characters: `\b`, `\f`, `\n`, `\r`, `\t`, others as `\u00XX`
  (uppercase hex), including 0 bytes (`\u0000`): binary safe, the whole
  `length()` is written.
- `/` is not escaped. DEL (0x7F) and all bytes >= 0x80 are written as they
  are, so UTF-8 text stays readable. Bytes are not validated: write valid
  UTF-8 if other JSON tools must read the result.
- Keys are written the same way.

## Round trip

For any document produced by `load`, `save` followed by `load` gives the
same tree: same types, same key order, same strings (byte for byte), same
numbers (bit for bit, except `-0` → `0`, and infinities from out of range
input → `null`). The whitespace of the original file is not kept.
