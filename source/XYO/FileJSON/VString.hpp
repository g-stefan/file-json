// File JSON
// Copyright (c) 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VSTRING_HPP
#define XYO_FILEJSON_VSTRING_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {

	class VString;
};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VString> : TMemoryPoolActive<XYO::FileJSON::VString> {};
};

namespace XYO::FileJSON {

	typedef String StringT;

	class VString : public Value {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(VString);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VString);

		public:
			StringT value;

			XYO_FILEJSON_EXPORT VString();

			inline void activeDestructor() {
				value.activeDestructor();
			};

			static inline void initMemory() {
				DynamicObject::initMemory();
				String::initMemory();
			};

			XYO_FILEJSON_EXPORT String toString();
			XYO_FILEJSON_EXPORT static TPointer<VString> fromString(const String &value);
	};

};

#endif
