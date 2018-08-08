/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Codec_H
#define Codec_H

#include <strings.h> // for bzero
#include <limits>

#include "odb_api/CodecFactory.h"
#include "odb_api/MDI.h"

namespace eckit { class DataHandle; }

namespace odb {

class Reader;
class SameByteOrder;
class OtherByteOrder;

namespace codec {

class Codec {
public:
	Codec(const std::string& name);
	virtual ~Codec();

	/// Creates a clone of this codec. NOTE: the clone is not really usefull for coding/decoding, but has the same stats/missing
	/// values as the original codec, which can be useful sometimes.
	virtual Codec* clone();

	const std::string& name() const { return name_; }

    virtual unsigned char* encode(unsigned char* p, const double& d) = 0;
    virtual void decode(double* out) = 0;
	virtual void dataHandle(void *) = 0;

	template <typename DATASTREAM> static Codec* findCodec(const std::string& name, bool differentByteOrder);
	//static Codec* findCodec(const std::string& name, bool differentByteOrder);
	
	////template<typename BYTEORDER> static Codec* findCodec(const std::string& name);

	template<typename DATASTREAM> static Codec* loadCodec(DATASTREAM &);

	template<typename DATASTREAM> void save(DATASTREAM &); 

	void resetStats() { min_ = max_ = missingValue_; hasMissing_ = false; }

    virtual void gatherStats(const double& v);

	void hasMissing(bool h) { hasMissing_ = h; }
	int32_t hasMissing() const { return hasMissing_; }

	void min(double m) { min_ = m; }
	double min() const { return min_; }

	void max(double m) { max_ = m; }
	double max() const { return max_; }

	void missingValue(double v); 
    double missingValue() const { return missingValue_; }

    // Some special functions for string handling inside the CodecOptimizer
    virtual size_t numStrings() { NOTIMP; }
    virtual void copyStrings(Codec& rhs) { NOTIMP; }

	virtual void print(std::ostream& s) const;

	friend std::ostream& operator<<(std::ostream& s, const Codec& p)
		{ p.print(s); return s; }

	static void loadCodecs();

    virtual size_t dataSizeDoubles() const { return 1; }
    virtual void dataSizeDoubles(size_t count) {
        if (count != 1)
            throw eckit::SeriousBug("Data size cannot be changed from 1x8 bytes", Here());
    }

protected:

	template<typename BYTEORDER>
		void loadBasics(eckit::DataHandle *dh)
	{
		DataStream<BYTEORDER> f(dh);
		f.readInt32(hasMissing_);
		f.readDouble(min_);
		f.readDouble(max_);
		f.readDouble(missingValue_);
	}

	template<typename BYTEORDER>
		void saveBasics(eckit::DataHandle *dh)
	{
		DataStream<BYTEORDER> f(dh);
		f.writeInt32(hasMissing_);
		f.writeDouble(min_);
		f.writeDouble(max_);
		f.writeDouble(missingValue_);
	}

	std::string name_;

