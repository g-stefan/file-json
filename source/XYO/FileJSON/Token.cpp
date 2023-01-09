// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MITisN

#include <XYO/FileJSON/Token.hpp>

namespace XYO::FileJSON {

	Token::Token(){};

	Token::~Token(){};

	void Token::activeDestructor() {
		input.setIRead(nullptr);
	};

	bool Token::isN(const char *name) {
		int k;
		for (k = 0; name[k] != 0; ++k) {
			if (input == name[k]) {
				if (input.read()) {
					continue;
				};
			};
			break;
		};
		if (name[k] == 0) {
			return true;
		};
		for (--k; k >= 0; --k) {
			input.push();
			input = name[k];
		};
		return false;
	};

	bool Token::isBOM() {
		if (isN("\xEF\xBB\xBF")) {
			return true;
		};
		return false;
	};

	bool Token::isSpace() {
		bool isOk = false;
		while (!isEof()) {

			if (is1('\x20')) {
				isOk = true;
				continue;
			};

			if (is1('\x09')) {
				isOk = true;
				continue;
			};

			if (is1('\x0D')) {
				isOk = true;
				continue;
			};

			if (is1('\x0A')) {
				isOk = true;
				continue;
			};

			return isOk;
		};
		return false;
	};

	void Token::ignoreSpace() {
		while (isBOM() || isSpace()) {
		};
	};

	bool Token::isString(String &token) {
		if (is('\"')) {
			token.empty();
			while (input.read()) {
				if (input != '\"') {
					if (is('\\')) {
						if (input.read()) {
							if (is('\"')) {
								token << '\"';
								continue;
							};
							if (is('\\')) {
								token << '\\';
								continue;
							};
							if (is('/')) {
								token << '/';
								continue;
							};
							if (is('b')) {
								token << '\x08';
								continue;
							};
							if (is('f')) {
								token << '\x0C';
								continue;
							};
							if (is('n')) {
								token << '\x0A';
								continue;
							};
							if (is('r')) {
								token << '\x0D';
								continue;
							};
							if (is('t')) {
								token << '\x09';
								continue;
							};
							if (is('u')) {
								char code[5];
								String codeScan;
								unsigned int code4;
								if (!input.read()) {
									return false;
								};
								code[0] = input;
								if (!input.read()) {
									return false;
								};
								code[1] = input;
								if (!input.read()) {
									return false;
								};
								code[2] = input;
								if (!input.read()) {
									return false;
								};
								code[3] = input;
								code[4] = 0;
								if (sscanf(code, "%04X", &code4) != 1) {
									return false;
								};
								code[0] = (code4 >> 8) & 0xFF;
								code[1] = (code4)&0xFF;
								code[2] = 0;
								token.concatenate(code, 2);
								continue;
							};
							return false;
						} else {
							return false;
						};
					};

					token << input;

				} else {
					input.read();
					ignoreSpace();
					return true;
				};
			};
		};
		return false;
	};

	bool Token::isNumber(String &token) {
		if (is('-') || between('0', '9')) {
			token.empty();
			token << input;
			while (input.read()) {
				if (between('0', '9')) {
					token << input;
					continue;
				};
				break;
			};
			if (is('.')) {
				if (input.read()) {
					if (between('0', '9')) {
						token << '.';
						token << input;
						while (input.read()) {
							if (between('0', '9')) {
								token << input;
								continue;
							};
							break;
						};
						if (is('e') || is('E')) {
							token << input;
							if (input.read()) {
								if (is('+') || is('-')) {
									token << input;
									while (input.read()) {
										if (between('0', '9')) {
											token << input;
											continue;
										};
										input.push();
										break;
									};
									return true;
								};
								input.push();
							};
							return true;
						};
						input.push();
						return true;
					};
					input.push();
					input = '.';
				};
				input.push();
			};
			return true;
		};
		return false;
	};

};
