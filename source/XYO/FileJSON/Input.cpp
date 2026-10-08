// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Input.hpp>

namespace XYO::FileJSON {

	Input::Input() {
		iRead = nullptr;
		bufferIndex = 0;
		bufferLength = 0;
		input = 0;
		eof = false;
		fileIndex = 0;
	};

	void Input::setIRead(IRead *value) {
		iRead = value;
		bufferIndex = 0;
		bufferLength = 0;
		input = 0;
		eof = false;
		fileIndex = 0;
	};

	bool Input::fill() {
		input = 0;
		if (eof || (iRead == nullptr)) {
			eof = true;
			return false;
		};
		bufferIndex = 0;
		bufferLength = iRead->read(buffer, bufferSize);
		if (bufferLength == 0) {
			eof = true;
			return false;
		};
		input = buffer[bufferIndex++];
		++fileIndex;
		return true;
	};

};
