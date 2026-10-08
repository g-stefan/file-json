# Document model

A parsed document is a tree of managed objects. The root is a
`TPointer<Value>`; every node is one of six classes derived from `Value`.

| JSON | Class | `ValueType` | Public data | Factory |
|------|-------|-------------|-------------|---------|
| `null` | `VNull` | `Null` | - | `TMemory<VNull>::newMemory()` |
| `true` / `false` | `VBoolean` | `Boolean` | `BooleanT value` (`bool`) | `VBoolean::fromBoolean(b)` |
| number | `VNumber` | `Number` | `NumberT value` (`double`) | `VNumber::fromNumber(n)` |
| string | `VString` | `String` | `StringT value` (`String`, UTF-8) | `VString::fromString(s)` |
| array | `VArray` | `Array` | `TPointerX<ArrayT> value` | `TMemory<VArray>::newMemory()` |
| object | `VAssociativeArray` | `AssociativeArray` | `TPointerX<AssociativeArrayT> value` | `TMemory<VAssociativeArray>::newMemory()` |

```cpp
typedef TDynamicArray<TPointerX<Value>, 4, TMemoryPoolActive> ArrayT;
typedef TAssociativeArray<String, TPointerX<Value>, 4, TMemoryPoolActive> AssociativeArrayT;
```

`Value` derives from `DynamicObject` (and so from `Object`): every node is
reference counted, comes from a per-thread active pool, and carries runtime
type info. The classes cannot be copied, assigned or moved; you work with
pointers to them. A plain `Value` (type `ValueType::Unknown`) is never
produced by the reader and is rejected by the writer.

## Inspecting a value

Two ways, use whichever reads better:

```cpp
// 1. checked cast, nullptr when the type does not match (or value is nullptr)
VString *name = TDynamicCast<VString *>(value);
if (name) {
	printf("%s\n", name->value.value());
};

// 2. type tag + static_cast, best in a switch over all types
switch (value->getValueType()) {
case ValueType::Number:
	total += static_cast<VNumber *>(value)->value;
	break;
case ValueType::String:
	...
};
```

`TIsType<VString>(value)` also works, but dereferences `value`: check for
`nullptr` first.

### Objects

`VAssociativeArray` is an **insertion ordered** map from `String` to value:
keys keep the order of the file, or the order in which they were added.

```cpp
VAssociativeArray *object = TDynamicCast<VAssociativeArray *>(document);

TPointer<Value> member;
if (object->get("port", member)) { ... }    // false if the key is missing

// all members, in order
AssociativeArrayT *map = object->value;
for (size_t k = 0; k < map->length(); ++k) {
	const String &key = map->arrayKey->index(k);
	Value *item = map->arrayValue->index(k);
	...
};
```

Lookup by key is a red black tree search (`O(log n)`); iteration by index is
direct.

### Arrays

```cpp
VArray *array = TDynamicCast<VArray *>(value);

for (size_t k = 0; k < array->length(); ++k) {
	Value *item = array->index(k);
	...
};

TPointer<Value> item;
if (array->get(7, item)) { ... }             // bounds checked: false past the end
```

**`index(k)` and `operator[]` are not bounds checked reads: past the end they
grow the array**, filling the new slots with `nullptr` (saved as `null`).
Check `length()` first, or use `get`.

## Building and changing documents

Create values with the factories, containers with `newMemory()`, and hand
them to a container; the container keeps them alive.

```cpp
TPointer<VAssociativeArray> config;
config.newMemory();

config->set("name", VString::fromString("server"));
config->set("port", VNumber::fromNumber(8080));
config->set("debug", VBoolean::fromBoolean(false));
config->set("proxy", TMemory<VNull>::newMemory());

TPointer<VArray> hosts;
hosts.newMemory();
hosts->value->push(VString::fromString("alpha"));
hosts->value->push(VString::fromString("beta"));
config->set("hosts", hosts);

save("config.json", config);
```

Object operations (`VAssociativeArray`, or its `value` for the rest):

| Call | Effect |
|------|--------|
| `object->set(key, x)` | add `key` at the end, or replace the value of an existing `key` **in place** (its position does not change) |
| `object->get(key, out)` | `out` = value, `false` if missing (`TPointer<Value>` or `TPointerX<Value>`) |
| `object->value->remove(key)` | remove the member, `false` if missing; later members move up |
| `object->value->length()` | number of members |
| `object->value->empty()` | remove all members |
| `object->value->arrayValue->index(k) = x` | replace the value of member `k` |

Array operations (`VArray`, or its `value`, a `TDynamicArray`):

| Call | Effect |
|------|--------|
| `array->value->push(x)` | append |
| `array->value->insert(k) = x` | insert at `k`, later elements move up |
| `array->value->remove(k)` | remove element `k`, `false` if out of range |
| `array->index(k) = x`, `array->value->set(k, x)` | replace element `k` (grows the array if `k >= length()`) |
| `array->value->setLength(n)` | truncate (or grow with empty `nullptr` slots, saved as `null`) |
| `array->value->empty()` | remove all elements |

Example:

```cpp
TPointer<VArray> list;
list.newMemory();
list->value->push(VNumber::fromNumber(1));
list->value->push(VNumber::fromNumber(2));
list->value->push(VNumber::fromNumber(3));
list->value->remove(0);                                  // [2,3]
list->index(0) = VString::fromString("two");             // ["two",3]
list->value->insert(0) = TMemory<VNull>::newMemory();    // [null,"two",3]
```