	int32_t hasMissing_;
	double missingValue_;
	double min_;
	double max_;
	
private:
	Codec(const Codec&);
	Codec& operator=(const Codec&);
};

//#include "odb_api/CodecFactory.h"

template<typename DATASTREAM>
void Codec::save(DATASTREAM &f)
{
	f.writeString(name_);
	AbstractCodecFactory<typename DATASTREAM::DataHandleType>::save(this, f);
}

template <typename DATASTREAM>
Codec* Codec::findCodec(const std::string& name, bool differentByteOrder)
{
	Codec::loadCodecs();
	return AbstractCodecFactory<typename DATASTREAM::DataHandleType>::getCodec(name, differentByteOrder);
}

template<typename BYTEORDER>
class CodecConstant : public Codec {
public:
	CodecConstant() : Codec("constant") {}
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	virtual void print(std::ostream& s) const;

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecConstantString : public CodecConstant<BYTEORDER> {
public:
	CodecConstantString() : CodecConstant<BYTEORDER>() { this->name_ = "constant_string"; }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	virtual void print(std::ostream& s) const;

    virtual size_t numStrings() { return 1; }

	void load(eckit::DataHandle *dh)
	{
		Codec::loadBasics<BYTEORDER>(dh);
		BYTEORDER::swap(this->min_);
		BYTEORDER::swap(this->max_);
	}

	void save(eckit::DataHandle *dh)
	{
		BYTEORDER::swap(this->min_);
		BYTEORDER::swap(this->max_);
		Codec::saveBasics<BYTEORDER>(dh);
	}
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecInt8 : public Codec {
public:
	CodecInt8() : Codec("int8") { this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI(); }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
	DataStream<BYTEORDER>& ds() { return ds_; }
private:
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecInt8Missing : public Codec {
public:
	CodecInt8Missing() : Codec("int8_missing") { this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI(); }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }

private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecConstantOrMissing : public CodecInt8Missing<BYTEORDER> {
public:
	CodecConstantOrMissing() : CodecInt8Missing<BYTEORDER>() { this->name_ = "constant_or_missing"; }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { CodecInt8Missing<BYTEORDER>::dataHandle(static_cast<eckit::DataHandle*>(p)); }

	virtual void print(std::ostream& s) const;

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecRealConstantOrMissing : public CodecConstantOrMissing<BYTEORDER> {
public:
	CodecRealConstantOrMissing() : CodecConstantOrMissing<BYTEORDER>()
	{
		this->name_ = "real_constant_or_missing";
		this->missingValue_ = this->min_ = this->max_ = odb::MDI::realMDI(); 
	}

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
//private:
//	DataStream<BYTEORDER>& ds() { return ds_; }
//	DataStream<BYTEORDER> ds_;
};


/// @note CodecChars is _only_ used as an intermediate codec. It encodes data during the
///       normal Writer phase that is then _reencoded_ using Int16String,...
///       We should NEVER see 'chars' in the output data.
template<typename BYTEORDER>
class CodecChars : public Codec {
public:
	CodecChars();
	~CodecChars();
	virtual Codec* clone();

    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	virtual void print(std::ostream& s) const;

    void gatherStats(const double& v);
	void load(eckit::DataHandle *dh);
	void save(eckit::DataHandle *dh);

    virtual size_t numStrings() { return strings_.size(); }
    virtual void copyStrings(Codec& rhs) {
        CodecChars<BYTEORDER>* c = dynamic_cast<CodecChars<BYTEORDER>*>(&rhs);
        ASSERT(c != 0);
        strings_ = c->strings_;
        stringLookup_ = c->stringLookup_;
    }

    virtual size_t dataSizeDoubles() const { return decodedSizeDoubles_; }
    virtual void dataSizeDoubles(size_t count) { decodedSizeDoubles_ = count; }

private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
protected:
    std::map<std::string, size_t> stringLookup_;
    std::vector<std::string> strings_;
    bool storeStringTable_;
    size_t decodedSizeDoubles_;
};

template<typename BYTEORDER>
Codec* CodecChars<BYTEORDER>::clone()
{
	CodecChars* c = static_cast<CodecChars*>(this->Codec::clone());
    c->stringLookup_ = stringLookup_;
    c->strings_ = strings_;
	ASSERT(c->min() == this->min());
	ASSERT(c->max() == this->max());
	//hashTable_->dumpTable(eckit::Log::info());
	return c;
}

template<typename BYTEORDER>
class CodecLongReal : public Codec {
public:
    CodecLongReal() :
        Codec("long_real"),
        hasShortRealInternalMissing_(false),
        hasShortReal2InternalMissing_(false) {}
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;
    virtual void gatherStats(const double& v);
    bool hasShortRealInternalMissing() { return hasShortRealInternalMissing_; }
    bool hasShortReal2InternalMissing() { return hasShortReal2InternalMissing_; }

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;

    bool hasShortRealInternalMissing_;
    bool hasShortReal2InternalMissing_;
};


template<typename BYTEORDER>
class CodecShortReal : public Codec {
public:
	CodecShortReal() : Codec("short_real") {}
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
    DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecShortReal2 : public Codec {
public:
	CodecShortReal2() : Codec("short_real2") {}
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
    DataStream<BYTEORDER> ds_;
};

/*
template<typename BYTEORDER>
class CodecShortRealScaled : public Codec {
public:
	CodecShortRealScaled() : Codec("short_real_scaled") {}
    virtual unsigned char* encode(unsigned char* p, const double& d);
	virtual double decode();

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;

	// TODO

};
*/

template<typename BYTEORDER>
class CodecInt32 : public Codec {
public:
	CodecInt32() : Codec("int32") { this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI(); }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecInt16 : public Codec {
public:
	CodecInt16() : Codec("int16") { this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI(); }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
	DataStream<BYTEORDER>& ds() { return ds_; }
private:
	DataStream<BYTEORDER> ds_;
};

template<typename BYTEORDER>
class CodecInt16Missing : public Codec {
public:
	CodecInt16Missing() : Codec("int16_missing") { this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI(); }
    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { ds_.dataHandle(static_cast<eckit::DataHandle*>(p)); }

	void load(eckit::DataHandle *dh) { Codec::loadBasics<BYTEORDER>(dh); }
	void save(eckit::DataHandle *dh) { Codec::saveBasics<BYTEORDER>(dh); }
private:
	DataStream<BYTEORDER>& ds() { return ds_; }
	DataStream<BYTEORDER> ds_;
};


template<typename BYTEORDER>
class CodecInt16String : public CodecChars<BYTEORDER> {
public:
    CodecInt16String() :
        CodecChars<BYTEORDER>(),
        intCodec()
	{
        this->storeStringTable_ = true;
		this->name_ = "int16_string";
		this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI();
		intCodec.min(0);
	}
	~CodecInt16String() {}
	virtual Codec* clone();

    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	virtual void dataHandle(void *p) { intCodec.ds().dataHandle(static_cast<eckit::DataHandle*>(p)); }

    void gatherStats(const double& v) { CodecChars<BYTEORDER>::gatherStats(v); }
	void load(eckit::DataHandle *dh) { CodecChars<BYTEORDER>::load(dh); }
	void save(eckit::DataHandle *dh) { CodecChars<BYTEORDER>::save(dh); }

private:
	DataStream<BYTEORDER>& ds() { return intCodec.ds(); }

	CodecInt16<BYTEORDER> intCodec;
};

template<typename BYTEORDER>
Codec* CodecInt16String<BYTEORDER>::clone()
{
	CodecInt16String* c = static_cast<CodecInt16String*>(this->CodecChars<BYTEORDER>::clone());
	c->intCodec.min(intCodec.min());
	c->intCodec.max(intCodec.max());
	c->min(this->min());
	c->max(this->max());
	return c;
}

template<typename BYTEORDER>
class CodecInt8String : public CodecChars<BYTEORDER> {
public:
    CodecInt8String() :
        CodecChars<BYTEORDER>(),
        intCodec() {
        this->storeStringTable_ = true;
		this->name_ = "int8_string";
		this->missingValue_ = this->min_ = this->max_ = odb::MDI::integerMDI();
		intCodec.min(0);
	}
	~CodecInt8String() {}
	virtual Codec* clone();

    virtual unsigned char* encode(unsigned char* p, const double& d) override;
    virtual void decode(double* out) override;

	void dataHandle(void *p) { intCodec.ds().dataHandle(static_cast<eckit::DataHandle*>(p)); }

    void gatherStats(const double& v) { CodecChars<BYTEORDER>::gatherStats(v); }
	void load(eckit::DataHandle *dh) { CodecChars<BYTEORDER>::load(dh); }
	void save(eckit::DataHandle *dh) { CodecChars<BYTEORDER>::save(dh); }

private:
	DataStream<BYTEORDER>& ds() { return intCodec.ds(); }
private:
	CodecInt8<BYTEORDER> intCodec;
};

template<typename BYTEORDER>
Codec* CodecInt8String<BYTEORDER>::clone()
{
	CodecInt8String* c = static_cast<CodecInt8String*>(this->CodecChars<BYTEORDER>::clone());
	c->intCodec.min(intCodec.min());
	c->intCodec.max(intCodec.max());
	c->min(this->min());
	c->max(this->max());
	return c;
}


template<typename BYTEORDER>
void CodecChars<BYTEORDER>::gatherStats(const double& v)
{
    size_t len = ::strnlen(reinterpret_cast<const char*>(&v), this->decodedSizeDoubles_*sizeof(double));
    std::string s(reinterpret_cast<const char*>(&v), len);

	char buf[255];
	memcpy(buf, &v, sizeof(double));
	buf[sizeof(double)] = 0;

    if (stringLookup_.find(s) == stringLookup_.end()) {
        size_t index = strings_.size();
        strings_.push_back(s);
        stringLookup_[s] = index;
    }

	// In case the column is const, the const value will be copied and used by the optimized codec.
	min_ = v;
}

template<typename BYTEORDER>
CodecChars<BYTEORDER>::CodecChars() :
    Codec("chars"),
    storeStringTable_(false),
    decodedSizeDoubles_(1) {}

template<typename BYTEORDER>
CodecChars<BYTEORDER>::~CodecChars() {}

template<typename BYTEORDER>
void CodecChars<BYTEORDER>::load(eckit::DataHandle *dh)
{
    Codec::loadBasics<BYTEORDER>(dh);
	DataStream<BYTEORDER> ds(dh);

    // Load the table of strings
    // This is based on the old-style hash-table storage, so it isn't a trivial list of strings
    int32_t numStrings;
    ds.readInt32(numStrings);
    ASSERT(numStrings >= 0);

    strings_.resize(numStrings);

    // How many doubles-worth of memory is needed to decode the largest string?
    decodedSizeDoubles_ = 1;

    for (size_t i = 0; i < size_t(numStrings); i++) {
        std::string s;
        ds.readString(s);

        int32_t cnt;
        ds.readInt32(cnt);

        int32_t index;
        ds.readInt32(index);

        ASSERT(index < numStrings);
        strings_[index] = s;

        decodedSizeDoubles_ = std::max(decodedSizeDoubles_, ((s.length()-1)/sizeof(double))+1);
    }

    // Ensure that the string lookup is EMPTY. We don't use it after reading
    ASSERT(stringLookup_.size() == 0);
}

template<typename BYTEORDER>
void CodecChars<BYTEORDER>::save(eckit::DataHandle *dh)
{
	Codec::saveBasics<BYTEORDER>(dh);
    DataStream<BYTEORDER> ds(dh);

    if (storeStringTable_) {

        int32_t numStrings = strings_.size();
        ds.writeInt32(numStrings);

        for (size_t i = 0; i < strings_.size(); i++) {
            ds.writeString(strings_[i]);
            ds.writeInt32(0); // "cnt" field is not used.
            ds.writeInt32(i);
        }

    } else {

        // Note that there are zero strings to follow
        int32_t zeroStrings = 0;
        ds.writeInt32(zeroStrings);
    }
}

//template<> Codec* Codec::findCodec<SameByteOrder>(const std::string& name) { findCodec(name, false); }
//template<> Codec* Codec::findCodec<OtherByteOrder>(const std::string& name) { findCodec(name, true); }

//template<typename DATASTREAM> void Codec::save(DATASTREAM &f) { f.writeString(name_); AbstractCodecFactory::save(this, f); }

template<typename BYTEORDER>
void CodecLongReal<BYTEORDER>::gatherStats(const double& v) {

    Codec::gatherStats(v);

    const uint32_t minFloatAsInt ( 0x800000 );
    const float internalMissing = *reinterpret_cast<const float*>(&minFloatAsInt);

    const uint32_t maxFloatAsInt ( 0x7f7fffff );
    const float internalMissing2 = -*reinterpret_cast<const float*>(&maxFloatAsInt);

    if (v == internalMissing) {
        this->hasShortRealInternalMissing_ = true;
    }
    if (v == internalMissing2) {
        this->hasShortReal2InternalMissing_ = true;
    }
}


template<typename BYTEORDER>
unsigned char* CodecShortReal<BYTEORDER>::encode(unsigned char* p, const double& d)
{
    const uint32_t minFloatAsInt ( 0x800000 );
    const float internalMissing = *reinterpret_cast<const float*>(&minFloatAsInt);

    float s;
    if (d == missingValue_) {
        s = internalMissing;
    } else {
        s = d;
        ASSERT(s != internalMissing);
    }

    BYTEORDER::swap(s);
    memcpy(p, &s, sizeof(s));
    return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecShortReal<BYTEORDER>::decode(double* out)
{
    float s;
    ds().readFloat(s);

    const uint32_t minFloatAsInt ( 0x800000 );
    (*out) = (s == *reinterpret_cast<const float*>( &minFloatAsInt ) ? missingValue_ : s);
}


template<typename BYTEORDER>
unsigned char* CodecShortReal2<BYTEORDER>::encode(unsigned char* p, const double& d)
{
    const uint32_t maxFloatAsInt ( 0x7f7fffff );
    const float internalMissing = -*reinterpret_cast<const float*>(&maxFloatAsInt);

    float s;
    if (d == missingValue_) {
        s = internalMissing;
    } else {
        s = d;
        ASSERT(s != internalMissing);
    }

    BYTEORDER::swap(s);
    memcpy(p, &s, sizeof(s));
    return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecShortReal2<BYTEORDER>::decode(double* out)
{
    float s;
    ds().readFloat(s);

    // The result of this is a bit pattern of 0xff7fffff
    const uint32_t maxFloatAsInt ( 0x7f7fffff );
    (*out) = (s == - *reinterpret_cast<const float*>( &maxFloatAsInt ) ? missingValue_ : s);
}

template<typename BYTEORDER>
unsigned char* CodecInt32<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	int32_t s = d;
    BYTEORDER::swap(s);
	memcpy(p, &s, sizeof(s));
	return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecInt32<BYTEORDER>::decode(double* out)
{
	int32_t s;
	ds().readInt32(s);
    (*out) = s;
}

template<typename BYTEORDER>
unsigned char* CodecInt16<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	unsigned short s = d - min_;
    BYTEORDER::swap(s);
    memcpy(p, &s, sizeof(s));
	return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecInt16<BYTEORDER>::decode(double* out)
{
	uint16_t s;
	ds().readUInt16(s);

    (*out) = s + min_;
}

template<typename BYTEORDER>
unsigned char* CodecInt16Missing<BYTEORDER>::encode(unsigned char* p, const double& d)
{
    uint16_t s;
    if (d == missingValue_) {
        s = 0xffff;
    } else {
        s = d - min_;
        ASSERT(s != 0xffff);
    }
    BYTEORDER::swap(s);
	memcpy(p, &s, sizeof(s));
	return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecInt16Missing<BYTEORDER>::decode(double* out)
{
	uint16_t s;
	ds().readUInt16(s);
    (*out) = (s == 0xffff ? missingValue_ : (s + min_));
}

template<typename BYTEORDER>
unsigned char* CodecInt8<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	unsigned char s = d - min_;
	memcpy(p, &s, sizeof(s));
	return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecInt8<BYTEORDER>::decode(double* out)
{
	unsigned char s;
	ds().readUChar(s);

    (*out) = s + min_;
}

template<typename BYTEORDER>
unsigned char* CodecInt8Missing<BYTEORDER>::encode(unsigned char* p, const double& d)
{
    unsigned char s;
    if (d == missingValue_) {
        s = 0xff;
    } else {
        s = d - min_;
        ASSERT(s != 0xff);
    }
	memcpy(p, &s, sizeof(s));
	return p + sizeof(s);
}

template<typename BYTEORDER>
void CodecInt8Missing<BYTEORDER>::decode(double* out)
{
	unsigned char s;
	ds().readUChar(s);
    (*out) = (s == 0xff ? missingValue_ : (s + min_));
}

template<typename BYTEORDER>
unsigned char* CodecInt8String<BYTEORDER>::encode(unsigned char* p, const double& d) {

    /// n.b. Yes this is ugly. This is a hack into the existing API - and it assumes
    ///      that the double& provided actually is the first element of a longer string.

    size_t len = ::strnlen(reinterpret_cast<const char*>(&d), this->decodedSizeDoubles_*sizeof(double));
    std::string s(reinterpret_cast<const char*>(&d), len);

    std::map<std::string, size_t>::const_iterator it = this->stringLookup_.find(s);
    ASSERT(it != this->stringLookup_.end());

    return intCodec.encode(p, it->second);
}

template<typename BYTEORDER>
void CodecInt8String<BYTEORDER>::decode(double* out) {

    double tmp_i;
    intCodec.decode(&tmp_i);
    int i = static_cast<int>(tmp_i);

    ASSERT(i < this->strings_.size());
    const std::string& s(this->strings_[i]);

    ::memset(out, 0, this->decodedSizeDoubles_*sizeof(double));
    ::memcpy(reinterpret_cast<char*>(out), &s[0], std::min(s.length(), this->decodedSizeDoubles_*sizeof(double)));
}

template<typename BYTEORDER>
unsigned char* CodecInt16String<BYTEORDER>::encode(unsigned char* p, const double& d) {

    /// n.b. Yes this is ugly. This is a hack into the existing API - and it assumes
    ///      that the double& provided actually is the first element of a longer string.

    size_t len = ::strnlen(reinterpret_cast<const char*>(&d), this->decodedSizeDoubles_*sizeof(double));
    std::string s(reinterpret_cast<const char*>(&d), len);

    std::map<std::string, size_t>::const_iterator it = this->stringLookup_.find(s);
    ASSERT(it != this->stringLookup_.end());

    return intCodec.encode(p, it->second);
}

template<typename BYTEORDER>
void CodecInt16String<BYTEORDER>::decode(double* out) {

    double tmp_i;
    intCodec.decode(&tmp_i);
    int i = static_cast<int>(tmp_i);

    ASSERT(i < this->strings_.size());
    const std::string& s(this->strings_[i]);

    ::memset(out, 0, this->decodedSizeDoubles_*sizeof(double));
    ::memcpy(reinterpret_cast<char*>(out), &s[0], std::min(s.length(), this->decodedSizeDoubles_*sizeof(double)));
}

template<typename BYTEORDER>
unsigned char* CodecLongReal<BYTEORDER>::encode(unsigned char* p,const double& s)
{
    double d = s;
    BYTEORDER::swap(d);
    memcpy(p, &d, sizeof(d));
    return p + sizeof(d);
}

template<typename BYTEORDER>
void CodecLongReal<BYTEORDER>::decode(double* out)
{
    ds().readDouble(*out);
}

template<typename BYTEORDER>
unsigned char* CodecChars<BYTEORDER>::encode(unsigned char* p, const double& s)
{
    /// n.b. Yes this is ugly. This is a hack into the existing API - and it assumes
    ///      that the double& provided actually is the first element of a longer string.

    memcpy(p, &s, decodedSizeDoubles_*sizeof(double));
    return p + (decodedSizeDoubles_*sizeof(double));
}

template<typename BYTEORDER>
void CodecChars<BYTEORDER>::decode(double* out)
{
    ds().read(out, sizeof(double)*decodedSizeDoubles_);
}

template<typename BYTEORDER>
unsigned char* CodecConstant<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	return p;
}

template<typename BYTEORDER>
void CodecConstant<BYTEORDER>::decode(double* out)
{
    (*out) = min_;
}

template<typename BYTEORDER>
unsigned char* CodecConstantOrMissing<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	//return CodecInt8Missing<BYTEORDER>::encode(p, d != this->missingValue_);
	return CodecInt8Missing<BYTEORDER>::encode(p, d);
}

template<typename BYTEORDER>
void CodecConstantOrMissing<BYTEORDER>::decode(double* out)
{
    CodecInt8Missing<BYTEORDER>::decode(out);
	//return n ? this->min_ : this->missingValue_;
}

template<typename BYTEORDER>
void CodecChars<BYTEORDER>::print(std::ostream& s) const
{
	s << this->name()
    << ", #words=" << this->strings_.size();
}

template<typename BYTEORDER>
void CodecConstant<BYTEORDER>::print(std::ostream& s) const
{
    s << this->name() << ", value=" << std::fixed << this->min_;
}

template<typename BYTEORDER>
void CodecConstantString<BYTEORDER>::print(std::ostream& s) const
{
	char buf[sizeof(double) + 1];
	bzero(buf, sizeof(buf));
	strncpy(buf, reinterpret_cast<const char *>(&this->min_), sizeof(double));
	s << this->name() << ", value='" << std::string(buf) << "'";
}

template<typename BYTEORDER>
void CodecConstantOrMissing<BYTEORDER>::print(std::ostream& s) const
{
	s << this->name() << ", value=";
	if (this->min_ == this->missingValue_)
		s << "NULL";
	else
        s << std::fixed << this->min_;

	if (this->hasMissing_)
		s << ", missingValue=" << this->missingValue_;
}



template<typename BYTEORDER>
unsigned char* CodecConstantString<BYTEORDER>::encode(unsigned char* p, const double& d)
{
	return p;
}

template<typename BYTEORDER>
void CodecConstantString<BYTEORDER>::decode(double* out)
{
    (*out) = this->min_;
}


} // namespace codec
} // namespace odb 

#endif

