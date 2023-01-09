// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/VNull.hpp>

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VNull, "{2CF03903-E324-49DE-8B96-E3FF8526768E}");

	VNull::VNull() {
		XYO_DYNAMIC_TYPE_PUSH(VNull);
	};

	String VNull::toString() {
		return "null";
	};

};
