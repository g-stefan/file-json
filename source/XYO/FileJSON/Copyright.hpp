// File JSON
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_COPYRIGHT_HPP
#define XYO_FILEJSON_COPYRIGHT_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

namespace XYO::FileJSON::Copyright {

	XYO_FILEJSON_EXPORT const char *copyright();
	XYO_FILEJSON_EXPORT const char *publisher();
	XYO_FILEJSON_EXPORT const char *company();
	XYO_FILEJSON_EXPORT const char *contact();

};

#endif
