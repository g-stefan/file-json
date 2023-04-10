// File JSON
// Copyright (c) 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Library.hpp>
#include <XYO/FileJSON/Writer.hpp>
#include <XYO/FileJSON/Mode.hpp>

namespace XYO::FileJSON {

	struct Writer {
			TPointer<IWrite> iWrite;
			Mode mode;

			inline bool write(const String &value) {
				return StreamX::write(*iWrite, value);
			};

			bool writeIndentationBegin(int level);
			bool writeIndentationSeparator();
			bool writeIndentationEnd();
			bool writeAssociativeArray(VAssociativeArray *vAssociativeArray, int level);
			bool writeArray(VArray *vArray, int level);
			bool writeString(VString *vString);
			bool writeNumber(VNumber *vNumber);
			bool writeBoolean(VBoolean *vBoolean);
			bool writeNull(VNull *vNull);
			bool write(Value *vNull, int level);
	};

	bool Writer::writeIndentationBegin(int level) {
		if (mode == Mode::Indentation4Spaces) {
			for (; level > 0; --level) {
				if (!write("\x20\x20\x20\x20")) {
					return false;
				};
			};
			return true;
		};
		if (mode == Mode::IndentationTab) {
			for (; level > 0; --level) {
				if (!write("\x09")) {
					return false;
				};
			};
			return true;
		};
		return true;
	};

	bool Writer::writeIndentationSeparator() {
		if ((mode == Mode::Indentation4Spaces) || (mode == Mode::IndentationTab)) {
			if (!write(" ")) {
				return false;
			};
			return true;
		};
		return true;
	};

	bool Writer::writeIndentationEnd() {
		if ((mode == Mode::Indentation4Spaces) || (mode == Mode::IndentationTab)) {
			if (!write("\x0D\x0A")) {
				return false;
			};
			return true;
		};
		return true;
	};

	bool Writer::writeAssociativeArray(VAssociativeArray *vAssociativeArray, int level) {
		size_t index;
		if (!write("{")) {
			return false;
		};
		if (!writeIndentationEnd()) {
			return false;
		};
		for (index = 0; index < vAssociativeArray->value->length();) {
			if (!writeIndentationBegin(level + 1)) {
				return false;
			};
			if (!write(vAssociativeArray->value->arrayKey->index(index).encodeC())) {
				continue;
			};
			if (!write(":")) {
				return false;
			};
			if (!writeIndentationSeparator()) {
				return false;
			};
			if (!write((vAssociativeArray->value->arrayValue->index(index)).value(), level + 1)) {
				return false;
			};
			++index;
			if (index < vAssociativeArray->value->length()) {
				if (!write(",")) {
					return false;
				};
			};
			if (!writeIndentationEnd()) {
				return false;
			};
		};
		if (!writeIndentationBegin(level)) {
			return false;
		};
		if (!write("}")) {
			return false;
		};
		return true;
	};

	bool Writer::writeArray(VArray *vArray, int level) {
		size_t index;
		if (!write("[")) {
			return false;
		};
		if (!writeIndentationEnd()) {
			return false;
		};
		for (index = 0; index < vArray->value->length();) {
			if (!writeIndentationBegin(level + 1)) {
				return false;
			};
			if (!write((vArray->value->index(index)).value(), level + 1)) {
				return false;
			};
			++index;
			if (index < vArray->value->length()) {
				if (!write(",")) {
					return false;
				};
				if (!writeIndentationEnd()) {
					return false;
				};
			};
		};

		if (!writeIndentationEnd()) {
			return false;
		};
		if (!writeIndentationBegin(level)) {
			return false;
		};

		if (!write("]")) {
			return false;
		};
		return true;
	};

	bool Writer::writeString(VString *vString) {
		return write(vString->value.encodeC());
	};

	bool Writer::writeNumber(VNumber *vNumber) {
		return write(vNumber->toString());
	};

	bool Writer::writeBoolean(VBoolean *vBoolean) {
		return write(vBoolean->toString());
	};

	bool Writer::writeNull(VNull *vNull) {
		return write(vNull->toString());
	};

	bool Writer::write(Value *value, int level) {
		VNull *vNull = TDynamicCast<VNull *>(value);
		if (vNull) {
			return writeNull(vNull);
		};
		VBoolean *vBoolean = TDynamicCast<VBoolean *>(value);
		if (vBoolean) {
			return writeBoolean(vBoolean);
		};
		VNumber *vNumber = TDynamicCast<VNumber *>(value);
		if (vNumber) {
			return writeNumber(vNumber);
		};
		VString *vString = TDynamicCast<VString *>(value);
		if (vString) {
			return writeString(vString);
		};
		VArray *vArray = TDynamicCast<VArray *>(value);
		if (vArray) {
			return writeArray(vArray, level);
		};
		VAssociativeArray *vAssociativeArray = TDynamicCast<VAssociativeArray *>(value);
		if (vAssociativeArray) {
			return writeAssociativeArray(vAssociativeArray, level);
		};
		return false;
	};

	bool save(const char *fileName, Value *document, Mode mode) {
		Writer writer;
		File file;

		if (document == nullptr) {
			return false;
		};

		if (file.openWrite(fileName)) {
			writer.iWrite = &file;
			writer.mode = mode;
			return writer.write(document, 0);
		};
		return false;
	};

	bool saveToString(String &output, Value *document, Mode mode) {
		Writer writer;
		StringWrite str;
		str.use(output);
		writer.iWrite = &str;
		writer.mode = mode;
		writer.write(document, 0);
		return true;
	};
};
