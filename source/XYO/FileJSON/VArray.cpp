// File JSON
// Copyright (c) 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/VArray.hpp>

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VArray, "{3ECCB902-C928-4DBD-AD23-815EF4BFA6AF}");

	VArray::VArray() {
		XYO_DYNAMIC_TYPE_PUSH(VArray);

		value.pointerLink(this);
		value.newMemory();
	};

};
