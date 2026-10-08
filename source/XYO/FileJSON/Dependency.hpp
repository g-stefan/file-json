// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#define XYO_FILEJSON_DEPENDENCY_HPP

// C++17 is required (nested namespaces, <charconv>),
// MSVC reports the standard in _MSVC_LANG, __cplusplus stays 199711L without /Zc:__cplusplus
#if defined(_MSVC_LANG)
#	if _MSVC_LANG < 201703L
#		error "File JSON requires C++17 or newer"
#	endif
#elif __cplusplus < 201703L
#	error "File JSON requires C++17 or newer"
#endif

#ifndef XYO_SYSTEM_HPP
#	include <XYO/System.hpp>
#endif

// -- Export

#ifndef XYO_FILEJSON_INTERNAL
#	ifdef FILE_JSON_INTERNAL
#		define XYO_FILEJSON_INTERNAL
#	endif
#endif

#ifdef XYO_FILEJSON_INTERNAL
#	define XYO_FILEJSON_EXPORT XYO_PLATFORM_LIBRARY_EXPORT
#else
#	define XYO_FILEJSON_EXPORT XYO_PLATFORM_LIBRARY_IMPORT
#endif
#ifdef XYO_FILEJSON_LIBRARY
#	undef XYO_FILEJSON_EXPORT
#	define XYO_FILEJSON_EXPORT
#endif

// --

// Maximum nesting of arrays/objects accepted by the reader and writer,
// guards against stack overflow on hostile input or cyclic documents
#ifndef XYO_FILEJSON_MAX_DEPTH
#	define XYO_FILEJSON_MAX_DEPTH 256
#endif

namespace XYO::FileJSON {
	using namespace XYO::ManagedMemory;
	using namespace XYO::DataStructures;
	using namespace XYO::Encoding;
	using namespace XYO::System;
};

#endif
