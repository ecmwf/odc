/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODAHandle.h
/// @author Piotr Kuchta - ECMWF Aug 2011

#ifndef ODAHandle_H
#define ODAHandle_H

#include "eclib/machine.h"

#include "eclib/Buffer.h"
#include "eclib/Date.h"
#include "eclib/Time.h"
#include "eclib/Offset.h"

#include "odblib/ODATranslator.h"


class ODAHandle {
public:
	ODAHandle(eckit::Offset, eckit::Offset);
	~ODAHandle(); // Change to virtual if base class

	void addValue(const string& columnName, double v);

	template <typename T>
		void getValue(const string& name, T& value)
	{
		value = ODATranslator<T>()(values_[name]);
		eckit::Log::debug() << "ODAHandle::getValue('" << name << "',=>" << value << ")" << endl;
	}

	void print(ostream&) const;

	eckit::Offset start() { return start_; }
	void start(const eckit::Offset& n) { start_ = n; }

	eckit::Offset end() { return end_; }
	void end(const eckit::Offset& n) { end_ = n; }

private:
// No copy allowed
	ODAHandle(const ODAHandle&);
	ODAHandle& operator=(const ODAHandle&);

	eckit::Offset start_;
	eckit::Offset end_;
	map<string, double> values_;

	friend ostream& operator<<(ostream& s, const ODAHandle& p)
		{ p.print(s); return s; }

};

#endif
