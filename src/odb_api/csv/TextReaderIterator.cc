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

#include "odb_api/csv/TextReaderIterator.h"
#include "odb_api/csv/TextReader.h"
#include "odb_api/ColumnType.h"

using namespace eckit;

typedef StringTools S;

namespace odb {

TextReaderIterator::TextReaderIterator(TextReader &owner)
: columns_(0),
  lastValues_(0),
  columnOffsets_(0),
  nrows_(0),
  delimiter_(owner.delimiter()),
  in_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  refCount_(0)
{
    in_ = &owner.stream();
    ASSERT(in_);

    parseHeader();
    next();
}

TextReaderIterator::TextReaderIterator(TextReader &owner, const PathName& pathName)
: columns_(0),
  lastValues_(0),
  columnOffsets_(0),
  nrows_(0),
  delimiter_(owner.delimiter()),
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
    next();
}

eckit::sql::BitfieldDef TextReaderIterator::parseBitfields(const std::string& c)
{
    //std::ostream& L( Log::debug() );

    size_t leftBracket (c.find('['));
    size_t rightBracket (c.find(']'));

    if ( !(leftBracket != std::string::npos && rightBracket != std::string::npos))
        throw UserError(std::string("Error parsing bitfield definition. Should be like: bitfield_column_name:BITFIELD[a:1;b:3] was: '") + c + "'");

    std::string s(c.substr(leftBracket + 1,  rightBracket - leftBracket - 1));

    //L << "TextReaderIterator::parseBitfields: s='" << s << "'" << std::endl;

    eckit::sql::FieldNames names;
    eckit::sql::Sizes      sizes;

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

    if (numberOfBits > 31) {
        throw UserError("Bitfields can have up to 31 bits only currently");
    }

	return eckit::sql::BitfieldDef(make_pair(names, sizes));
}

void TextReaderIterator::parseHeader()
{
    std::string header;
    std::getline(*in_, header);
    std::vector<std::string> columns (S::split(delimiter_, header));
    //c->missingValue(missingValue);

    std::ostream& L(Log::info());

    L << "TextReaderIterator::parseHeader: columns: " << columns << std::endl;
    L << "TextReaderIterator::parseHeader: delimiter: '" << delimiter_ << "'" << std::endl;
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
    delete [] columnOffsets_;
}


bool TextReaderIterator::operator!=(const TextReaderIterator& other)
{
    return noMore_;
}

void TextReaderIterator::initRowBuffer()
{
    delete [] lastValues_;
    delete [] columnOffsets_;

    rowDataSizeDoubles_ = 0;
    columnOffsets_ = new size_t[columns().size()];
    for (size_t i = 0; i < columns().size(); i++) {
        columnOffsets_[i] = rowDataSizeDoubles_;
        rowDataSizeDoubles_ += columns()[i]->dataSizeDoubles();
    }

    lastValues_ = new double [rowDataSizeDoubles_];
	for(size_t i = 0; i < columns().size(); i++)
        lastValues_[columnOffsets_[i]] = columns()[i]->missingValue();
}

bool TextReaderIterator::next()
{
    newDataset_ = false;
    if (noMore_)
        return false; 

    std::string line;
    std::getline(*in_, line);
    line = S::trim(line);
    std::vector<std::string> values(S::split(delimiter_, line));

    size_t nCols = values.size();
    if (nCols == 0)
        return ! (noMore_ = true);
    ASSERT(nCols == columns().size());

    for(size_t i = 0; i < nCols; ++i)
    {
        const std::string& v (S::trim(values[i]));
        if (S::upper(v) == "NULL") {
            lastValues_[columnOffsets_[i]] = columns_[i]->missingValue();
        } else  {
            odb::ColumnType typ ( columns()[i]->type() );

            switch (typ) {

            case odb::STRING: {
                std::string unquoted = S::unQuote(v);
                size_t charlen = unquoted.length();
                size_t lenDoubles = charlen > 0 ? (((charlen - 1) / 8) + 1): 1;

                // If the string is bigger than any we have come across before, we need to
                // resize the buffers to cope for this
                // TODO: Adjust the writer to be able to easily continue if all we have changed is a column size.
                if (lenDoubles > columns_[i]->dataSizeDoubles()) {

                    newDataset_ = true;
                    columns_[i]->dataSizeDoubles(lenDoubles);

                    // Allocate a new buffer, but keep the old data around
                    double* oldData = lastValues_;
                    lastValues_ = 0;
                    initRowBuffer();
                    ASSERT(oldData);
                    ::memcpy(lastValues_, oldData, columnOffsets_[i]*sizeof(double));
                    delete oldData;
                }

                char* buf = reinterpret_cast<char*>(&lastValues_[columnOffsets_[i]]);
                lenDoubles = columns_[i]->dataSizeDoubles();

                ::memcpy(buf, &unquoted[0], charlen);
                ::memset(buf + charlen, 0, (lenDoubles * sizeof(double)) - charlen);
                break;
            }

            case odb::REAL:
                lastValues_[columnOffsets_[i]] = static_cast<double>(Translator<std::string, float>()(v));
                break;

            case odb::DOUBLE:
                lastValues_[columnOffsets_[i]] = Translator<std::string, double>()(v);
                break;

            case odb::INTEGER:
            case odb::BITFIELD:
                lastValues_[columnOffsets_[i]] = static_cast<double>(Translator<std::string, long>()(v));
                break;

            default:
                throw SeriousBug("Unexpected type in column", Here());
            }
        }
    }

    return nCols;
}

bool TextReaderIterator::isNewDataset() { return newDataset_; }

double& TextReaderIterator::data(size_t i)
{
    ASSERT(i >= 0 && i < columns().size());
    return lastValues_[columnOffsets_[i]];
}

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

