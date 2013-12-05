/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "odblib/odb_api.h"
//#include "eckit/exception/Exceptions.h"
//#include "eckit/io/DataHandle.h"
//#include "odblib/DataStream.h"
//#include "odblib/HashTable.h"
//#include "odblib/FastODA2Request.h"
//#include "odblib/Codec.h"
//#include "odblib/Column.h"
//#include "odblib/MetaDataReaderIterator.h"
//#include "odblib/MetaDataReader.h"
//#include "odblib/CodecOptimizer.h"
//#include "odblib/FastODA2Request.h"
//#include "odblib/GribCodes.h"
//#include "odblib/ODAHandle.h"
//#include "odblib/StringTool.h"

namespace odb {

template <typename T>
FastODA2Request<T>::FastODA2Request()
: firstMD_(0),
  rowsNumber_(0),
  mergeSimilarBlocks_(true)
{}

template <typename T>
void FastODA2Request<T>::parseConfig(const std::string& s)
{
    std::vector<std::string> lines;
    eckit::Tokenizer("\n")(s, lines);

    eckit::Tokenizer tokenizer(": \t");
    for (size_t i = 0; i < lines.size(); ++i)
	{
		eckit::Log::debug() << "FastODA2Request<T>::parseConfig: " << i<< ": '" << lines[i] << "'" << std::endl;
		std::vector<std::string> words;
		tokenizer(lines[i], words);

		if (words.size() == 0) continue;

		ASSERT("Each line of config file should be like: 'MARS_KEYWORD : odb_column_name'" && words.size() == 2);

		addColumn(words[0], words[1]);
	}
}

template <typename T>
void FastODA2Request<T>::addColumn(const std::string& keyword, const std::string& columnName)
{
	keywords_.push_back(keyword);
	columnNames_.push_back(columnName);
}

template <typename T>
bool FastODA2Request<T>::scanFile(const eckit::PathName& fileName)
{
	eckit::OffsetList offsets;
	eckit::LengthList lengths;
	std::vector<ODAHandle*> handles;

	bool r = scanFile(fileName, offsets, lengths, handles);

	for (size_t i = 0; i < handles.size(); ++i)
		delete handles[i];
	handles.clear();

	return r;
}

template <typename T>
bool FastODA2Request<T>::scanFile(const eckit::PathName& fileName, eckit::OffsetList& offsets, eckit::LengthList& lengths, std::vector<ODAHandle*>& handles)
{
    using eckit::Log;

	Log::debug() << "Iterating over headers of '" << fileName << "'" <<  std::endl;
	
    inputFile_ = fileName;

	typedef MetaDataReader<MetaDataReaderIterator> MDR;

	MDR mdReader(fileName);
	MDR::iterator it = mdReader.begin(), end = mdReader.end();

	auto_ptr<MetaData> currentMD(it->columns().clone());
	rowsNumber_ = currentMD->rowsNumber();

	values_ = std::vector<set<string> >(currentMD->size(), std::set<string>());
	unsigned long int mds = 0;	
	for ( ; it != end; ++it)
	{
		ASSERT(it->isNewDataset());
		MetaData &md = it->columns();
		++mds;

		eckit::Offset startOffset = (**it).blockStartOffset(), endOffset = (**it).blockEndOffset();
		eckit::Length blockSize = endOffset - startOffset;

		if (!offsets.size() || !mergeSimilarBlocks_ || !currentMD->equalsIncludingConstants(md, columnNames_))
		{
			Log::debug() << "FastODA2Request::scanFile: new handle for <" << startOffset << "," << endOffset << ">" << std::endl;

			ODAHandle* odaHandle = new ODAHandle(startOffset, endOffset);
			if (! collectValues(md, *odaHandle))
			{
				Log::debug() << "FastODA2Request<T>::scanFile: collectValues returned false" << std::endl;
				return false;
			}
			currentMD.reset(md.clone());
			ASSERT(currentMD->equalsIncludingConstants(md, columnNames_));

			offsets.push_back(startOffset);
			lengths.push_back(blockSize);
			handles.push_back(odaHandle);
		}
		else
		{
			Log::debug() << "FastODA2Request::scanFile: append <" << startOffset << "," << endOffset << "> to existing handle" << std::endl;

			ODAHandle& lastHandle = *(handles.back());
			lastHandle.end(lastHandle.end() + blockSize);
			lengths.back() += blockSize;
		}
		rowsNumber_ += md.rowsNumber();
	}
	Log::debug() << "FastODA2Request<T>::scanFile => offsets=" << offsets << std::endl;
	Log::debug() << "FastODA2Request<T>::scanFile => lengths=" << lengths << std::endl;
	Log::debug() << "FastODA2Request<T>::scanFile => rowsNumber_=" << rowsNumber_ << std::endl;
	return true;
}

template <typename T>
bool FastODA2Request<T>::collectValues(const MetaData& md, ODAHandle& odaHandle)
{
    using eckit::Offset;
    
	std::vector<std::string> currentValues;
	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const std::string& columnName = columnNames_[i];
		eckit::Log::debug() << "FastODA2Request::collectValues: columnName: " << columnName << std::endl;

		Column* column = md.hasColumn(columnName) ? md.columnByName(columnName) : 0;
		std::string v = ! column ? columnNotFound(columnName)
				: ! column->isConstant() ? columnIsNotConstant(*column)
				: column->type() == odb::STRING ? StringTool::double_as_string(column->min())
				: column->type() == odb::INTEGER ? StringTool::int_as_double2string(column->min())
				: eckit::Translator<double, std::string>()(column->min());
		values_[i].insert(v);
		currentValues.push_back(v);
		double dv = !column ? odb::MDI::realMDI() : column->min();

		odaHandle.addValue(columnNames_[i], dv);
		doubleValues_[keywords_[i]].insert(dv);
	}

