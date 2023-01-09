// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VBOOLEAN_HPP
#define XYO_FILEJSON_VBOOLEAN_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {
	class VBoolean;
};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VBoolean> : TMemoryPoolActive<XYO::FileJSON::VBoolean> {};
};

namespace XYO::FileJSON {

	typedef bool BooleanT;

	class VBoolean : public Value {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(VBoolean);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VBoolean);

		public:
			BooleanT value;

			XYO_FILEJSON_EXPORT VBoolean();

			inline void activeDestructor() {
				value = false;
			};

			XYO_FILEJSON_EXPORT String toString();
			XYO_FILEJSON_EXPORT static TPointer<VBoolean> fromBoolean(BooleanT value);
			XYO_FILEJSON_EXPORT static TPointer<VBoolean> fromString(const String &value);
	};

};

#endif
