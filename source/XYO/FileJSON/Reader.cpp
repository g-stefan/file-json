// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Library.hpp>
#include <XYO/FileJSON/Reader.hpp>
#include <XYO/FileJSON/Token.hpp>

namespace XYO::FileJSON {

	bool read(Token &token, TPointer<Value> &result);
	bool readArray(Token &token, TPointer<Value> &result);
	bool readAssociativeArray(Token &token, TPointer<Value> &result);

	bool readArray(Token &token, TPointer<Value> &result) {
		token.ignoreSpace();
		if (token.is1('[')) {
			size_t index = 0;
			TPointer<VArray> vArray;
			vArray.newMemory();
			result = vArray;
			while (!token.isEof()) {
				token.ignoreSpace();
				if (token.is1(']')) {
					return true;
				};
				if (token.is1(',')) {
					continue;
				};
				TPointer<Value> value;
				if (read(token, value)) {
					vArray->value->set(index++, value);
					continue;
				};
				break;
			};
		};
		return false;
	};

	bool readAssociativeArray(Token &token, TPointer<Value> &result) {
		token.ignoreSpace();
		if (token.is1('{')) {
			String key;
			TPointer<VAssociativeArray> vAssociativeArray;
			vAssociativeArray.newMemory();
			result = vAssociativeArray;
			while (!token.isEof()) {
				token.ignoreSpace();
				if (token.is1('}')) {
					return true;
				};
				if (token.is1(',')) {
					continue;
				};
				if (token.isString(key)) {
					token.ignoreSpace();
					if (token.is1(':')) {
						token.ignoreSpace();
						TPointer<Value> value;
						if (read(token, value)) {
							vAssociativeArray->value->set(key, value);
							continue;
						};
					};
				};
				break;
			};
		};
		return false;
	};

	bool read(Token &token, TPointer<Value> &result) {
		token.ignoreSpace();
		String value;
		if (token.is('{')) {
			return readAssociativeArray(token, result);
		};
		if (token.is('[')) {
			return readArray(token, result);
		};
		if (token.isString(value)) {
			result = VString::fromString(value);
			return true;
		};
		if (token.isNumber(value)) {
			result = VNumber::fromString(value);
			return true;
		};
		if (token.isN("true")) {
			result = VBoolean::fromBoolean(true);
			return true;
		};
		if (token.isN("false")) {
			result = VBoolean::fromBoolean(false);
			return true;
		};
		if (token.isN("null")) {
			TPointer<VNull> vNull;
			vNull.newMemory();
			result = vNull;
			return true;
		};
		return false;
	};

	bool load(const char *fileName, TPointer<Value> &document) {
		Token token;
		File file;
		if (file.openRead(fileName)) {
			token.setIRead(&file);
			if (token.read()) {
				read(token, document);
				token.ignoreSpace();
				return token.isEof();
			};
		};
		return false;
	};

	bool loadFromString(const char *value, TPointer<Value> &document) {
		Token token;
		MemoryRead memory;
		if (memory.open(value, strlen(value))) {
			token.setIRead(&memory);
			if (token.read()) {
				read(token, document);
				token.ignoreSpace();
				return token.isEof();
			};
		};
		return false;
	};

};
