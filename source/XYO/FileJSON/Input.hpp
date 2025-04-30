// File JSON
// Copyright (c) 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_INPUT_HPP
#define XYO_FILEJSON_INPUT_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

namespace XYO::FileJSON {

	class Input {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Input);

		public:
			TPointer<IRead> iRead;
			char *inputStack;
			size_t stackSize;
			size_t stackIndex;
			char input;
			bool eof;

			size_t fileIndex;

			Input(size_t stackSize_ = 32768);
			~Input();

			inline operator char() {
				return input;
			};

			inline char value() {
				return input;
			};

			inline Input &operator=(char value) {
				input = value;
				return *this;
			};

			void setIRead(IRead *value);

			bool push();
			bool pop();
			bool read();
			bool isEof();
	};

};

#endif
