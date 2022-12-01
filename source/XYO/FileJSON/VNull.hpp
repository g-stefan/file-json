// File JSON
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VNULL_HPP
#define XYO_FILEJSON_VNULL_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {
	class VNull;
}

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VNull> : TMemoryPoolActive<XYO::FileJSON::VNull> {};
};

namespace XYO::FileJSON {

	class VNull : public Value {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(VNull);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VNull);

		public:
			XYO_FILEJSON_EXPORT VNull();

			XYO_FILEJSON_EXPORT String toString();
	};

};

#endif
