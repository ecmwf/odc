/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"
#include "eclib/Exceptions.h"
#include "eclib/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/FastODA2Request.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/MetaDataReader.h"
#include "odblib/CodecOptimizer.h"
#include "odblib/FastODA2Request.h"
#include "odblib/GribCodes.h"
#include "odblib/ODAHandle.h"
#include "odblib/StringTool.h"

namespace odb {

template <typename T>
FastODA2Request<T>::FastODA2Request()
: firstMD_(0),
  rowsNumber_(0),
  mergeSimilarBlocks_(true)
{}

template <typename T>
void FastODA2Request<T>::parseConfig(const string& s)
{
    vector<string> lines;
    Tokenizer("\n")(s, lines);

    Tokenizer tokenizer(": \t");
    for (size_t i = 0; i < lines.size(); ++i)
	{
		Log::debug() << "FastODA2Request<T>::parseConfig: " << i<< ": '" << lines[i] << "'" << endl;
		vector<string> words;
		tokenizer(lines[i], words);

		if (words.size() == 0) continue;

		ASSERT("Each line of config file should be like: 'MARS_KEYWORD : odb_column_name'" && words.size() == 2);

		addColumn(words[0], words[1]);
	}
}

template <typename T>
void FastODA2Request<T>::addColumn(const string& keyword, const string& columnName)
{
	keywords_.push_back(keyword);
	columnNames_.push_back(columnName);
}

template <typename T>
bool FastODA2Request<T>::scanFile(const PathName& fileName)
{
	OffsetList offsets;
	LengthList lengths;
	vector<ODAHandle*> handles;

	bool r = scanFile(fileName, offsets, lengths, handles);

	for (size_t i = 0; i < handles.size(); ++i)
		delete handles[i];
	handles.clear();

	return r;
}

template <typename T>
bool FastODA2Request<T>::scanFile(const PathName& fileName, OffsetList& offsets, LengthList& lengths, vector<ODAHandle*>& handles)
{
	ostream& L(Log::debug());
	L << "Iterating over headers of '" << fileName << "'" <<  endl;
	inputFile_ = fileName;

	typedef MetaDataReader<MetaDataReaderIterator> MDR;

	MDR mdReader(fileName);
	MDR::iterator it = mdReader.begin();
	MDR::iterator end = mdReader.end();

	MetaData currentMD = it->columns();
	rowsNumber_ = currentMD.rowsNumber();

	values_ = vector<set<string> >(currentMD.size(), set<string>());

	unsigned long int mds = 0;	
	for ( ; it != end; ++it)
	{
		ASSERT(it->isNewDataset());

		MetaData &md = it->columns();
		++mds;

		Offset startOffset = (**it).blockStartOffset(),
				endOffset = (**it).blockEndOffset();
		Length blockSize = endOffset - startOffset;

		if (!offsets.size() || !mergeSimilarBlocks_ || !currentMD.equalsIncludingConstants(md, columnNames_))
		{
			L << "FastODA2Request::scanFile: new handle for <" << startOffset << "," << endOffset << ">" << endl;

			ODAHandle* odaHandle = new ODAHandle(startOffset, endOffset);
			if (! collectValues(md, *odaHandle))
			{
				L << "FastODA2Request<T>::scanFile: collectValues returned false" << endl;
				return false;
			}
			currentMD = md;
			ASSERT(currentMD.equalsIncludingConstants(md, columnNames_));

			offsets.push_back(startOffset);
			lengths.push_back(blockSize);
			handles.push_back(odaHandle);
		}
		else
		{
			L << "FastODA2Request::scanFile: append <" << startOffset << "," << endOffset << "> to existing handle" << endl;

			ODAHandle& lastHandle = *(handles.back());
			lastHandle.end(lastHandle.end() + blockSize);
			lengths.back() += blockSize;
		}

		rowsNumber_ += md.rowsNumber();
	}
	L << "FastODA2Request<T>::scanFile => offsets=" << offsets << endl;
	L << "FastODA2Request<T>::scanFile => lengths=" << lengths << endl;
	L << "FastODA2Request<T>::scanFile => handles=" << handles << endl;
	L << "FastODA2Request<T>::scanFile => rowsNumber_=" << rowsNumber_ << endl;
	return true;
}

template <typename T>
bool FastODA2Request<T>::collectValues(const MetaData& md, ODAHandle& odaHandle)
{
	vector<string> currentValues;
	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const string& columnName = columnNames_[i];
		Log::debug() << "FastODA2Request::collectValues: columnName: " << columnName << endl;

		Column* column = md.hasColumn(columnName) ? md.columnByName(columnName) : 0;
		string v = ! column ? columnNotFound(columnName)
				: ! column->isConstant() ? columnIsNotConstant(*column)
				: column->type() == odb::STRING ? StringTool::double_as_string(column->min())
				: column->type() == odb::INTEGER ? StringTool::int_as_double2string(column->min())
				: Translator<double, string>()(column->min());
		values_[i].insert(v);
		currentValues.push_back(v);
		double dv = !column ? MISSING_VALUE_REAL : column->min();

		odaHandle.addValue(columnNames_[i], dv);
		doubleValues_[keywords_[i]].insert(dv);
	}

