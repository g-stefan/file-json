// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/License.hpp>
#include <XYO/FileJSON/Copyright.hpp>

namespace XYO::FileJSON::License {

	std::string license() {
		std::string retV;
		retV += ManagedMemory::License::licenseMITHeader();
		retV += Copyright::copyright();
		retV += "\r\n";
		retV += ManagedMemory::License::licenseMITContent();
		return retV;
	};

	std::string shortLicense() {
		std::string retV;
		retV += Copyright::copyright();
		retV += "\r\n";
		retV += ManagedMemory::License::licenseMITShort();
		return retV;
	};

};
