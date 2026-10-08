// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_FILEJSON_INPUT_HPP
#define XYO_FILEJSON_INPUT_HPP

#ifndef XYO_FILEJSON_DEPENDENCY_HPP
#	include <XYO/FileJSON/Dependency.hpp>
#endif

namespace XYO::FileJSON {

	// Buffered single character lookahead over an IRead,
	// the IRead is not owned and must outlive the Input
	class Input {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(Input);

		protected:
			static constexpr size_t bufferSize = 4096;
			char buffer[bufferSize];
			size_t bufferIndex;
			size_t bufferLength;

			XYO_FILEJSON_EXPORT bool fill();

		public:
			IRead *iRead;
			char input;
			bool eof;

			// count of characters read
			size_t fileIndex;

			XYO_FILEJSON_EXPORT Input();

			inline operator char() const {
				return input;
			};

			inline char value() const {
				return input;
			};

			XYO_FILEJSON_EXPORT void setIRead(IRead *value);

			// advance to the next character, at end of input sets eof and input to 0
			inline bool read() {
				if (bufferIndex < bufferLength) {
					input = buffer[bufferIndex++];
					++fileIndex;
					return true;
				};
				return fill();
			};

			inline bool isEof() const {
				return eof;
			};
	};

};

#endif