	if (columnNames_.size())
	{
		if (valuesSeen_.find(currentValues) == valuesSeen_.end())
			valuesSeen_[currentValues] = make_pair<Offset,Offset>(odaHandle.start(), odaHandle.end());
		else {
			std::pair<Offset,Offset> p = valuesSeen_[currentValues];
			std::vector<std::string> vs = columnNames_;
			for (size_t i = 0; i < vs.size(); ++i)
				vs[i] += std::string("=") + currentValues[i];
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
std::string FastODA2Request<T>::genRequest() const
{
	stringstream request;

	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const std::string& key = keywords_[i];
		std::string k = eckit::StringTools::upper(key);
		std::string valuesList;
		const std::set<string>& vs = values_[i];
		for (std::set<string>::const_iterator vi = vs.begin(); vi != vs.end(); ++vi)
			valuesList += std::string(vi != vs.begin() ? "/" : "") + patchValue(k, *vi);
		if (i > 0)
			request << ",\n";
		request << key << " = " << valuesList;
	}

	eckit::Log::debug() << "FastODA2Request<T>::genRequest() => " << endl << request.str() << std::endl;
	
	return request.str();
}

template <typename T>
std::string FastODA2Request<T>::patchValue(const std::string& k, const std::string& value) const
{
    using eckit::Log;
    using eckit::StringTools;
    
	std::string v = StringTools::trim(value);
	Log::debug() << "FastODA2Request::patchValue: v = '" << v  << "', key = " << k << std::endl;
	if (k == "TIME")
		v = StringTool::patchTimeForMars(v);
	else if (k == "CLASS" || k == "TYPE" || k == "STREAM" || k == "OBSGROUP")
	{
		Log::debug() << "FastODA2Request::genRequest: checking if '" << v << "' is numeric" << std::endl;
		if (StringTool::check(v, isdigit))
		{
			Log::debug() << "FastODA2Request::genRequest: replacing " << v << " with ";
			v = GribCodes::alphanumeric(StringTools::lower(k), v);
			Log::debug() << v << std::endl;
		}
		v = StringTools::upper(v);
	}
	return v;
}

template <typename T>
const std::set<string>& FastODA2Request<T>::getValues(const std::string& keyword)
{
	for (size_t i = 0; i < keywords_.size(); ++i)
		if (keywords_[i] == keyword)
			return values_[i];
	throw eckit::UserError(std::string("Keyword '") + keyword + "' not found");
	// This is to keep the compiler happy:
	return values_[-1];
}

template <typename T>
std::map<std::string, std::vector<double> > FastODA2Request<T>::getValues()
{
	std::map<std::string, std::vector<double> > r;

	for (std::map<std::string, std::set<double> >::const_iterator it = doubleValues_.begin(); it != doubleValues_.end(); ++it)
	{
		const std::string &k = it->first;
		const std::set<double>& values = it->second;

		std::vector<double>& vs = r[k] = std::vector<double>();

		for (std::set<double>::const_iterator vi = values.begin(); vi != values.end(); ++vi)
			vs.push_back(*vi);
	}
	return r;
}

template <typename T>
std::map<std::string, double> FastODA2Request<T>::getUniqueValues()
{
	std::map<std::string, double> r;
	for (size_t i = 0; i < keywords_.size(); ++i)
	{
		std::string kw = keywords_[i];
		if ( doubleValues_[kw].size() != 1)
		{
			stringstream s;
			s << "Data contains more than one '" << kw << "' value.";
			throw eckit::UserError(s.str());
		}
		r[kw] = *doubleValues_[kw].begin();
	}
	return r;
}

} // namespace odb

