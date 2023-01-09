// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#define XYO_FILEJSON_DEPENDENCY_HPP

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
#	define XYO_FILEJSON_EXPORT XYO_LIBRARY_EXPORT
#else
#	define XYO_FILEJSON_EXPORT XYO_LIBRARY_IMPORT
#endif

// --

namespace XYO::FileJSON {
	using namespace XYO::ManagedMemory;
	using namespace XYO::DataStructures;
	using namespace XYO::Encoding;
	using namespace XYO::System;
};

#endif
