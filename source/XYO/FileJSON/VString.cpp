// File JSON
// Copyright (c) 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/VString.hpp>

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VString, "{37AA6B34-DC44-446A-B425-601FF347C813}");

	VString::VString() {
		XYO_DYNAMIC_TYPE_PUSH(VString);
	};

	String VString::toString() {
		return value;
	};

	TPointer<VString> VString::fromString(const String &value) {
		TPointer<VString> retV(TMemory<VString>::newMemory());
		retV->value = value;
		return retV;
	};

};
