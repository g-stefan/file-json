// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2020-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <clocale>
#include <cmath>
#include <XYO/FileJSON.hpp>

using namespace XYO::FileJSON;

static int failed = 0;

static void check(bool condition, const char *label) {
	if (!condition) {
		printf("* Fail: %s\n", label);
		++failed;
	};
};

// parse, then compare the minified output (nullptr expects a parse error)
static void parse(const char *input, const char *expected) {
	TPointer<Value> document;
	bool ok = loadFromString(input, document);
	if (expected == nullptr) {
		if (ok || document) {
			printf("* Fail: accepted invalid input: %s\n", input);
			++failed;
		};
		return;
	};
	if (!ok) {
		printf("* Fail: rejected valid input: %s\n", input);
		++failed;
		return;
	};
	String output;
	if (!saveToString(output, document, Mode::Minified) || (output != expected)) {
		printf("* Fail: %s -> %s, expected %s\n", input, output.value(), expected);
		++failed;
	};
};

static void testReader() {
	// top level scalars at end of input
	parse("true", "true");
	parse("false", "false");
	parse("null", "null");
	parse("123", "123");
	parse("1.5", "1.5");
	parse("\"x\"", "\"x\"");
	parse(" \r\n\t[ 1 , 2 ]\r\n", "[1,2]");
	parse("\xEF\xBB\xBF{}", "{}");
	// numbers
	parse("[1.5]", "[1.5]");
	parse("{\"a\":1.5}", "{\"a\":1.5}");
	parse("[1.5e10,1.5E+10,1e5,-2.5e-3,0,-0.25]", "[15000000000,15000000000,100000,-0.0025,0,-0.25]");
	parse("[123456789012]", "[123456789012]");
	parse("[1e400]", "[null]");
	// strings
	parse("[\"A\\u0041\\u00e9\\u20AC\\uD83D\\uDE00\"]", "[\"AA\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80\"]");
	parse("[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]", "[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]");
	parse("{\"a\":1,\"a\":2}", "{\"a\":2}");
	// invalid input
	parse("", nullptr);
	parse("   ", nullptr);
	parse("{\"a\":\"x\"", nullptr);
	parse("[\"a\",\"b\"", nullptr);
	parse("[[[[", nullptr);
	parse("[\"a\" \"b\"]", nullptr);
	parse("[,1]", nullptr);
	parse("[1,]", nullptr);
	parse("{\"a\":1,}", nullptr);
	parse("{\"a\" 1}", nullptr);
	parse("{a:1}", nullptr);
	parse("[-]", nullptr);
	parse("[007]", nullptr);
	parse("[1.]", nullptr);
	parse("[.5]", nullptr);
	parse("[1e]", nullptr);
	parse("[+1]", nullptr);
	parse("tru", nullptr);
	parse("truex", nullptr);
	parse("nul", nullptr);
	parse("[1] [2]", nullptr);
	parse("[\"\\x41\"]", nullptr);
	parse("[\"\\u12zz\"]", nullptr);
	parse("[\"\\uD83D\"]", nullptr);
	parse("[\"\\uDE00\"]", nullptr);
	parse("[\"a\x01\"]", nullptr);
	parse("[\"abc", nullptr);
	parse("[1,\xEF\xBB\xBF 2]", nullptr);
};

static void testDepth() {
	char buffer[2 * XYO_FILEJSON_MAX_DEPTH + 8];
	TPointer<Value> document;

	memset(buffer, '[', XYO_FILEJSON_MAX_DEPTH);
	memset(buffer + XYO_FILEJSON_MAX_DEPTH, ']', XYO_FILEJSON_MAX_DEPTH);
	buffer[2 * XYO_FILEJSON_MAX_DEPTH] = 0;
	check(loadFromString(buffer, document), "depth at limit");

	memset(buffer, '[', XYO_FILEJSON_MAX_DEPTH + 1);
	memset(buffer + XYO_FILEJSON_MAX_DEPTH + 1, ']', XYO_FILEJSON_MAX_DEPTH + 1);
	buffer[2 * XYO_FILEJSON_MAX_DEPTH + 2] = 0;
	check(!loadFromString(buffer, document), "depth over limit");

	// must fail cleanly instead of overflowing the stack
	const size_t deep = 1000000;
	char *hostile = new char[deep + 1];
	memset(hostile, '[', deep);
	hostile[deep] = 0;
	check(!loadFromString(hostile, document), "hostile depth");
	delete[] hostile;
};

