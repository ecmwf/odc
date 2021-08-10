/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Table.h"

#include <functional>
#include <bitset>

#include "eckit/io/AutoCloser.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/types/FixedString.h"

#include "odc/core/DecodeTarget.h"
#include "odc/core/Header.h"
#include "odc/core/MetaData.h"
#include "odc/core/Codec.h"

using namespace eckit;


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Table::Table(const ThreadSharedDataHandle& dh) :
    dh_(dh) {}

Offset Table::startPosition() const {
    return startPosition_;
}


Offset Table::nextPosition() const {
    return nextPosition_;
}

Length Table::encodedDataSize() const {
    return dataSize_;
}

size_t Table::rowCount() const {
    return metadata_.rowsNumber();
}

size_t Table::columnCount() const {
    return metadata_.size();
}

int32_t Table::byteOrder() const {
    return byteOrder_;
}

bool Table::otherByteOrder() const {
    return byteOrder_ != BYTE_ORDER_INDICATOR;
}

const MetaData& Table::columns() const {
    return metadata_;
}

const Properties& Table::properties() const {
    return properties_;
}

Buffer Table::readEncodedData(bool includeHeader) {

    if (includeHeader) {
        Buffer data(nextPosition() - startPosition());
        dh_.seek(startPosition());
        dh_.read(data, nextPosition() - startPosition());
        return data;
    } else {
        Buffer data(dataSize_);
        dh_.seek(dataPosition_);
        dh_.read(data, dataSize_);
        return data;
    }
}

const std::map<std::string, size_t>& Table::columnLookup() {

    if (columnLookup_.empty()) {

        const MetaData& metadata(columns());
        size_t ncols = metadata.size();

        for (size_t i = 0; i < ncols; i++) {
            const auto& nm(metadata[i]->name());
            if (!columnLookup_.emplace(nm, i).second) {
                std::stringstream ss;
                ss << "Duplicate column '" << nm << "' " << " found in table";
                throw ODBDecodeError(ss.str(), Here());
            }
            simpleColumnLookup_.emplace(nm.substr(0, nm.find('@')), i);
        }
    }

    return columnLookup_;
}

const std::map<std::string, size_t>&Table::simpleColumnLookup() {

    if (simpleColumnLookup_.empty()) {
        columnLookup();
    }

    return simpleColumnLookup_;
}


