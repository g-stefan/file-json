# Getting started

## 1. Build and install

The library is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `xyo-platform`,
`xyo-managed-memory`, `xyo-data-structures`, `xyo-multithreading`,
`xyo-encoding` and `xyo-system` must be installed to the SDK first. From the
repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.*.cpp (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

Two libraries are produced:

| Project            | Kind                                | Use it when                               |
|--------------------|-------------------------------------|-------------------------------------------|
| `file-json`        | DLL / shared library (`dll-or-lib`) | default, shared between several libraries |
| `file-json.static` | static library, static CRT          | self-contained executables                |

The whole library is compiled; the only header only parts are the small
inline accessors of the value classes.

## 2. Depend on it from another fabricare project

In the consumer's `fabricare.json`:

```json
{
	"name": "my-application",
	"make": "exe",
	"sourcePath": "XYO/MyApplication",
	"dependency": [
		"file-json"
	]
}
```

For the static variant use `"file-json.static"`. It exports
`XYO_FILEJSON_LIBRARY` to the consumer (`dependencyDefines`), which turns
`XYO_FILEJSON_EXPORT` into nothing. `xyo-system` and everything below it
come in as transitive dependencies.

## 3. Include

```cpp
#include <XYO/FileJSON.hpp>
```

The umbrella header pulls in `<XYO/System.hpp>` (and through it the
encoding, multithreading, data structures, managed memory and platform
headers) and every public header of this library. C++17 is required; the
header stops with an error on an older standard.

Namespace `XYO::FileJSON` contains `using namespace` for
`XYO::ManagedMemory`, `XYO::DataStructures`, `XYO::Encoding` and
`XYO::System`, so `using namespace XYO::FileJSON;` also brings in `String`,
`TPointer`, `TMemory`, `TDynamicArray`, `TDynamicCast`, `File`, ... The
metadata namespaces (`Version`, `Copyright`, `License`) then exist several
times: write `XYO::FileJSON::Version::version()` in full.

In a library or application of your own, either use the namespace or
qualify (`FileJSON::load`, `FileJSON::VString`), as the XYO tools do:

```cpp
#include <XYO/FileJSON.hpp>

namespace XYO::MyApplication {
	using namespace XYO::FileJSON;
};
```

## 4. First program

Parse a document, read a few members, change it, print it and save it.

```cpp
#include <XYO/FileJSON.hpp>

using namespace XYO::FileJSON;

int main(int, char *[]) {
	TPointer<Value> document;
	if (!loadFromString("{\"name\": \"file-json\", \"version\": [6, 0, 0], \"stable\": true}", document)) {
		printf("invalid JSON\n");
		return 1;
	};

	VAssociativeArray *root = TDynamicCast<VAssociativeArray *>(document);
	if (!root) {
		printf("not an object\n");
		return 1;
	};

	TPointer<Value> value;
	if (root->get("name", value)) {
		VString *name = TDynamicCast<VString *>(value);
		if (name) {
			printf("name: %s\n", name->value.value());
		};
	};

	if (root->get("version", value)) {
		VArray *version = TDynamicCast<VArray *>(value);
		if (version) {
			for (size_t k = 0; k < version->length(); ++k) {
				VNumber *part = TDynamicCast<VNumber *>(version->index(k));
				if (part) {
					printf("version[%zu]: %g\n", k, part->value);
				};
			};
		};
	};

	root->set("stable", VBoolean::fromBoolean(false)); // replaced in place

	TPointer<VArray> tags;
	tags.newMemory();
	tags->value->push(VString::fromString("json"));
	tags->value->push(VString::fromString("c++"));
	root->set("tags", tags); // new key, added at the end

	String text;
	if (!saveToString(text, document, Mode::Indentation4Spaces)) {
		return 1;
	};
	printf("%s\n", text.value());

	if (!save("example.json", document)) {
		printf("cannot write example.json\n");
		return 1;
	};
	return 0;
};
```

Output:

```
name: file-json
version[0]: 6
version[1]: 0
version[2]: 0
{
    "name": "file-json",
    "version": [
        6,
        0,
        0
    ],
    "stable": false,
    "tags": [
        "json",
        "c++"
    ]
}
```

Things to notice, all explained in [Document model](document-model.md):

- every lookup is checked twice: does the member exist (`get` returns
  `bool`), and is it of the expected type (`TDynamicCast` returns
  `nullptr`);
- `root`, `name`, `version` are plain pointers into the document; they stay
  valid while `document` holds the tree;
- values are created with the `fromX` factories or `newMemory()`, and given
  to the tree with `set` / `push`; the tree keeps them alive;
- pass `String`s to `printf` with `.value()`.

As a fabricare test project:

```json
{
	"name": "test.03",
	"make": "exe",
	"category": "test",
	"SPDX-License-Identifier": "Unlicense",
	"dependency": [
		"file-json"
	]
}
```

with the source in `test/test.03.cpp`.

## 5. Threads

Everything in `xyo-managed-memory` about threads applies here:

- **a document belongs to the thread that created it.** Values come from
  per-thread active pools and are reference counted with plain integers. Do
  not share a document between threads, and do not release one on another
  thread. To hand a document to another thread, pass its text
  (`saveToString`, then copy the characters into a `std::string` or a
  `TMemorySystem` object) and `loadFromString` it on the receiving thread;
- create threads and move data between them with **`xyo-multithreading`**;
- the main thread must use the library before any thread starts. Call
  `XYO::ManagedMemory::Registry::registryInit()` at the start of `main` when
  in doubt. `XYO::FileJSON::initMemory()` creates the pools of all value
  classes of the current thread at once, in a fixed order; call it at the
  start of a thread when other pools you create there will hold JSON values.

`load`, `loadFromString`, `save` and `saveToString` have no shared state;
different threads can parse and write their own documents at the same time.

## 6. Building without fabricare

Compile the seven amalgams with your sources:

1. Put `source/` of `xyo-platform`, `xyo-managed-memory`,
   `xyo-data-structures`, `xyo-multithreading`, `xyo-encoding`,
   `xyo-system` and `file-json` on the include path.
2. Provide the configuration headers of `xyo-platform`, `xyo-managed-memory`
   and `xyo-system` (see their documentation; for the default configuration
   copy each `Config.Template.hpp` to `Config.hpp`).
3. Compile `Platform.Amalgam.cpp`, `ManagedMemory.Amalgam.cpp`,
   `DataStructures.Amalgam.cpp`, `Multithreading.Amalgam.cpp`,
   `Encoding.Amalgam.cpp`, `System.Amalgam.cpp` and `FileJSON.Amalgam.cpp`
   together with your sources, and define `XYO_PLATFORM_LIBRARY`,
   `XYO_MANAGEDMEMORY_LIBRARY`, `XYO_DATASTRUCTURES_LIBRARY`,
   `XYO_MULTITHREADING_LIBRARY`, `XYO_ENCODING_LIBRARY`,
   `XYO_SYSTEM_LIBRARY` and `XYO_FILEJSON_LIBRARY` everywhere (plain static
   linking).
4. Link `pthread` on Linux; `user32` on Windows (pulled in by a `#pragma`
   with MSVC).

Example on Linux, with the repositories side by side:

```bash
g++ -std=c++17 \
    -Ixyo-platform/source -Ixyo-managed-memory/source \
    -Ixyo-data-structures/source -Ixyo-multithreading/source \
    -Ixyo-encoding/source -Ixyo-system/source -Ifile-json/source \
    -DXYO_PLATFORM_LIBRARY -DXYO_MANAGEDMEMORY_LIBRARY \
    -DXYO_DATASTRUCTURES_LIBRARY -DXYO_MULTITHREADING_LIBRARY \
    -DXYO_ENCODING_LIBRARY -DXYO_SYSTEM_LIBRARY -DXYO_FILEJSON_LIBRARY \
    xyo-platform/source/XYO/Platform.Amalgam.cpp \
    xyo-managed-memory/source/XYO/ManagedMemory.Amalgam.cpp \
    xyo-data-structures/source/XYO/DataStructures.Amalgam.cpp \
    xyo-multithreading/source/XYO/Multithreading.Amalgam.cpp \
    xyo-encoding/source/XYO/Encoding.Amalgam.cpp \
    xyo-system/source/XYO/System.Amalgam.cpp \
    file-json/source/XYO/FileJSON.Amalgam.cpp \
    main.cpp -o main -pthread
```

On Windows with MSVC, also define `XYO_PLATFORM_COMPILE_STATIC`:

```bat
cl /EHsc /std:c++17 ^
   /Ixyo-platform\source /Ixyo-managed-memory\source ^
   /Ixyo-data-structures\source /Ixyo-multithreading\source ^
   /Ixyo-encoding\source /Ixyo-system\source /Ifile-json\source ^
   /DXYO_PLATFORM_COMPILE_STATIC /DXYO_PLATFORM_LIBRARY /DXYO_MANAGEDMEMORY_LIBRARY ^
   /DXYO_DATASTRUCTURES_LIBRARY /DXYO_MULTITHREADING_LIBRARY ^
   /DXYO_ENCODING_LIBRARY /DXYO_SYSTEM_LIBRARY /DXYO_FILEJSON_LIBRARY ^
   xyo-platform\source\XYO\Platform.Amalgam.cpp ^
   xyo-managed-memory\source\XYO\ManagedMemory.Amalgam.cpp ^
   xyo-data-structures\source\XYO\DataStructures.Amalgam.cpp ^
   xyo-multithreading\source\XYO\Multithreading.Amalgam.cpp ^
   xyo-encoding\source\XYO\Encoding.Amalgam.cpp ^
   xyo-system\source\XYO\System.Amalgam.cpp ^
   file-json\source\XYO\FileJSON.Amalgam.cpp ^
   main.cpp
```

With an installed SDK (`~/.fabricare/<platform>`) you can instead compile
only your sources against `include/` and link the `*.static` libraries with
the static CRT (`/MT`): `file-json.static`, `xyo-system.static`,
`xyo-multithreading.static`, `xyo-encoding.static`,
`xyo-data-structures.static`, `xyo-managed-memory.static`,
`xyo-platform.static`, defining the matching `XYO_*_LIBRARY` macros.
