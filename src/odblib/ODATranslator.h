/// \file ODATranslator.h
/// Piotr Kuchta - ECMWF June 2009

#ifndef ODATranslator_H
#define ODATranslator_H

#include "machine.h"
#include <strings.h>
#include "Time.h"
#include "Date.h"
#include "Types.h"
#include "PathName.h"
#include "Translator.h"
#include "StringTool.h"

#include <sstream>

#ifndef SRC
#define SRC __FILE__,__LINE__
#endif

template <typename T>
struct ODATranslator {
	T operator()(double n) { return T(n); }
};

template <>
struct ODATranslator<string> {
	string operator()(double n) {
		string r = odb::StringTool::double_as_string(n);
		Log::info(SRC) << "ODATranslator<string>::operator()(double n=" << n << ") => " << r << endl;
		return r;
	}
};

template <>
struct ODATranslator<Time> {
	Time operator()(double n)
	{
		static const char * zeroes = "000000";

		string t = Translator<double, string>()(n);
		if (t.size() < 6)
			t = string(zeroes + t.size()) + t;
		Time tm = t;

		Log::info(SRC) << "ODATranslator<Time>::operator()(double n=" << n << ") => " << tm << endl;
		return tm;
	}
};

template <>
struct ODATranslator<Date> {
	Date operator()(double n)
	{
		static const char * zeroes = "000000";

		string t = Translator<long, string>()(n);
		if (t.size() < 6)
			t = string(zeroes + t.size()) + t;
		Date d = t;

		Log::info(SRC) << "ODATranslator<Date>::operator()(double n=" << n << ") => " << d << endl;
		return d;
	}
};

#endif