void Table::decode(DecodeTarget& target) {

    const MetaData& metadata(columns());
    size_t nrows = metadata.rowsNumber();
    size_t ncols = metadata.size();

    const std::map<std::string, size_t>& columnLookup(this->columnLookup());
    const std::map<std::string, size_t>& lookupSimple(simpleColumnLookup());

    // Loop over the specified output columns, and select the correct ones for decoding.

    std::vector<char> visitColumn(ncols, false);
    std::vector<api::StridedData*> facades(ncols, 0); // TODO: Do we want to do a copy, rather than point to StridedData*?

    ASSERT(target.columns().size() == target.dataFacades().size());
    ASSERT(target.columns().size() <= ncols);

    for (size_t i = 0; i < target.columns().size(); i++) {

        const auto& nm(target.columns()[i]);
        auto it = columnLookup.find(nm);
        if (it == columnLookup.end()) it = lookupSimple.find(nm);
        if (it == lookupSimple.end()) {
            std::stringstream ss;
            ss << "Column '" << nm << "' not found in ODB";
            throw ODBDecodeError(ss.str(), Here());
        }

        size_t pos = it->second;
        if (visitColumn[pos]) {
            std::stringstream ss;
            ss << "Duplicated column '" << nm << "' in decode specification";
            throw ODBDecodeError(ss.str(), Here());
        }

        visitColumn[pos] = true;
        facades[pos] = &target.dataFacades()[i];
        ASSERT(target.dataFacades()[i].nelem() >= nrows);
    }

    // Read the data in in bulk for this table

    const Buffer readBuffer(readEncodedData());

    // Special case for the empty table

    if (nrows == 0) return;

    // Prepare decoders for reading

    GeneralDataStream ds(otherByteOrder(), readBuffer);

    std::vector<std::reference_wrapper<Codec>> decoders;
    decoders.reserve(ncols);
    for (auto& col : metadata) {
        decoders.push_back(col->coder());
        decoders.back().get().setDataStream(ds);
    }

    // Fill the initial row with missingValues. This means that if we have an (old, unsupported)
    // ODB that doesn't start from column zero in the first column, then it gets the correct
    // value

    for (int col = 0; col < long(ncols); col++) {
        if (visitColumn[col]) {
            *reinterpret_cast<double*>((*facades[col])[0]) = decoders[col].get().missingValue();
        }
    }

    // Do the decoding

    int lastStartCol = 0;
    std::vector<size_t> lastDecoded(ncols, 0);

    for (size_t rowCount = 0; rowCount < nrows; ++rowCount) {

        unsigned char marker[2];
        ds.readBytes(&marker, sizeof(marker));
        int startCol = (marker[0] * 256) + marker[1]; // Endian independant

        if (lastStartCol > startCol) {
            for (int col = startCol; col < lastStartCol; col++) {
                if (visitColumn[col]) {
                    facades[col]->fill(lastDecoded[col], rowCount-1);
                }
            }
        }

        for (int col = startCol; col < long(ncols); col++) {
            if (visitColumn[col]) {
                decoders[col].get().decode(reinterpret_cast<double*>((*facades[col])[rowCount]));
                lastDecoded[col] = rowCount;
            } else {
                decoders[col].get().skip();
            }
        }

        lastStartCol = startCol;
    }

    // And fill in any columns that are incomplete

    for (size_t col = 0; col < ncols; col++) {
        if (lastDecoded[col] < nrows-1) {
            if (visitColumn[col]) {
                facades[col]->fill(lastDecoded[col], nrows-1);
            }
        } else {
            break;
        }
    }
}


Span Table::span(const std::vector<std::string>& columns, bool onlyConstants) {

    Span s(startPosition(), nextPosition()-startPosition());

    // Get any constant columns

    std::vector<std::string> nonConstantColumns;

    for (const std::string& columnName : columns) {

        Column* column = metadata_.columnByName(columnName);
        if (!column) throw UserError("Column '" + columnName + "' not found", Here());

        if (column->isConstant()) {
            s.addValue(columnName, column->type(), column->min());
        } else {
            nonConstantColumns.push_back(columnName);
        }
    }

    // We don't decode non-constant columns unless allowed to

    if (!nonConstantColumns.empty() && onlyConstants) {
        std::stringstream ss;
        ss << "Non-constant columns required in span: " << nonConstantColumns;
        throw UserError(ss.str(), Here());
    }

    if (!nonConstantColumns.empty()) {
        s.extend(decodeSpan(nonConstantColumns));
    }

    return s;
}


// Helper workers to simplify building span decoder

class ColumnValuesBase {
public: // methods

    virtual ~ColumnValuesBase() {}

    virtual void updateSpan(Span& span) = 0;
    virtual void addValue(double* val) = 0;
};


template <typename T>
class ColumnValues : public ColumnValuesBase {
public: // methods

    ColumnValues(const std::string& name) : name_(name) {}

    void updateSpan(Span& s) override {
        s.addValues(name_, values_);
    }

protected: // members
    std::string name_;
    std::set<T> values_;
};

struct IntegerColumnValues : ColumnValues<long> {
    using ColumnValues<long>::ColumnValues;
    void addValue(double* val) override { values_.insert(static_cast<int64_t>(*val)); }
};

struct DoubleColumnValues : ColumnValues<double> {
    using ColumnValues<double>::ColumnValues;
    void addValue(double* val) override { values_.insert(*val); }
};

struct StringColumnValues : ColumnValues<std::string> {
    StringColumnValues(const std::string& nm, size_t maxlen) : ColumnValues<std::string>(nm), maxLength_(maxlen) {}
    void addValue(double* val) override {
        const char* c = reinterpret_cast<const char*>(val);
        values_.insert(std::string(c, ::strnlen(c, maxLength_)));
    }
    size_t maxLength_;
};



