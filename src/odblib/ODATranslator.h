/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODATranslator.h
/// Piotr Kuchta - ECMWF June 2009

#ifndef ODATranslator_H
#define ODATranslator_H

//#include <strings.h>
//#include <sstream>

//#include "eckit/eckit.h"

//#include "eckit/types/Date.h"
//#include "eckit/filesystem/PathName.h"
//#include "eckit/types/Time.h"
//#include "eckit/parser/Translator.h"
//#include "eckit/types/Types.h"

//#include "odblib/StringTool.h"

template <typename T>
struct ODATranslator {
	T operator()(double n) { return T(n); }
};

template <>
struct ODATranslator<string> {
	std::string operator()(double n) {
		std::string r = odb::StringTool::double_as_string(n);
		eckit::Log::info(Here()) << "ODATranslator<string>::operator()(double n=" << n << ") => " << r << std::endl;
		return r;
	}
};

template <>
struct ODATranslator<eckit::Time> {
	eckit::Time operator()(double n)
	{
		static const char * zeroes = "000000";

		std::string t = eckit::Translator<double, std::string>()(n);
		if (t.size() < 6)
			t = std::string(zeroes + t.size()) + t;
		eckit::Time tm = t;

		eckit::Log::info(Here()) << "ODATranslator<Time>::operator()(double n=" << n << ") => " << tm << std::endl;
		return tm;
	}
};

template <>
struct ODATranslator<eckit::Date> {
	eckit::Date operator()(double n)
	{
		static const char * zeroes = "000000";

		std::string t = eckit::Translator<long, std::string>()(n);
		if (t.size() < 6)
			t = std::string(zeroes + t.size()) + t;
		eckit::Date d = t;

		eckit::Log::info(Here()) << "ODATranslator<Date>::operator()(double n=" << n << ") => " << d << std::endl;
		return d;
	}
};

#endif
