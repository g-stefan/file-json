// File JSON
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_LICENSE_HPP
#define XYO_FILEJSON_LICENSE_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

namespace XYO::FileJSON::License {

	XYO_FILEJSON_EXPORT const char *licenseHeader();
	XYO_FILEJSON_EXPORT const char *licenseBody();
	XYO_FILEJSON_EXPORT const char *shortLicense();

};

#endif
