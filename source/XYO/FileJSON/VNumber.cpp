// File JSON
// Copyright (c) 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <limits>
#include <cmath>

#include <XYO/FileJSON/VNumber.hpp>

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VNumber, "{3C8FC0DA-17AD-4E73-BED9-48079308E9D3}");

	VNumber::VNumber() {
		XYO_DYNAMIC_TYPE_PUSH(VNumber);

		value = 0;
	};

	String VNumber::toString() {
		char buf[128];
		if (std::isnan(value)) {
			return "NaN";
		};
		if (std::isinf(value)) {
			if (std::signbit(value)) {
				return "-Infinity";
			} else {
				return "Infinity";
			};
		};
		if ((value > 0 && value < 1e+11) || (value < 0 && value > -1e+11)) {
			NumberT fractpart, intpart;
			fractpart = std::modf(value, &intpart);
			if (fractpart == 0.0) {
				sprintf(buf, "%.0lf", value);
				return buf;
			};
		};
		sprintf(buf, "%g", value);
		return buf;
	};

	TPointer<VNumber> VNumber::fromNumber(NumberT value) {
		TPointer<VNumber> retV(TMemory<VNumber>::newMemory());
		retV->value = value;
		return retV;
	};

	TPointer<VNumber> VNumber::fromString(const String &value) {
		TPointer<VNumber> retV(TMemory<VNumber>::newMemory());
		if (value == "NaN") {
			retV->value = NAN;
			return retV;
		};
		if (value == "Infinity") {
			retV->value = std::numeric_limits<double>::infinity();
			return retV;
		};
		if (value == "-Infinity") {
			retV->value = -std::numeric_limits<double>::infinity();
			return retV;
		};
		if (sscanf(value, "%lf", &retV->value) != 1) {
			retV->value = 0;
		};
		return retV;
	};

};
