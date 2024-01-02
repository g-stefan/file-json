// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Library.hpp>

namespace XYO::FileJSON {

	void initMemory() {
		TMemory<Value>::initMemory();
		TMemory<VNull>::initMemory();
		TMemory<VBoolean>::initMemory();
		TMemory<VNumber>::initMemory();
		TMemory<VString>::initMemory();
		TMemory<VArray>::initMemory();
		TMemory<VAssociativeArray>::initMemory();
	};

};
