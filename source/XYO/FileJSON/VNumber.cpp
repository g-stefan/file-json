// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <limits>
#include <cmath>
#include <cerrno>
#include <clocale>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <charconv>

#include <XYO/FileJSON/VNumber.hpp>

// Floating point to_chars/from_chars are missing from some standard libraries (older libc++),
// there printf/strtod are used, with the locale decimal separator swapped for '.'
#if defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L) && !defined(XYO_FILEJSON_NO_FLOAT_CHARCONV)
#	define XYO_FILEJSON_FLOAT_CHARCONV
#endif

namespace XYO::FileJSON {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VNumber, "{3C8FC0DA-17AD-4E73-BED9-48079308E9D3}");

	VNumber::VNumber() {
		XYO_DYNAMIC_TYPE_PUSH(VNumber);

		valueType_ = ValueType::Number;

		value = 0;
	};

	static char localeDecimalPoint() {
		const struct lconv *lc = localeconv();
		if (lc && lc->decimal_point && lc->decimal_point[0]) {
			return lc->decimal_point[0];
		};
		return '.';
	};

	static void replaceChar(char *buffer, size_t length, char from, char to) {
		if (from == to) {
			return;
		};
		for (size_t k = 0; k < length; ++k) {
			if (buffer[k] == from) {
				buffer[k] = to;
			};
		};
	};

	// strtod with '.' as decimal separator whatever the current locale is,
	// also used for out of range values, where it yields +/-HUGE_VAL or 0
	static bool parseWithStrtod(const char *text, size_t length, NumberT &value) {
		char stackBuffer[128];
		char *buffer = stackBuffer;
		if (length >= sizeof(stackBuffer)) {
			buffer = new char[length + 1];
		};
		memcpy(buffer, text, length);
		buffer[length] = 0;
		replaceChar(buffer, length, '.', localeDecimalPoint());
		char *end = nullptr;
		errno = 0;
		value = strtod(buffer, &end);
		bool retV = (end == buffer + length);
		if (buffer != stackBuffer) {
			delete[] buffer;
		};
		return retV;
	};

	size_t VNumber::toChars(NumberT value, char *buffer) {
		if (!std::isfinite(value)) {
			return 0;
		};
		// integers are written without exponent while exactly representable (2^53)
		if ((std::trunc(value) == value) && (std::fabs(value) < 9007199254740992.0)) {
			std::to_chars_result result = std::to_chars(buffer, buffer + NumberBufferSize, static_cast<long long>(value));
			return result.ptr - buffer;
		};
#ifdef XYO_FILEJSON_FLOAT_CHARCONV
		std::to_chars_result result = std::to_chars(buffer, buffer + NumberBufferSize, value);
		return result.ptr - buffer;
#else
		// fewest significant digits (up to 17) that read back to the same value
		int length = 0;
		for (int precision = 15; precision <= 17; ++precision) {
			length = snprintf(buffer, NumberBufferSize, "%.*g", precision, value);
			NumberT check;
			if (parseWithStrtod(buffer, length, check) && (check == value)) {
				break;
			};
		};
		replaceChar(buffer, length, localeDecimalPoint(), '.');
		return length;
#endif
	};

	bool VNumber::fromChars(const char *text, size_t length, NumberT &value) {
#ifdef XYO_FILEJSON_FLOAT_CHARCONV
		std::from_chars_result result = std::from_chars(text, text + length, value);
		if (result.ec == std::errc()) {
			return (result.ptr == text + length);
		};
		if (result.ec != std::errc::result_out_of_range) {
			return false;
		};
#endif
		return parseWithStrtod(text, length, value);
	};

	String VNumber::toString() {
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
		char buffer[NumberBufferSize];
		return String(buffer, toChars(value, buffer));
	};

	TPointer<VNumber> VNumber::fromNumber(NumberT value) {
		TPointer<VNumber> retV(TMemory<VNumber>::newMemory());
		retV->value = value;
		return retV;
	};

	TPointer<VNumber> VNumber::fromString(const String &value) {
		TPointer<VNumber> retV(TMemory<VNumber>::newMemory());
		if (value == "NaN") {
			retV->value = std::numeric_limits<NumberT>::quiet_NaN();
			return retV;
		};
		if (value == "Infinity") {
			retV->value = std::numeric_limits<NumberT>::infinity();
			return retV;
		};
		if (value == "-Infinity") {
			retV->value = -std::numeric_limits<NumberT>::infinity();
			return retV;
		};
		if (!fromChars(value.value(), value.length(), retV->value)) {
			retV->value = 0;
		};
		return retV;
	};

};