Span Table::decodeSpan(const std::vector<std::string>& columns) {

    const MetaData& metadata(this->columns());
    size_t nrows = metadata.rowsNumber();
    size_t ncols = metadata.size();

    const std::map<std::string, size_t>& columnLookup = this->columnLookup();
    const std::map<std::string, size_t>& lookupSimple = simpleColumnLookup();

    // Store the unique values

    std::vector<std::unique_ptr<ColumnValuesBase>> columnValues(ncols);

    // Loop over the specified output columns, and select the correct ones for decoding.

    std::vector<char> visitColumn(ncols, false);
    size_t maxDoublesDecode = 1;

    for (const std::string& columnName : columns) {

        auto it = columnLookup.find(columnName);
        if (it == columnLookup.end()) it = lookupSimple.find(columnName);
        if (it == lookupSimple.end()) {
            std::stringstream ss;
            ss << "Column '" << columnName << "' not found in ODB";
            throw ODBDecodeError(ss.str(), Here());
        }

        visitColumn[it->second] = true;

        // What do we do with the values?

        switch (metadata[it->second]->type()) {
        case api::BITFIELD:
        case api::INTEGER:
            columnValues[it->second].reset(new IntegerColumnValues(columnName));
            break;
        case api::REAL:
        case api::DOUBLE:
            columnValues[it->second].reset(new DoubleColumnValues(columnName));
            break;
        case api::STRING:
            columnValues[it->second].reset(new StringColumnValues(columnName, sizeof(double)*metadata[it->second]->dataSizeDoubles()));
            maxDoublesDecode = std::max(maxDoublesDecode, metadata[it->second]->dataSizeDoubles());
            break;
        default:
            throw SeriousBug("Unexpected type in decoding column: " + columnName, Here());
        };
    }

    // Read the data in in bulk for this table

    const Buffer readBuffer(readEncodedData());
    GeneralDataStream ds(otherByteOrder(), readBuffer);

    std::vector<std::reference_wrapper<Codec>> decoders;
    decoders.reserve(ncols);
    for (auto& col : metadata) {
        decoders.push_back(col->coder());
        decoders.back().get().setDataStream(ds);
    }

    // Do the decoding

    std::vector<size_t> lastDecoded(ncols, 0);
    double decodeBuffer[maxDoublesDecode];

    for (size_t rowCount = 0; rowCount < nrows; ++rowCount) {

        unsigned char marker[2];
        ds.readBytes(&marker, sizeof(marker));
        int startCol = (marker[0] * 256) + marker[1]; // Endian independant

        for (int col = startCol; col < long(ncols); col++) {
            if (visitColumn[col]) {
                decoders[col].get().decode(decodeBuffer);
                columnValues[col]->addValue(decodeBuffer);
            } else {
                decoders[col].get().skip();
            }
        }
    }

    // And add these to the spans

    Span s(startPosition(), nextPosition()-startPosition());
    for (const auto& values : columnValues) {
        if (values.get()) values->updateSpan(s);
    }
    return s;
}


std::unique_ptr<Table> Table::readTable(odc::core::ThreadSharedDataHandle& dh) {

    Offset startPosition = dh.position();

    // Check the magic number. If no more data, we are done

    if (!Header::readMagic(dh)) return 0;

    // Load the header

    std::unique_ptr<Table> newTable(new Table(dh));
    Header hdr(newTable->metadata_, newTable->properties_);
    hdr.loadAfterMagic(dh);

    newTable->startPosition_ = startPosition;
    newTable->dataPosition_ = dh.position();
    newTable->dataSize_ = hdr.dataSize();
    newTable->nextPosition_ = dh.position() + newTable->dataSize_;
    newTable->byteOrder_ = hdr.byteOrder();

    // Check that the ODB hasn't been truncated.
    // n.b. Some DataHandles always return 0 (e.g. on a stream), so leth that pass.
    if (newTable->nextPosition_ > dh.estimate() && dh.estimate() != 0) {
        throw ODBIncomplete(dh.title(), Here());
    }

    return newTable;
}

//----------------------------------------------------------------------------------------------------------------------

}
}