	if (columnNames_.size())
	{
		if (valuesSeen_.find(currentValues) == valuesSeen_.end())
			valuesSeen_[currentValues] = make_pair<Offset, Offset>(odaHandle.start(), odaHandle.end());
		else {
			pair<Offset,Offset> p = valuesSeen_[currentValues];
			vector<string> vs = columnNames_;
			for (size_t i = 0; i < vs.size(); ++i)
				vs[i] += string("=") + currentValues[i];
			stringstream s;
			s << "Values " << vs << " found in blocks <" << p.first << "," << p.second << ">"
				<< " and <" << odaHandle.start() << "," << odaHandle.end() << ">";
			if (! duplicateCombination(s.str()))
				return false;
		}
	}
	return true;
}

template <typename T>
string FastODA2Request<T>::genRequest() const
{
	stringstream request;

	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const string& key = keywords_[i];
		string k = StringTools::upper(key);
		string valuesList;
		const set<string>& vs = values_[i];
		for (set<string>::const_iterator vi = vs.begin(); vi != vs.end(); ++vi)
			valuesList += string(vi != vs.begin() ? "/" : "") + patchValue(k, *vi);
		if (i > 0)
			request << ",\n";
		request << key << " = " << valuesList;
	}

	Log::debug() << "FastODA2Request<T>::genRequest() => " << endl << request.str() << endl;
	
	return request.str();
}

template <typename T>
string FastODA2Request<T>::patchValue(const string& k, const string& value) const
{
	string v = StringTools::trim(value);
	Log::debug() << "FastODA2Request::patchValue: v = '" << v  << "', key = " << k << endl;
	if (k == "TIME")
		v = StringTool::patchTimeForMars(v);
	else if (k == "CLASS" || k == "TYPE" || k == "STREAM" || k == "OBSGROUP")
	{
		Log::debug() << "FastODA2Request::genRequest: checking if '" << v << "' is numeric" << endl;
		if (StringTool::check(v, isdigit))
		{
			Log::debug() << "FastODA2Request::genRequest: replacing " << v << " with ";
			v = GribCodes::alphanumeric(StringTools::lower(k), v);
			Log::debug() << v << endl;
		}
		v = StringTools::upper(v);
	}
	return v;
}

template <typename T>
const set<string>& FastODA2Request<T>::getValues(const string& keyword)
{
	for (size_t i = 0; i < keywords_.size(); ++i)
		if (keywords_[i] == keyword)
			return values_[i];
	throw UserError(string("Keyword '") + keyword + "' not found");
	// This is to keep the compiler happy:
	return values_[-1];
}

template <typename T>
map<string, vector<double> > FastODA2Request<T>::getValues()
{
	map<string, vector<double> > r;

	for (map<string, set<double> >::const_iterator it = doubleValues_.begin(); it != doubleValues_.end(); ++it)
	{
		const string &k = it->first;
		const set<double>& values = it->second;

		vector<double>& vs = r[k] = vector<double>();

		for (set<double>::const_iterator vi = values.begin(); vi != values.end(); ++vi)
			vs.push_back(*vi);
	}
	return r;
}

template <typename T>
map<string, double> FastODA2Request<T>::getUniqueValues()
{
	map<string, double> r;
	for (size_t i = 0; i < keywords_.size(); ++i)
	{
		string kw = keywords_[i];
		if ( doubleValues_[kw].size() != 1)
		{
			stringstream s;
			s << "Data contains more than one '" << kw << "' value.";
			throw UserError(s.str());
		}
		r[kw] = *doubleValues_[kw].begin();
	}
	return r;
}

} // namespace odb

