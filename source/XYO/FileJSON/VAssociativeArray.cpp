// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/VAssociativeArray.hpp>

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VAssociativeArray, "{6D88E885-5D8E-4BB5-8EEC-32AAA4942359}");

	VAssociativeArray::VAssociativeArray() {
		XYO_DYNAMIC_TYPE_PUSH(VAssociativeArray);

		value.pointerLink(this);
		value.newMemory();
	};

};
