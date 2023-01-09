// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VALUE_HPP
#define XYO_FILEJSON_VALUE_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

namespace XYO::FileJSON {

	class Value;

};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::Value> : TMemoryPoolActive<XYO::FileJSON::Value> {};
};

namespace XYO::FileJSON {

	class Value : public DynamicObject {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(Value);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, Value);

		public:
			XYO_FILEJSON_EXPORT Value();
	};

};

#endif
