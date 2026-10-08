// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VNUMBER_HPP
#define XYO_FILEJSON_VNUMBER_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {
	class VNumber;
};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VNumber> : TMemoryPoolActive<XYO::FileJSON::VNumber> {};
};

namespace XYO::FileJSON {

	typedef double NumberT;

	class VNumber : public Value {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(VNumber);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VNumber);

		public:
			NumberT value;

			XYO_FILEJSON_EXPORT VNumber();

			inline void activeDestructor() {
				value = 0;
			};

			XYO_FILEJSON_EXPORT String toString();
			XYO_FILEJSON_EXPORT static TPointer<VNumber> fromNumber(NumberT value);
			XYO_FILEJSON_EXPORT static TPointer<VNumber> fromString(const String &value);

			// Shortest text that reads back to the same finite value, locale independent,
			// buffer must hold at least NumberBufferSize chars, returns length (0 if not finite)
			static constexpr size_t NumberBufferSize = 64;
			XYO_FILEJSON_EXPORT static size_t toChars(NumberT value, char *buffer);
			// Parse a JSON number (locale independent), false if the text is not fully consumed
			XYO_FILEJSON_EXPORT static bool fromChars(const char *text, size_t length, NumberT &value);
	};

};

#endif
