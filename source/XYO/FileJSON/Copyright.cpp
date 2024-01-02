// File JSON
// Copyright (c) 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2020-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/FileJSON/Copyright.hpp>
#include <XYO/FileJSON/Copyright.rh>

namespace XYO::FileJSON::Copyright {

	static const char *copyright_ = XYO_FILEJSON_COPYRIGHT;
	static const char *publisher_ = XYO_FILEJSON_PUBLISHER;
	static const char *company_ = XYO_FILEJSON_COMPANY;
	static const char *contact_ = XYO_FILEJSON_CONTACT;

	const char *copyright() {
		return copyright_;
	};

	const char *publisher() {
		return publisher_;
	};

	const char *company() {
		return company_;
	};

	const char *contact() {
		return contact_;
	};

};
