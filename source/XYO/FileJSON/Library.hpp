// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_LIBRARY_HPP
#define XYO_FILEJSON_LIBRARY_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

#ifndef XYO_FILEJSON_VNULL_HPP
#	include <XYO/FileJSON/VNull.hpp>
#endif

#ifndef XYO_FILEJSON_VBOOLEAN_HPP
#	include <XYO/FileJSON/VBoolean.hpp>
#endif

#ifndef XYO_FILEJSON_VNUMBER_HPP
#	include <XYO/FileJSON/VNumber.hpp>
#endif

#ifndef XYO_FILEJSON_VSTRING_HPP
#	include <XYO/FileJSON/VString.hpp>
#endif

#ifndef XYO_FILEJSON_VARRAY_HPP
#	include <XYO/FileJSON/VArray.hpp>
#endif

#ifndef XYO_FILEJSON_VASSOCIATIVEARRAY_HPP
#	include <XYO/FileJSON/VAssociativeArray.hpp>
#endif

namespace XYO::FileJSON {

	XYO_FILEJSON_EXPORT void initMemory();

};

#endif