To change a scalar you can also write its `value` directly:
`static_cast<VNumber *>(item)->value = 42;`. The node is shared by everyone
holding a pointer to it.

## Helpers worth writing

The library stays minimal; a few typed accessors make application code
short. These compile as they are:

```cpp
static VString *getString(VAssociativeArray *object, const char *key) {
	TPointer<Value> value;
	if (!object->get(key, value)) {
		return nullptr;
	};
	return TDynamicCast<VString *>(value);
};

static bool getNumber(VAssociativeArray *object, const char *key, NumberT &out) {
	TPointer<Value> value;
	if (!object->get(key, value)) {
		return false;
	};
	VNumber *number = TDynamicCast<VNumber *>(value);
	if (!number) {
		return false;
	};
	out = number->value;
	return true;
};

static VAssociativeArray *getOrCreateObject(VAssociativeArray *object, const char *key) {
	TPointer<Value> value;
	if (object->get(key, value)) {
		VAssociativeArray *child = TDynamicCast<VAssociativeArray *>(value);
		if (child) {
			return child;
		};
	};
	TPointer<VAssociativeArray> child;
	child.newMemory();
	object->set(key, child);
	return child; // owned by object from now on
};
```

Returning a raw pointer is fine here: the node is still held by `object`
after the local `TPointer` goes away.

Walking any document:

```cpp
static void walk(Value *value, size_t level) {
	if (!value) { // empty slot of a document built by hand, written as null
		printf("%*snull\n", (int)level * 2, "");
		return;
	};
	switch (value->getValueType()) {
	case ValueType::Null:
		printf("%*snull\n", (int)level * 2, "");
		break;
	case ValueType::Boolean:
		printf("%*s%s\n", (int)level * 2, "", static_cast<VBoolean *>(value)->value ? "true" : "false");
		break;
	case ValueType::Number:
		printf("%*s%g\n", (int)level * 2, "", static_cast<VNumber *>(value)->value);
		break;
	case ValueType::String:
		printf("%*s\"%s\"\n", (int)level * 2, "", static_cast<VString *>(value)->value.value());
		break;
	case ValueType::Array: {
		ArrayT *array = static_cast<VArray *>(value)->value;
		for (size_t k = 0; k < array->length(); ++k) {
			printf("%*s[%zu]\n", (int)level * 2, "", k);
			walk(array->index(k), level + 1);
		};
		break;
	};
	case ValueType::AssociativeArray: {
		AssociativeArrayT *object = static_cast<VAssociativeArray *>(value)->value;
		for (size_t k = 0; k < object->length(); ++k) {
			printf("%*s%s:\n", (int)level * 2, "", object->arrayKey->index(k).value());
			walk(object->arrayValue->index(k), level + 1);
		};
		break;
	};
	default:
		break;
	};
};
```

A document built by hand may contain empty (`nullptr`) slots (see below); a
document returned by `load` never does.

## Memory rules

The rules of `xyo-managed-memory` apply:

- **`TPointer<Value>`** for locals, parameters and return values: it keeps
  the whole tree alive.
- **Raw pointers** (`Value *`, `VString *`, ...) into the tree are fine while
  the tree holds the node. They dangle once the node is removed or replaced
  (`set` with the same key, `remove`, `empty`) and nothing else holds it.
  Take a `TPointer` if you need the node to outlive that.
- **As a class member** use `TPointerX<Value>` and link it in the
  constructor: `document.pointerLink(this);`.
- A node may appear in two places of the tree (it is a graph); it is
  written twice. A container that contains itself, directly or through
  its children (a cycle), is refused by the writer (`save` returns `false`
  once the nesting limit is reached); JSON has no way to express it.
- One thread per document, see [Getting started](getting-started.md#5-threads).

## Pitfalls

1. **Empty slots are written as `null`.** `array->value->set(5, x)` on an
   empty array, `index(k)` past the end, growing `setLength(n)` and
   `object->set(key, nullptr)` leave `nullptr` elements; the writer outputs
   them as JSON `null` (`[null,null,3]`). Reading the file back gives `VNull`
   nodes there, not `nullptr`. Code that walks a hand built tree must check
   for `nullptr` before calling `getValueType()`. (Empty containers are not
   affected: they are written as `[]` / `{}`.)
2. **Reading past the end with `index` / `[]` grows the array** with empty
   slots, which are then saved as `null`. Use `get(k, out)` or check
   `length()`.
3. **Numbers are `double`.** Integers above 2^53 (9007199254740992) lose
   precision on load; 64 bit ids should be stored as strings. `-0` is
   written as `0`.
4. **Strings may contain 0 bytes.** `"\u0000"` decodes to a 0 byte inside a
   `String` (its `length()` counts it). `String` comparisons and `.value()`
   with C functions stop at the first 0 byte.
5. **The reader does not validate UTF-8.** Bytes inside a JSON string are
   copied as they are (only `\u` escapes are checked), and written back as
   they are.
6. **`fromString` is lenient.** `VBoolean::fromString` gives `true` only for
   `"true"`; `VNumber::fromString` accepts `"NaN"`, `"Infinity"`,
   `"-Infinity"` and gives `0` for text it cannot parse. Use them to convert
   text you trust; `loadFromString` is the strict path.
7. **Duplicate keys in a file**: the last value wins, at the position of the
   first occurrence (`{"a":1,"b":2,"a":3}` reads as `{"a":3,"b":2}`).
