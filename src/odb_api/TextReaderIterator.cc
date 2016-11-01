/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file TextReaderIterator.cc
///
/// @author Piotr Kuchta, Oct 2010

#include "eckit/parser/StringTools.h"
#include "eckit/utils/Translator.h"
#include "eckit/types/Types.h"

#include "odb_api/TextReaderIterator.h"
#include "odb_api/TextReader.h"
#include "odb_api/StringTool.h"
#include "odb_api/ColumnType.h"

using namespace eckit;

typedef StringTools S;

namespace odb {

TextReaderIterator::TextReaderIterator(TextReader &owner)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  nrows_(0),
  in_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  refCount_(0)
{
	in_ = &owner.stream();
	ASSERT(in_);

	parseHeader();
}

TextReaderIterator::TextReaderIterator(TextReader &owner, const PathName& pathName)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  nrows_(0),
  in_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  refCount_(0)
{
	in_ = new std::ifstream(pathName.localPath());
	ASSERT(in_);
	ownsF_ = true;
	parseHeader();
}

odb::BitfieldDef TextReaderIterator::parseBitfields(const std::string& c)
{
    //std::ostream& L( Log::debug() );

    size_t leftBracket (c.find('['));
    size_t rightBracket (c.find(']'));

    if ( !(leftBracket != std::string::npos && rightBracket != std::string::npos))
        throw UserError(std::string("Error parsing bitfield definition. Should be like: bitfield_column_name:BITFIELD[a:1;b:3] was: '") + c + "'");

    std::string s(c.substr(leftBracket + 1,  rightBracket - leftBracket - 1));

    //L << "TextReaderIterator::parseBitfields: s='" << s << "'" << std::endl;

    odb::FieldNames names;
    odb::Sizes      sizes;

    size_t numberOfBits = 0;
    std::vector<std::string> bs(S::split(";", s));

    //L << "TextReaderIterator::parseBitfields: bs=" << bs << std::endl;

    for (size_t i = 0; i < bs.size(); ++i)
    {
		std::vector<std::string> v(S::split(":", bs[i]));

        //L << "TextReaderIterator::parseBitfields:   bs[" << i << "] = " << bs[i] << " " << v << " :  " << v.size() << std::endl;

		if (v.size() != 2)
            throw UserError("Bitfields definition parse error");

        if (std::find(names.begin(), names.end(), v[0]) != names.end())
            throw UserError("Names of fields must be unique within one bitfield");

		names.push_back(v[0]);

		int size = atoi(v[1].c_str());
		
        if ( !(v.size() > 0) )
            throw UserError("Size of a bitfield must be positive and larger than zero");

        numberOfBits += size;
		sizes.push_back(size);
	}
    //L << "TextReaderIterator::parseBitfields: numberOfbits=" << numberOfBits << std::endl;

    if (numberOfBits > 31)
        throw UserError("Bitfields can have up to 31 bits only currently");

	return odb::BitfieldDef(make_pair(names, sizes));
}

void TextReaderIterator::parseHeader()
{
    std::string header;
    std::getline(*in_, header);
    std::vector<std::string> columns (S::split(owner_.delimiter(), header));
    //c->missingValue(missingValue);

    std::ostream& L(Log::info());

    L << "TextReaderIterator::parseHeader: columns: " << columns << std::endl;
    L << "TextReaderIterator::parseHeader: delimiter: '" << owner_.delimiter() << "'" << std::endl;
    L << "TextReaderIterator::parseHeader: header: '" << header << "'" << std::endl;

	for (size_t i = 0; i < columns.size(); ++i)
	{
		Log::debug() << "TextReaderIterator::parseHeader: column " << i << " '" << columns[i] << "'" << std::endl;
		std::vector<std::string> column (S::split(":", columns[i]));
		if (column.size() < 2)
			throw UserError(std::string("Column '") + columns[i] + "': format should be NAME \":\" TYPE");

		const std::string columnName (S::trim(column[0]));
		const std::string columnType (S::upper(S::join(":", std::vector<std::string>(column.begin() + 1, column.end()))));

		if (! S::startsWith(columnType, "BITFIELD"))
		{
			Log::debug() << "TextReaderIterator::parseHeader: adding column " << columns_.size() << " '" << columnName << "' : " 
						<< columnType << std::endl;
			columns_.addColumn(columnName, columnType);
		}
		else
		{
			Log::debug() << "TextReaderIterator::parseHeader: adding BITFIELD " << columns_.size() << " '" << columns[i] << std::endl;
			columns_.addBitfield(columnName, parseBitfields(columns[i]));
		}
	}
	initRowBuffer();
}

TextReaderIterator::~TextReaderIterator ()
{
	close();
	delete [] lastValues_;
}


bool TextReaderIterator::operator!=(const TextReaderIterator& other)
{
	ASSERT(&other == 0);
	return noMore_;
}

void TextReaderIterator::initRowBuffer()
{
	delete [] lastValues_;
	lastValues_ = new double [columns().size()];
	for(size_t i = 0; i < columns().size(); i++)
		lastValues_[i] = columns()[i]->missingValue(); 
}

bool TextReaderIterator::next(ecml::ExecutionContext*)
{
    newDataset_ = false;
    if (noMore_)
        return false; 

    std::string line;
    std::getline(*in_, line);
    StringTool::trimInPlace(line);
    std::vector<std::string> values(S::split(owner_.delimiter(), line));

    size_t nCols = values.size();
    if (nCols == 0)
        return ! (noMore_ = true);
    ASSERT(nCols == columns().size());

    for(size_t i = 0; i < nCols; ++i)
    {
        const std::string& v (S::trim(values[i]));
        if (S::upper(v) == "NULL")
            lastValues_[i] = columns_[i]->missingValue();
        else 
        {
            odb::ColumnType typ ( columns()[i]->type() );
            lastValues_[i] = typ == odb::STRING ? StringTool::cast_as_double(StringTool::unQuote(v))
                           : typ == odb::REAL ? Translator<std::string, double>()(v) 
                           : typ == odb::DOUBLE ? Translator<std::string, double>()(v) 
                           : typ == odb::INTEGER ? Translator<std::string, int>()(v)
                           : typ == odb::BITFIELD ? Translator<std::string, int>()(v)
                           // TODO: signal error
                           : columns_[i]->missingValue();
        }
    }

    return nCols;
}

bool TextReaderIterator::isNewDataset() { return newDataset_; }

const double* TextReaderIterator::data() { return lastValues_; }

int TextReaderIterator::close()
{
	//if (ownsF_ && f) { f->close(); delete f; f = 0; }

	if (ownsF_ && in_)
	{
		delete in_;
		in_ = 0;
	}

	return 0;
}

} // namespace odb

