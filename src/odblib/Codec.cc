/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <limits>
#include <strings.h>

#include "eclib/Exceptions.h"
#include "eclib/DataHandle.h"

#include "odblib/Codec.h"
#include "odblib/Comparator.h"
#include "odblib/DataStream.h"
#include "odblib/MemoryBlock.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/UnsafeInMemoryDataHandle.h"

using namespace eclib;

namespace odb {
namespace codec {

template<> map<string, AbstractCodecFactory<DataHandle>* > AbstractCodecFactory<DataHandle>::codecFactories = map<string, AbstractCodecFactory<DataHandle>* >();
template<> map<string, AbstractCodecFactory<FastInMemoryDataHandle>* > AbstractCodecFactory<FastInMemoryDataHandle>::codecFactories = map<string, AbstractCodecFactory<FastInMemoryDataHandle>* >();
template<> map<string, AbstractCodecFactory<PrettyFastInMemoryDataHandle>* > AbstractCodecFactory<PrettyFastInMemoryDataHandle>::codecFactories = map<string, AbstractCodecFactory<PrettyFastInMemoryDataHandle>* >();

Codec::Codec(const string& name)
: name_(name),
  hasMissing_(false),
  missingValue_(odb::MDI::realMDI()),
  min_(missingValue_),
  max_(missingValue_)
{}

Codec* Codec::clone()
{
    Codec* c = findCodec<DataStream<SameByteOrder, DataHandle> >(name_, false);
    c->hasMissing_ = hasMissing_;
    c->missingValue_ = missingValue_;
    c->min_ = min_;
    c->max_ = max_;
    return c;
}

Codec::~Codec() {}

void Codec::missingValue(double v)
{
    ASSERT("Cannot change missing value after encoding of column data started" && (min_ == missingValue_) && (max_ == missingValue_));
    min_ = max_ = missingValue_ = v;
}

HashTable& Codec::hashTable() { NOTIMP; }
HashTable* Codec::giveHashTable() { NOTIMP; }
void Codec::hashTable(HashTable *) { NOTIMP; }

void Codec::gatherStats(double v)
{
    if(v == missingValue_)
        hasMissing_ = 1;
    else
    {
        if(v < min_ || min_ == missingValue_)
            min_ = v;
        if(v > max_ || max_ == missingValue_)
            max_ = v;
    }
}

void Codec::print(ostream& s) const
{
    s << name()
    //<< ", min=" << fixed << min_
    //<< ", max=" << max_
        << ", range=<" << fixed << min_ << "," << max_ << ">";

    if (hasMissing_)
    {
        //s << ", hasMissing=";

        s << ", missingValue=" << missingValue_;
    }
}

template<> Codec* Codec::loadCodec(DataStream<SameByteOrder,DataHandle> &f) { return AbstractCodecFactory<DataHandle>::loadCodec(f.dataHandle(), false); }
template<> Codec* Codec::loadCodec(DataStream<OtherByteOrder,DataHandle> &f) { return AbstractCodecFactory<DataHandle>::loadCodec(f.dataHandle(), true); }

template<> Codec* Codec::loadCodec(DataStream<SameByteOrder,FastInMemoryDataHandle> &f) { return AbstractCodecFactory<FastInMemoryDataHandle>::loadCodec(f.dataHandle(), false); }
template<> Codec* Codec::loadCodec(DataStream<OtherByteOrder,FastInMemoryDataHandle> &f) { return AbstractCodecFactory<FastInMemoryDataHandle>::loadCodec(f.dataHandle(), true); }

template<> Codec* Codec::loadCodec(DataStream<SameByteOrder,PrettyFastInMemoryDataHandle> &f) { return AbstractCodecFactory<PrettyFastInMemoryDataHandle>::loadCodec(f.dataHandle(), false); }
template<> Codec* Codec::loadCodec(DataStream<OtherByteOrder,PrettyFastInMemoryDataHandle> &f) { return AbstractCodecFactory<PrettyFastInMemoryDataHandle>::loadCodec(f.dataHandle(), true); }

template<>
template<> void AbstractCodecFactory<DataHandle>::save<SameByteOrder>(Codec *codec, DataStream<SameByteOrder,DataHandle> &f) { codecFactories[codec->name()]->save(codec, f.dataHandle(), false); }
template<> 
template<> void AbstractCodecFactory<DataHandle>::save<OtherByteOrder>(Codec *codec, DataStream<OtherByteOrder,DataHandle> &f) { codecFactories[codec->name()]->save(codec, f.dataHandle(), true); }

void Codec::loadCodecs() {
	static CodecFactory<CodecConstant,DataHandle> codecConstantFactory("constant");
	static CodecFactory<CodecConstantString,DataHandle> codecConstantStringFactory("constant_string");
	static CodecFactory<CodecConstantOrMissing,DataHandle> codecConstantOrMissingFactory("constant_or_missing");
	static CodecFactory<CodecRealConstantOrMissing,DataHandle> codecRealConstantOrMissingFactory("real_constant_or_missing");
	static CodecFactory<CodecChars,DataHandle> codecCharsFactory("chars");
	static CodecFactory<CodecLongReal,DataHandle> codecLongRealFactory("long_real");
	static CodecFactory<CodecShortReal,DataHandle> codecShortRealFactory("short_real");
	static CodecFactory<CodecShortReal2,DataHandle> codecShortReal2Factory("short_real2");
	static CodecFactory<CodecInt32,DataHandle> codecInt32Factory("int32");
	static CodecFactory<CodecInt16,DataHandle> codecInt16Factory("int16");
	static CodecFactory<CodecInt8,DataHandle> codecInt8Factory("int8");
	static CodecFactory<CodecInt16Missing,DataHandle> codecInt16MissingFactory("int16_missing");
	static CodecFactory<CodecInt8Missing,DataHandle> codecInt8MissingFactory("int8_missing");
	static CodecFactory<CodecInt16String,DataHandle> codecInt16StringFactory("int16_string");
	static CodecFactory<CodecInt8String,DataHandle> codecInt8StringFactory("int8_string");

	static CodecFactory<CodecConstant,FastInMemoryDataHandle> fastCodecConstantFactory("constant");
	static CodecFactory<CodecConstantString,FastInMemoryDataHandle> fastCodecConstantStringFactory("constant_string");
	static CodecFactory<CodecConstantOrMissing,FastInMemoryDataHandle> fastCodecConstantOrMissingFactory("constant_or_missing");
	static CodecFactory<CodecRealConstantOrMissing,FastInMemoryDataHandle> fastCodecRealConstantOrMissingFactory("real_constant_or_missing");
	static CodecFactory<CodecChars,FastInMemoryDataHandle> fastCodecCharsFactory("chars");
	static CodecFactory<CodecLongReal,FastInMemoryDataHandle> fastCodecLongRealFactory("long_real");
	static CodecFactory<CodecShortReal,FastInMemoryDataHandle> fastCodecShortRealFactory("short_real");
	static CodecFactory<CodecShortReal2,FastInMemoryDataHandle> fastCodecShortReal2Factory("short_real2");
	static CodecFactory<CodecInt32,FastInMemoryDataHandle> fastCodecInt32Factory("int32");
	static CodecFactory<CodecInt16,FastInMemoryDataHandle> fastCodecInt16Factory("int16");
	static CodecFactory<CodecInt8,FastInMemoryDataHandle> fastCodecInt8Factory("int8");
	static CodecFactory<CodecInt16Missing,FastInMemoryDataHandle> fastCodecInt16MissingFactory("int16_missing");
	static CodecFactory<CodecInt8Missing,FastInMemoryDataHandle> fastCodecInt8MissingFactory("int8_missing");
	static CodecFactory<CodecInt16String,FastInMemoryDataHandle> fastCodecInt16StringFactory("int16_string");
	static CodecFactory<CodecInt8String,FastInMemoryDataHandle> fastCodecInt8StringFactory("int8_string");

	static CodecFactory<CodecConstant,PrettyFastInMemoryDataHandle> prettyFastCodecConstantFactory("constant");
	static CodecFactory<CodecConstantString,PrettyFastInMemoryDataHandle> prettyFastCodecConstantStringFactory("constant_string");
	static CodecFactory<CodecConstantOrMissing,PrettyFastInMemoryDataHandle> prettyFastCodecConstantOrMissingFactory("constant_or_missing");
	static CodecFactory<CodecRealConstantOrMissing,PrettyFastInMemoryDataHandle> prettyFastCodecRealConstantOrMissingFactory("real_constant_or_missing");
	static CodecFactory<CodecChars,PrettyFastInMemoryDataHandle> prettyFastCodecCharsFactory("chars");
	static CodecFactory<CodecLongReal,PrettyFastInMemoryDataHandle> prettyFastCodecLongRealFactory("long_real");
	static CodecFactory<CodecShortReal,PrettyFastInMemoryDataHandle> prettyFastCodecShortRealFactory("short_real");
	static CodecFactory<CodecShortReal2,PrettyFastInMemoryDataHandle> prettyFastCodecShortReal2Factory("short_real2");
	static CodecFactory<CodecInt32,PrettyFastInMemoryDataHandle> prettyFastCodecInt32Factory("int32");
	static CodecFactory<CodecInt16,PrettyFastInMemoryDataHandle> prettyFastCodecInt16Factory("int16");
	static CodecFactory<CodecInt8,PrettyFastInMemoryDataHandle> prettyFastCodecInt8Factory("int8");
	static CodecFactory<CodecInt16Missing,PrettyFastInMemoryDataHandle> prettyFastCodecInt16MissingFactory("int16_missing");
	static CodecFactory<CodecInt8Missing,PrettyFastInMemoryDataHandle> prettyFastCodecInt8MissingFactory("int8_missing");
	static CodecFactory<CodecInt16String,PrettyFastInMemoryDataHandle> prettyFastCodecInt16StringFactory("int16_string");
	static CodecFactory<CodecInt8String,PrettyFastInMemoryDataHandle> prettyFastCodecInt8StringFactory("int8_string");
}

} // namespace codec
} // namespace odb

