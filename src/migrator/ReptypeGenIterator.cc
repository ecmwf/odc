/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file ReptypeGenIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include <strings.h>

extern "C" {
#include "odbdump.h"
}

#include "eckit/parser/StringTools.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Translator.h"
#include "migrator/FakeODBIterator.h"
#include "migrator/ODBIterator.h"
#include "migrator/ReptypeGenIterator.h"
#include "odb_api/odb_api.h"
#include "tools/Tool.h"


using namespace eckit;

namespace odb {
namespace tool {

std::ostream& operator<<(std::ostream& s, const ReptypeTable& m)
{
	s << "{";
	for (ReptypeTable::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		s << "[";
		const Values& vals = it->first;
		for (Values::const_iterator i = vals.begin(); i != vals.end(); ++i)
			s << *i;
		s << "]";

		s << " : " << it->second << "," << std::endl;
	}
	s << "}";
	return s;
}

/*
Hi Peter,

most of the columns given by Manuel do not exist yet.

As a first attempt, you can use
sensor@hdr,
satname_1@hdr,
satname_2@hdr,
satname_3@hdr,
satname_4@hdr,
bufrtype@hdr,
subtype@hdr,
obstype@hdr,
codetype@hdr
(when sat.len > 0 you can add satid@sat).

Anne. 
*/

std::vector<std::string> ReptypeTableConfig::columns_ = std::vector<std::string>();
ReptypeTable ReptypeTableConfig::reptypeTable_ = ReptypeTable();

void ReptypeTableConfig::load(const PathName& fileName)
{
	std::string s = Tool::readFile(fileName, false);
	Log::debug() << "ReptypeTableConfig::load(fileName = '" << fileName << "')" << std::endl;
	Log::debug() << "ReptypeTableConfig::load(fileName = '" << fileName << "')" << "'" << s << "'" << std::endl;

    std::vector<std::string> lines = StringTools::split("\n", s);

	size_t i = 0;
	while (lines[i] == "")
		++i;

    std::vector<std::string> firstLine = StringTools::split(":", lines[i]);
	ASSERT(firstLine[0] == "reptype");

    std::vector<std::string> columnNames = StringTools::split(",", firstLine[1]);
	std::for_each(columnNames.begin(), columnNames.end(), Tool::trimInPlace);

	columns_.insert(columns_.end(), columnNames.begin(), columnNames.end());
	++i;

    for (; i < lines.size(); ++i)
    {
        std::vector<std::string> lr;
        Tokenizer(":")(lines[i], lr);

        // Skip empty lines.
        if (lr.size() == 0) continue;

		ASSERT(lr.size() == 2);

		int reptype_value = Translator<std::string, int>()(lr[0]);

        std::vector<std::string> rvalues = StringTools::split(",", lr[1]);
		//Log::debug() << "ReptypeTableConfig::load: rvalues = " << rvalues << std::endl;

		ASSERT("Number of values must be equal to number of column names (first line)"
			&& columnNames.size() == rvalues.size());

		std::for_each(rvalues.begin(), rvalues.end(), Tool::trimInPlace);
		Values vals;
		Log::debug() << "ReptypeTableConfig::load: " << reptype_value << " = ";
		for (size_t j = 0; j < rvalues.size(); ++j)
		{
			std::string &vs(rvalues[j]);

			Log::debug() << "{" << vs << ":" << Tool::isInQuotes(vs) << "}";
			double v = Tool::isInQuotes(vs)
				? Tool::cast_as_double(Tool::unQuote(vs))
				: Translator<std::string, double>()(vs);
			vals.push_back(v);
			Log::debug() << "[" << rvalues[j] << "] '" << Tool::double_as_string(v) << "', " << std::endl;
		}
		Log::debug() << std::endl;
		//at(vals) = reptype_value;
		reptypeTable_[vals] = reptype_value;
	}

	//Log::debug() << "ReptypeTableConfig::load: columns_ = " << columns_ << std::endl; 
	//Log::debug() << "ReptypeTableConfig::load: reptypeTable_ = " << reptypeTable_ << std::endl;
}

template<typename ITERATOR, typename CONFIG>
ReptypeGenIterator<ITERATOR, CONFIG>::ReptypeGenIterator(const PathName& db, const std::string& sql)
: iterator_(db, sql),
  data_(0),
  reptypeTable_(CONFIG::reptypeTable())
{
	odb::MetaData &md = iterator_.columns();
	data_ = new double[md.size()];
	reptypeIndex_ = md.columnIndex("reptype");

	const std::vector<std::string>& columnNames = CONFIG::columns();
	for (std::vector<std::string>::const_iterator i = columnNames.begin(); i != columnNames.end(); ++i)
	{
		std::string name = *i;

		Log::debug() << "ReptypeGenIterator<ITERATOR>::ctor: " << name << std::endl;

		indices_.push_back(iterator_.columns().columnIndex(name));
		values_.push_back(0);
	}
	Log::debug() << "ReptypeGenIterator::ReptypeGenIterator: Reptype table:" << std::endl;
	Log::debug() << "reptypeTable_ = " << reptypeTable_ << std::endl;
}

template<typename ITERATOR, typename CONFIG>
ReptypeGenIterator<ITERATOR, CONFIG>::~ReptypeGenIterator()
{
	Log::debug() << "ReptypeGenIterator::~ReptypeGenIterator: Reptype table:" << std::endl;
	Log::debug() << "reptypeTable_.size() = " << reptypeTable_.size() << std::endl;
	Log::debug() << "reptypeTable_ =" << reptypeTable_ << std::endl;
	delete [] data_;
}

template<typename ITERATOR, typename CONFIG>
odb::MetaData& ReptypeGenIterator<ITERATOR, CONFIG>::columns()
{
	return iterator_.columns();
}

template<typename ITERATOR, typename CONFIG>
double* ReptypeGenIterator<ITERATOR, CONFIG>::data()
{
	return data_;
}

template<typename ITERATOR, typename CONFIG>
bool ReptypeGenIterator<ITERATOR, CONFIG>::isNewDataset()
{
	return iterator_.isNewDataset();
}

template<typename ITERATOR, typename CONFIG>
bool ReptypeGenIterator<ITERATOR, CONFIG>::next()
{
	bool r = iterator_.next();
	if (r)
	{
		double* trueData = iterator_.data();

        std::copy(trueData, trueData + iterator_.columns().size(), data_);

		for (size_t i = 0; i < indices_.size(); ++i)
			values_[i] = data_[indices_[i]];

		ReptypeTable::const_iterator it = reptypeTable_.find(values_); 
		if (it != reptypeTable_.end())
			data_[reptypeIndex_] = it->second;
		else
		{
			// TODO: rethink!
			Log::info() << "ReptypeGenIterator::next(): No matching report type, creating new one." << std::endl;

			size_t newRT = reptypeTable_.size();
			for (ReptypeTable::const_iterator i = reptypeTable_.begin(); i != reptypeTable_.end(); ++i)
			{
				size_t rt = i->second;
				if (newRT <= rt)
					newRT = rt + 1;
			}

			Log::info() << "ReptypeGenIterator::next(): New report type: " << newRT << std::endl;

			data_[reptypeIndex_] = reptypeTable_[values_] = newRT;
		}
	}
	noMore_ = !r;
	return r;
}

template class ReptypeGenIterator<>;
template class ReptypeGenIterator<FakeODBIterator>;

} // namespace tool 
} // namespace odb 

