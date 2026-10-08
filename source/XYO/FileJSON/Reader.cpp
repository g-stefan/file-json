// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Library.hpp>
#include <XYO/FileJSON/Reader.hpp>
#include <XYO/FileJSON/Token.hpp>

namespace XYO::FileJSON {

	struct Reader {
			Token &token;
			String scratch;
			size_t depth;

			inline Reader(Token &token_) : token(token_), depth(0) {};

			bool readValue(TPointer<Value> &result);
			bool readArray(TPointer<Value> &result);
			bool readAssociativeArray(TPointer<Value> &result);
			bool readDocument(TPointer<Value> &result);
	};

	// current character is '['
	bool Reader::readArray(TPointer<Value> &result) {
		if (++depth > XYO_FILEJSON_MAX_DEPTH) {
			return false;
		};
		token.read();
		TPointer<VArray> vArray;
		vArray.newMemory();
		result = vArray;
		token.ignoreSpace();
		if (token.is1(']')) {
			--depth;
			return true;
		};
		size_t index = 0;
		for (;;) {
			TPointer<Value> value;
			if (!readValue(value)) {
				return false;
			};
			vArray->value->set(index++, value);
			token.ignoreSpace();
			if (token.is1(',')) {
				token.ignoreSpace();
				continue;
			};
			if (token.is1(']')) {
				--depth;
				return true;
			};
			return false;
		};
	};

	// current character is '{'
	bool Reader::readAssociativeArray(TPointer<Value> &result) {
		if (++depth > XYO_FILEJSON_MAX_DEPTH) {
			return false;
		};
		token.read();
		TPointer<VAssociativeArray> vAssociativeArray;
		vAssociativeArray.newMemory();
		result = vAssociativeArray;
		token.ignoreSpace();
		if (token.is1('}')) {
			--depth;
			return true;
		};
		String key;
		for (;;) {
			if (!token.isString(key)) {
				return false;
			};
			token.ignoreSpace();
			if (!token.is1(':')) {
				return false;
			};
			token.ignoreSpace();
			TPointer<Value> value;
			if (!readValue(value)) {
				return false;
			};
			vAssociativeArray->value->set(key, value);
			token.ignoreSpace();
			if (token.is1(',')) {
				token.ignoreSpace();
				continue;
			};
			if (token.is1('}')) {
				--depth;
				return true;
			};
			return false;
		};
	};

	// current character is the first character of the value
	bool Reader::readValue(TPointer<Value> &result) {
		switch (token.input.input) {
		case '{':
			return readAssociativeArray(result);
		case '[':
			return readArray(result);
		case '\"':
			if (token.isString(scratch)) {
				result = VString::fromString(scratch);
				return true;
			};
			return false;
		case 't':
			if (token.isN("true")) {
				result = VBoolean::fromBoolean(true);
				return true;
			};
			return false;
		case 'f':
			if (token.isN("false")) {
				result = VBoolean::fromBoolean(false);
				return true;
			};
			return false;
		case 'n':
			if (token.isN("null")) {
				result = TMemory<VNull>::newMemory();
				return true;
			};
			return false;
		default:
			break;
		};
		if (token.isNumber(scratch)) {
			TPointer<VNumber> vNumber(TMemory<VNumber>::newMemory());
			if (!VNumber::fromChars(scratch.value(), scratch.length(), vNumber->value)) {
				return false;
			};
			result = vNumber;
			return true;
		};
		return false;
	};

	bool Reader::readDocument(TPointer<Value> &result) {
		if (!token.read()) {
			return false;
		};
		token.isBOM();
		token.ignoreSpace();
		if (!readValue(result)) {
			return false;
		};
		token.ignoreSpace();
		// nothing but whitespace may follow the value
		return token.isEof();
	};

	static bool readDocument(IRead *iRead, TPointer<Value> &document) {
		Token token;
		Reader reader(token);
		TPointer<Value> result;
		token.setIRead(iRead);
		if (reader.readDocument(result)) {
			document = result;
			return true;
		};
		document = nullptr;
		return false;
	};

	bool load(const char *fileName, TPointer<Value> &document) {
		File file;
		if (!file.openRead(fileName)) {
			document = nullptr;
			return false;
		};
		return readDocument(&file, document);
	};

	bool loadFromString(const char *value, TPointer<Value> &document) {
		MemoryRead memory;
		if ((value == nullptr) || !memory.open(value, strlen(value))) {
			document = nullptr;
			return false;
		};
		return readDocument(&memory, document);
	};

};
