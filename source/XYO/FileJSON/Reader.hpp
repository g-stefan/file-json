// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_READER_HPP
#define XYO_FILEJSON_READER_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {

	XYO_FILEJSON_EXPORT bool load(const char *fileName, TPointer<Value> &document);
	XYO_FILEJSON_EXPORT bool loadFromString(const char *value, TPointer<Value> &document);

};

#endif
