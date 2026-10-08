// File JSON
// Copyright (c) 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Library.hpp>
#include <XYO/FileJSON/Writer.hpp>
#include <XYO/FileJSON/Mode.hpp>

namespace XYO::FileJSON {

	struct Writer {
			IWrite *iWrite;
			Mode mode;
			bool isOk;
			size_t depth;
			size_t length;
			char buffer[4096];

			inline Writer(IWrite *iWrite_, Mode mode_) : iWrite(iWrite_), mode(mode_), isOk(true), depth(0), length(0) {};

			bool flush();

			inline void write(const char *value, size_t size) {
				if (length + size > sizeof(buffer)) {
					flush();
					if (size > sizeof(buffer)) {
						writeDirect(value, size);
						return;
					};
				};
				memcpy(buffer + length, value, size);
				length += size;
			};

			inline void write(char value) {
				if (length == sizeof(buffer)) {
					flush();
				};
				buffer[length++] = value;
			};

			inline bool isIndented() const {
				return (mode == Mode::IndentationTab) || (mode == Mode::Indentation4Spaces);
			};

			void writeDirect(const char *value, size_t size);
			void writeIndentationBegin(size_t level);
			void writeIndentationSeparator();
			void writeIndentationEnd();
			void writeString(const String &value);
			void writeNumber(NumberT value);
			bool writeAssociativeArray(VAssociativeArray *vAssociativeArray, size_t level);
			bool writeArray(VArray *vArray, size_t level);
			bool writeValue(Value *value, size_t level);
	};

	void Writer::writeDirect(const char *value, size_t size) {
		while (isOk && size) {
			size_t written = iWrite->write(value, size);
			if (written == 0) {
				isOk = false;
				return;
			};
			value += written;
			size -= written;
		};
	};

	bool Writer::flush() {
		writeDirect(buffer, length);
		length = 0;
		return isOk;
	};

	void Writer::writeIndentationBegin(size_t level) {
		static const char tabs[] = "\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09";
		static const char spaces[] = "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		                             "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20";
		const char *fill;
		size_t fillSize;
		size_t size;
		if (mode == Mode::IndentationTab) {
			fill = tabs;
			fillSize = sizeof(tabs) - 1;
			size = level;
		} else if (mode == Mode::Indentation4Spaces) {
			fill = spaces;
			fillSize = sizeof(spaces) - 1;
			size = level * 4;
		} else {
			return;
		};
		while (size > fillSize) {
			write(fill, fillSize);
			size -= fillSize;
		};
		write(fill, size);
	};

	void Writer::writeIndentationSeparator() {
		if (isIndented()) {
			write('\x20');
		};
	};

	void Writer::writeIndentationEnd() {
		if (isIndented()) {
			write("\x0D\x0A", 2);
		};
	};

	void Writer::writeString(const String &value) {
		static const char hex[] = "0123456789ABCDEF";
		const char *scan = value.value();
		size_t size = value.length();
		size_t start = 0;
		write('\"');
		for (size_t k = 0; k < size; ++k) {
			unsigned char x = static_cast<unsigned char>(scan[k]);
			if ((x >= 0x20) && (x != '\"') && (x != '\\')) {
				continue;
			};
			// runs of characters that need no escaping are written at once,
			// UTF-8 sequences are written as they are
			write(scan + start, k - start);
			start = k + 1;
			switch (x) {
			case '\"':
				write("\\\"", 2);
				break;
			case '\\':
				write("\\\\", 2);
				break;
			case '\x08':
				write("\\b", 2);
				break;
			case '\x0C':
				write("\\f", 2);
				break;
			case '\x0A':
				write("\\n", 2);
				break;
			case '\x0D':
				write("\\r", 2);
				break;
			case '\x09':
				write("\\t", 2);
				break;
			default: {
				char code[6] = {'\\', 'u', '0', '0', hex[x >> 4], hex[x & 0x0F]};
				write(code, 6);
				break;
			};
			};
		};
		write(scan + start, size - start);
		write('\"');
	};

	void Writer::writeNumber(NumberT value) {
		char number[VNumber::NumberBufferSize];
		size_t size = VNumber::toChars(value, number);
		// NaN and Infinity have no JSON representation
		if (size == 0) {
			write("null", 4);
			return;
		};
		write(number, size);
	};

	bool Writer::writeAssociativeArray(VAssociativeArray *vAssociativeArray, size_t level) {
		AssociativeArrayT *value = vAssociativeArray->value;
		size_t count = value->length();
		if (count == 0) {
			write("{}", 2);
			return true;
		};
		write('{');
		writeIndentationEnd();
		for (size_t index = 0; index < count; ++index) {
			writeIndentationBegin(level + 1);
			writeString(value->arrayKey->index(index));
			write(':');
			writeIndentationSeparator();
			if (!writeValue(value->arrayValue->index(index).value(), level + 1)) {
				return false;
			};
			if (index + 1 < count) {
				write(',');
			};
			writeIndentationEnd();
		};
		writeIndentationBegin(level);
		write('}');
		return isOk;
	};

	bool Writer::writeArray(VArray *vArray, size_t level) {
		ArrayT *value = vArray->value;
		size_t count = value->length();
		if (count == 0) {
			write("[]", 2);
			return true;
		};
		write('[');
		writeIndentationEnd();
		for (size_t index = 0; index < count; ++index) {
			writeIndentationBegin(level + 1);
			if (!writeValue(value->index(index).value(), level + 1)) {
				return false;
			};
			if (index + 1 < count) {
				write(',');
			};
			writeIndentationEnd();
		};
		writeIndentationBegin(level);
		write(']');
		return isOk;
	};

	bool Writer::writeValue(Value *value, size_t level) {
		if (!isOk) {
			return false;
		};
		// empty slots (sparse arrays, members set to nullptr) are written as null
		if (value == nullptr) {
			write("null", 4);
			return true;
		};
		switch (value->getValueType()) {
		case ValueType::Null:
			write("null", 4);
			return true;
		case ValueType::Boolean:
			if (static_cast<VBoolean *>(value)->value) {
				write("true", 4);
			} else {
				write("false", 5);
			};
			return true;
		case ValueType::Number:
			writeNumber(static_cast<VNumber *>(value)->value);
			return true;
		case ValueType::String:
			writeString(static_cast<VString *>(value)->value);
			return true;
		case ValueType::Array:
		case ValueType::AssociativeArray: {
			// also stops on cyclic documents
			if (++depth > XYO_FILEJSON_MAX_DEPTH) {
				return false;
			};
			bool retV;
			if (value->getValueType() == ValueType::Array) {
				retV = writeArray(static_cast<VArray *>(value), level);
			} else {
				retV = writeAssociativeArray(static_cast<VAssociativeArray *>(value), level);
			};
			--depth;
			return retV;
		};
		default:
			break;
		};
		return false;
	};

	bool save(const char *fileName, Value *document, Mode mode) {
		if (document == nullptr) {
			return false;
		};
		File file;
		if (!file.openWrite(fileName)) {
			return false;
		};
		Writer writer(&file, mode);
		if (!writer.writeValue(document, 0)) {
			return false;
		};
		return writer.flush();
	};

	bool saveToString(String &output, Value *document, Mode mode) {
		if (document == nullptr) {
			return false;
		};
		// output is replaced only on success, on error it is left unchanged
		String retV;
		StringWrite stringWrite;
		stringWrite.use(retV);
		Writer writer(&stringWrite, mode);
		if (!writer.writeValue(document, 0)) {
			return false;
		};
		if (!writer.flush()) {
			return false;
		};
		output = retV;
		return true;
	};

};
