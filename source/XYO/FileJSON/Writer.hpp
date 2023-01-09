// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_WRITER_HPP
#define XYO_FILEJSON_WRITER_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

#ifndef XYO_FILEJSON_MODE_HPP
#	include <XYO/FileJSON/Mode.hpp>
#endif

namespace XYO::FileJSON {

	XYO_FILEJSON_EXPORT bool save(const char *fileName, Value *document, Mode mode = Mode::IndentationTab);
	XYO_FILEJSON_EXPORT bool saveToString(String &output, Value *document, Mode mode = Mode::IndentationTab);

};

#endif
