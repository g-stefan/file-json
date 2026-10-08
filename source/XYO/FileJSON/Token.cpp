// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Token.hpp>

namespace XYO::FileJSON {

	// Collects characters in chunks, to avoid growing the string one character at a time
	struct TokenBuffer {
			String &token;
			char chunk[256];
			size_t length;

			inline TokenBuffer(String &token_) : token(token_), length(0) {};

			inline void flush() {
				if (length) {
					token.concatenate(chunk, length);
					length = 0;
				};
			};

			inline void add(char value) {
				if (length == sizeof(chunk)) {
					flush();
				};
				chunk[length++] = value;
			};

			inline void addUTF8(uint32_t code) {
				if (code < 0x80) {
					add(static_cast<char>(code));
					return;
				};
				if (code < 0x800) {
					add(static_cast<char>(0xC0 | (code >> 6)));
					add(static_cast<char>(0x80 | (code & 0x3F)));
					return;
				};
				if (code < 0x10000) {
					add(static_cast<char>(0xE0 | (code >> 12)));
					add(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
					add(static_cast<char>(0x80 | (code & 0x3F)));
					return;
				};
				add(static_cast<char>(0xF0 | (code >> 18)));
				add(static_cast<char>(0x80 | ((code >> 12) & 0x3F)));
				add(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
				add(static_cast<char>(0x80 | (code & 0x3F)));
			};
	};

	// Reads the 4 hex digits of a \uXXXX escape
	static bool readHex4(Input &input, uint32_t &value) {
		value = 0;
		for (int k = 0; k < 4; ++k) {
			if (!input.read()) {
				return false;
			};
			char digit = input.input;
			uint32_t x;
			if ((digit >= '0') && (digit <= '9')) {
				x = digit - '0';
			} else if ((digit >= 'a') && (digit <= 'f')) {
				x = digit - 'a' + 10;
			} else if ((digit >= 'A') && (digit <= 'F')) {
				x = digit - 'A' + 10;
			} else {
				return false;
			};
			value = (value << 4) | x;
		};
		return true;
	};

	Token::Token() {};

	Token::~Token() {};

	void Token::activeDestructor() {
		input.setIRead(nullptr);
	};

	bool Token::isN(const char *name) {
		for (int k = 0; name[k] != 0; ++k) {
			if (!is(name[k])) {
				return false;
			};
			input.read();
		};
		return true;
	};

	bool Token::isBOM() {
		return isN("\xEF\xBB\xBF");
	};

	bool Token::isSpace() {
		bool isOk = false;
		while (is('\x20') || is('\x09') || is('\x0D') || is('\x0A')) {
			isOk = true;
			input.read();
		};
		return isOk;
	};

	void Token::ignoreSpace() {
		isSpace();
	};

	bool Token::isString(String &token) {
		if (!is('\"')) {
			return false;
		};
		token.empty();
		TokenBuffer buffer(token);
		for (;;) {
			if (!input.read()) {
				return false;
			};
			if (is('\"')) {
				break;
			};
			// control characters must be escaped
			if (static_cast<unsigned char>(input.input) < 0x20) {
				return false;
			};
			if (!is('\\')) {
				buffer.add(input.input);
				continue;
			};
			if (!input.read()) {
				return false;
			};
			switch (input.input) {
			case '\"':
				buffer.add('\"');
				break;
			case '\\':
				buffer.add('\\');
				break;
			case '/':
				buffer.add('/');
				break;
			case 'b':
				buffer.add('\x08');
				break;
			case 'f':
				buffer.add('\x0C');
				break;
			case 'n':
				buffer.add('\x0A');
				break;
			case 'r':
				buffer.add('\x0D');
				break;
			case 't':
				buffer.add('\x09');
				break;
			case 'u': {
				uint32_t code;
				if (!readHex4(input, code)) {
					return false;
				};
				if ((code >= 0xD800) && (code <= 0xDBFF)) {
					// high surrogate, must be followed by a low surrogate
					uint32_t low;
					if (!input.read() || !is('\\')) {
						return false;
					};
					if (!input.read() || !is('u')) {
						return false;
					};
					if (!readHex4(input, low)) {
						return false;
					};
					if ((low < 0xDC00) || (low > 0xDFFF)) {
						return false;
					};
					code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
				} else if ((code >= 0xDC00) && (code <= 0xDFFF)) {
					// lone low surrogate
					return false;
				};
				buffer.addUTF8(code);
				break;
			};
			default:
				return false;
			};
		};
		buffer.flush();
		input.read();
		return true;
	};

	// -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?
	bool Token::isNumber(String &token) {
		if (!is('-') && !between('0', '9')) {
			return false;
		};
		token.empty();
		TokenBuffer buffer(token);
		if (is('-')) {
			buffer.add('-');
			input.read();
		};
		if (is('0')) {
			buffer.add('0');
			input.read();
			// no leading zeros
			if (between('0', '9')) {
				return false;
			};
		} else if (between('1', '9')) {
			do {
				buffer.add(input.input);
			} while (input.read() && between('0', '9'));
		} else {
			return false;
		};
		if (is('.')) {
			buffer.add('.');
			input.read();
			if (!between('0', '9')) {
				return false;
			};
			do {
				buffer.add(input.input);
			} while (input.read() && between('0', '9'));
		};
		if (is('e') || is('E')) {
			buffer.add('e');
			input.read();
			if (is('+') || is('-')) {
				buffer.add(input.input);
				input.read();
			};
			if (!between('0', '9')) {
				return false;
			};
			do {
				buffer.add(input.input);
			} while (input.read() && between('0', '9'));
		};
		buffer.flush();
		return true;
	};

};
