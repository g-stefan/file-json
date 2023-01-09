// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_TOKEN_HPP
#define XYO_FILEJSON_TOKEN_HPP

#ifndef XYO_FILEJSON_INPUT_HPP
#	include <XYO/FileJSON/Input.hpp>
#endif

namespace XYO::FileJSON {

	class Token {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(Token);

		public:
			Input input;

			XYO_FILEJSON_EXPORT Token();
			XYO_FILEJSON_EXPORT ~Token();
			XYO_FILEJSON_EXPORT void activeDestructor();

			inline void setIRead(IRead *value) {
				input.setIRead(value);
			};

			XYO_FILEJSON_EXPORT bool isBOM();
			XYO_FILEJSON_EXPORT bool isSpace();
			XYO_FILEJSON_EXPORT void ignoreSpace();
			XYO_FILEJSON_EXPORT bool isString(String &token);
			XYO_FILEJSON_EXPORT bool isNumber(String &token);

			inline bool is(const char inputChar) {
				// printf("%02X=%02X(%c,%c)",(char)input,inputChar,(char)input,inputChar);
				return (((char)input) == inputChar);
			};

			inline bool is1(const char inputChar) {
				if (is(inputChar)) {
					return input.read();
				};
				return false;
			};

			XYO_FILEJSON_EXPORT bool isN(const char *token);

			inline bool read() {
				return input.read();
			};

			inline bool isEof() {
				return input.isEof();
			};

			inline bool between(const char inputChar_A, const char inputChar_B) {
				return ((input >= inputChar_A) && (input <= inputChar_B));
			};
	};

};

#endif
