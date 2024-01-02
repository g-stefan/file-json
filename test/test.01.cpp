// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <XYO/FileJSON.hpp>

using namespace XYO::FileJSON;

void test() {
	TPointer<Value> json;		
	if(!load("../../input/simple.json",json)){	
		throw std::runtime_error("File load");
	};
	if(!save("simple.indentation-tab.json",json)){
		throw std::runtime_error("File save");
	};
	if(!save("simple.indentation-4-spaces.json",json,Mode::Indentation4Spaces)){
		throw std::runtime_error("File save");
	};
	if(!save("simple.minified.json",json,Mode::Minified)){
		throw std::runtime_error("File save");
	};
	printf("Done.\r\n");
};

int main(int cmdN, char *cmdS[]) {

	try {
		test();
		return 0;
	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
