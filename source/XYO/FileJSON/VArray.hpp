// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_VARRAY_HPP
#define XYO_FILEJSON_VARRAY_HPP

#ifndef XYO_FILEJSON_VALUE_HPP
#	include <XYO/FileJSON/Value.hpp>
#endif

namespace XYO::FileJSON {
	class VArray;

};

namespace XYO::ManagedMemory {
	template <>
	struct TMemory<XYO::FileJSON::VArray> : TMemoryPoolActive<XYO::FileJSON::VArray> {};
};

namespace XYO::FileJSON {

	typedef TDynamicArray<TPointerX<Value>, 4, TMemoryPoolActive> ArrayT;

	class VArray : public Value {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(VArray);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_FILEJSON_EXPORT, VArray);

		public:
			TPointerX<ArrayT> value;

			XYO_FILEJSON_EXPORT VArray();

			inline void activeDestructor() {
				value->activeDestructor();
			};

			static inline void initMemory() {
				Value::initMemory();
				ArrayT::initMemory();
			};

			inline size_t length() const {
				return value->length();
			};

			inline TPointerX<Value> &index(size_t idx) {
				return value->index(idx);
			};

			inline TPointerX<Value> &operator[](int idx) {
				return value->index((size_t)idx);
			};

			inline bool get(size_t idx, TPointer<Value> &x) {
				return value->get(idx, x);
			};

			inline bool get(size_t idx, TPointerX<Value> &x) {
				return value->get(idx, x);
			};
	};

};

#endif
