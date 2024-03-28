// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VASSOCIATIVEARRAY_HPP
#define XYO_FILEJSON_VASSOCIATIVEARRAY_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {
	class VAssociativeArray;
};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VAssociativeArray> : TMemoryPoolActive<XYO::FileJSON::VAssociativeArray> {};
};

namespace XYO::FileJSON {

	typedef TAssociativeArray<String, TPointerX<Value>, 4, TMemoryPoolActive> AssociativeArrayT;

	class VAssociativeArray : public Value {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(VAssociativeArray);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VAssociativeArray);

		public:
			TPointerX<AssociativeArrayT> value;

			XYO_FILEJSON_EXPORT VAssociativeArray();

			inline void activeDestructor() {
				value->activeDestructor();
			};

			static inline void initMemory() {
				Value::initMemory();
				AssociativeArrayT::initMemory();
			};

			inline void set(const String &key, const Value *x) {
				value->set(key, x);
			};

			inline bool get(const String &key, TPointer<Value> &x) {
				return value->get(key, x);
			};

			inline bool get(const String &key, TPointerX<Value> &x) {
				return value->get(key, x);
			};
	};

};

#endif