static void testWriter() {
	TPointer<VArray> array;
	array.newMemory();
	array->value->set(0, VString::fromString("caf\xC3\xA9 \xE2\x82\xAC"));
	array->value->set(1, VString::fromString(String("\x01\x07\x1F\x7F", 4)));
	array->value->set(2, VNumber::fromNumber(0.1 + 0.2));
	array->value->set(3, VNumber::fromNumber(3.141592653589793));
	array->value->set(4, VNumber::fromNumber(123456789012.0));
	array->value->set(5, VNumber::fromNumber(1e-7));
	array->value->set(6, VNumber::fromNumber(NAN));
	array->value->set(7, VNumber::fromNumber(-INFINITY));
	array->value->set(8, TMemory<VArray>::newMemory());
	array->value->set(9, TMemory<VAssociativeArray>::newMemory());

	String output;
	check(saveToString(output, array, Mode::Minified), "saveToString");
	const char *expected = "[\"caf\xC3\xA9 \xE2\x82\xAC\",\"\\u0001\\u0007\\u001F\x7F\","
	                       "0.30000000000000004,3.141592653589793,123456789012,1e-07,null,null,[],{}]";
	if (output != expected) {
		printf("* Fail: writer output %s\n", output.value());
		++failed;
	};

	// own output must read back to the same values
	TPointer<Value> back;
	check(loadFromString(output, back), "read back own output");
	VArray *vArray = TDynamicCast<VArray *>(back.value());
	check(vArray && (vArray->length() == 10), "read back length");
	if (vArray && (vArray->length() == 10)) {
		check(TDynamicCast<VString *>(vArray->index(0).value())->value == "caf\xC3\xA9 \xE2\x82\xAC", "utf-8 round trip");
		check(TDynamicCast<VString *>(vArray->index(1).value())->value == String("\x01\x07\x1F\x7F", 4), "control round trip");
		check(TDynamicCast<VNumber *>(vArray->index(2).value())->value == 0.1 + 0.2, "number round trip");
		check(TDynamicCast<VNumber *>(vArray->index(3).value())->value == 3.141592653589793, "pi round trip");
	};

	TPointer<VAssociativeArray> object;
	object.newMemory();
	object->set("a", VNumber::fromNumber(1));
	object->set("b", array->index(8).value());
	TPointer<VArray> inner;
	inner.newMemory();
	inner->value->set(0, VBoolean::fromBoolean(true));
	inner->value->set(1, TMemory<VNull>::newMemory());
	object->set("c", inner.value());

	check(saveToString(output, object, Mode::IndentationTab), "saveToString tab");
	check(output == "{\r\n\t\"a\": 1,\r\n\t\"b\": [],\r\n\t\"c\": [\r\n\t\ttrue,\r\n\t\tnull\r\n\t]\r\n}", "indentation tab");
	check(saveToString(output, object, Mode::Indentation4Spaces), "saveToString spaces");
	check(output == "{\r\n    \"a\": 1,\r\n    \"b\": [],\r\n    \"c\": [\r\n        true,\r\n        null\r\n    ]\r\n}", "indentation 4 spaces");

	check(!saveToString(output, nullptr, Mode::Minified), "saveToString nullptr");

	// empty containers
	TPointer<VAssociativeArray> emptyObject;
	emptyObject.newMemory();
	check(saveToString(output, emptyObject, Mode::IndentationTab) && (output == "{}"), "empty object");
	TPointer<VArray> emptyArray;
	emptyArray.newMemory();
	check(saveToString(output, emptyArray, Mode::IndentationTab) && (output == "[]"), "empty array");

	// empty slots are written as null
	TPointer<VArray> sparse;
	sparse.newMemory();
	sparse->value->set(2, VNumber::fromNumber(3));
	check(saveToString(output, sparse, Mode::Minified) && (output == "[null,null,3]"), "sparse array");
	TPointer<VAssociativeArray> withEmpty;
	withEmpty.newMemory();
	withEmpty->set("a", nullptr);
	withEmpty->set("b", sparse.value());
	check(saveToString(output, withEmpty, Mode::Minified) && (output == "{\"a\":null,\"b\":[null,null,3]}"), "nullptr member");
	check(loadFromString(output, back), "read back empty slots");

	// output is replaced on success, left unchanged on error
	output = "previous";
	check(saveToString(output, emptyArray, Mode::Minified) && (output == "[]"), "saveToString replaces output");
	output = "previous";
	check(!saveToString(output, nullptr, Mode::Minified) && (output == "previous"), "saveToString nullptr keeps output");
	TPointer<VArray> cycle;
	cycle.newMemory();
	cycle->value->push(cycle.value());
	check(!saveToString(output, cycle, Mode::Minified) && (output == "previous"), "saveToString cycle keeps output");
	cycle->value->empty();
};

static void testLocale() {
	// a comma decimal separator locale must not change the format
	if (setlocale(LC_ALL, "de-DE") || setlocale(LC_ALL, "de_DE.UTF-8")) {
		parse("[1.5,-0.25]", "[1.5,-0.25]");
		setlocale(LC_ALL, "C");
	};
};

int main(int cmdN, char *cmdS[]) {
	testReader();
	testDepth();
	testWriter();
	testLocale();
	if (failed) {
		printf("* Error: %d check(s) failed\n", failed);
		return 1;
	};
	printf("Done.\r\n");
	return 0;
};
