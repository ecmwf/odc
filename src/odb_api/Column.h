/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Column_H
#define Column_H

#include "odb_api/Codec.h"
#include "odb_api/ColumnType.h"
#include "eckit/sql/SQLTypedefs.h"

namespace eckit { class DataHandle; }

namespace odc {

class Reader;
class MetaData;

class Column {

public:
	Column(MetaData &);
	Column(const Column&);

	static const char *columnTypeName(ColumnType type);

	static ColumnType type(const std::string&);

	Column& operator=(const Column&);

	bool operator==(const Column&) const;
	bool operator!=(const Column& other) const { return ! (*this == other); }

	virtual ~Column();

	template<typename DATASTREAM> void load(DATASTREAM &);
	template<typename DATASTREAM> void save(DATASTREAM &);

	codec::Codec& coder() const { return *coder_; }
	void coder(codec::Codec *c) { delete coder_; coder_ = c; }

    size_t dataSizeDoubles() const { return coder_->dataSizeDoubles(); }
    void dataSizeDoubles(size_t count) { coder_->dataSizeDoubles(count); }

	void name(const std::string name) { name_ = name; }
	const std::string &name() const { return name_; }

	template<typename DATASTREAM> void type(ColumnType t, bool differentByteOrder); 

	void type(ColumnType t) { type_ = t; }
	ColumnType type() const { return ColumnType(type_); }

    bool hasInitialisedCoder() const { return coder_ != 0; }
	bool isConstant();

	/// Delegations to Codec:

	void hasMissing(bool h) { coder_->hasMissing(h); }
	int32_t hasMissing() const { return coder_->hasMissing(); }

	void min(double m) { coder_->min(m); }
	double min() const { return coder_->min(); }

	void max(double m) { coder_->max(m); }
	double max() const { return coder_->max(); }

	void missingValue(double v) { coder_->missingValue(v); }
	double missingValue() const { return coder_->missingValue(); } 

	void resetStats() { coder_->resetStats(); }

    void bitfieldDef(const eckit::sql::BitfieldDef& b) { bitfieldDef_ = b; }
    const eckit::sql::BitfieldDef& bitfieldDef() const { return bitfieldDef_; }

	virtual void print(std::ostream& s) const;

#ifdef SWIGPYTHON
	const std::string __repr__()
	{
		return //std::string("<") + 
			name_ + ":" + columnTypeName(odc::ColumnType(type_))
		 //+ ">" 
			;
	}
	const std::string __str__() { return name_; }
#endif

	friend std::ostream& operator<<(std::ostream& s, const Column& p)
		{ p.print(s); return s; }

private:

	MetaData& owner_;
	std::string name_;
	/// Note: type_ should be ColumnType, but it is saved on file so must be of a fixed size type.
	int32_t type_;
	codec::Codec* coder_;
	/// bitfieldDef_ is not empty if type_ == BITFIELD.
    eckit::sql::BitfieldDef bitfieldDef_;
	//std::string typeSignature_;

};

template<typename DATASTREAM> void Column::load(DATASTREAM &f)
{
	f.readString(name_);
	f.readInt32(type_);
	if (type_ == BITFIELD)
	{
        eckit::sql::FieldNames names;
        eckit::sql::Sizes sizes;
		bitfieldDef_ = make_pair(names, sizes);
		f.readBitfieldDef(bitfieldDef_);
	}

	coder(codec::Codec::loadCodec(f));
}

template<typename DATASTREAM> void Column::save(DATASTREAM &f)
{
	f.writeString(name_);
	f.writeInt32(type_);

	if (type_ == BITFIELD)
		f.writeBitfieldDef(bitfieldDef_);

	coder().save(f);
}

template <typename DATASTREAM> 
void Column::type(ColumnType t, bool differentByteOrder)
{
	type_ = t;
	std::string codecName;
	switch (type_)
	{
		case INTEGER:  codecName = "int32"; break;
		case BITFIELD:
                       // TODO: 'unsigned_int64'
                       codecName = "int32"; break;
		case REAL:     codecName = "long_real"; break;
		case DOUBLE:   codecName = "long_real"; break;
		case STRING:   codecName = "chars"; break;
		default:
			ASSERT(!"Type not supported");
			break;
	}
	coder(codec::Codec::findCodec<DATASTREAM>(codecName, differentByteOrder));

    // TODO: when we have codec unsigned_int64 it will have 0 as 
    if (type_ == BITFIELD) missingValue(MDI::bitfieldMDI());
}

} // namespace odc {

#endif
