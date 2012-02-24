#include "oda.h"
#include "stdint.h"

#include <iostream>
#include <sstream>
#include <limits>
#include <strings.h>

#include "Exceptions.h"
#include "MemoryBlock.h"

#include "Comparator.h"
#include "Codec.h"
#include "DataHandle.h"
#include "DataStream.h"
#include "SQLIteratorSession.h"
#include "DataHandle.h"
#include "UnsafeInMemoryDataHandle.h"

namespace odb {
namespace codec {

template<> map<string, AbstractCodecFactory<DataHandle>* > AbstractCodecFactory<DataHandle>::codecFactories = map<string, AbstractCodecFactory<DataHandle>* >();
template<> map<string, AbstractCodecFactory<FastInMemoryDataHandle>* > AbstractCodecFactory<FastInMemoryDataHandle>::codecFactories = map<string, AbstractCodecFactory<FastInMemoryDataHandle>* >();
template<> map<string, AbstractCodecFactory<PrettyFastInMemoryDataHandle>* > AbstractCodecFactory<PrettyFastInMemoryDataHandle>::codecFactories = map<string, AbstractCodecFactory<PrettyFastInMemoryDataHandle>* >();

Codec::Codec(const string& name)
: name_(name),
  hasMissing_(false),
  missingValue_(MISSING_VALUE_REAL),
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
//template<> void AbstractCodecFactory<DataHandle>::save<OtherByteOrder>(Codec *codec, DataStream<OtherByteOrder,DataHandle> &f) { codecFactories[codec->name()]->save(codec, f.dataHandle(), true); }


CodecFactory<CodecConstant,DataHandle> codecConstantFactory("constant");
CodecFactory<CodecConstantString,DataHandle> codecConstantStringFactory("constant_string");
CodecFactory<CodecConstantOrMissing,DataHandle> codecConstantOrMissingFactory("constant_or_missing");
CodecFactory<CodecRealConstantOrMissing,DataHandle> codecRealConstantOrMissingFactory("real_constant_or_missing");
CodecFactory<CodecChars,DataHandle> codecCharsFactory("chars");
CodecFactory<CodecLongReal,DataHandle> codecLongRealFactory("long_real");
CodecFactory<CodecShortReal,DataHandle> codecShortRealFactory("short_real");
CodecFactory<CodecInt32,DataHandle> codecInt32Factory("int32");
CodecFactory<CodecInt16,DataHandle> codecInt16Factory("int16");
CodecFactory<CodecInt8,DataHandle> codecInt8Factory("int8");
CodecFactory<CodecInt16Missing,DataHandle> codecInt16MissingFactory("int16_missing");
CodecFactory<CodecInt8Missing,DataHandle> codecInt8MissingFactory("int8_missing");
CodecFactory<CodecInt16String,DataHandle> codecInt16StringFactory("int16_string");
CodecFactory<CodecInt8String,DataHandle> codecInt8StringFactory("int8_string");

CodecFactory<CodecConstant,FastInMemoryDataHandle> fastCodecConstantFactory("constant");
CodecFactory<CodecConstantString,FastInMemoryDataHandle> fastCodecConstantStringFactory("constant_string");
CodecFactory<CodecConstantOrMissing,FastInMemoryDataHandle> fastCodecConstantOrMissingFactory("constant_or_missing");
CodecFactory<CodecRealConstantOrMissing,FastInMemoryDataHandle> fastCodecRealConstantOrMissingFactory("real_constant_or_missing");
CodecFactory<CodecChars,FastInMemoryDataHandle> fastCodecCharsFactory("chars");
CodecFactory<CodecLongReal,FastInMemoryDataHandle> fastCodecLongRealFactory("long_real");
CodecFactory<CodecShortReal,FastInMemoryDataHandle> fastCodecShortRealFactory("short_real");
CodecFactory<CodecInt32,FastInMemoryDataHandle> fastCodecInt32Factory("int32");
CodecFactory<CodecInt16,FastInMemoryDataHandle> fastCodecInt16Factory("int16");
CodecFactory<CodecInt8,FastInMemoryDataHandle> fastCodecInt8Factory("int8");
CodecFactory<CodecInt16Missing,FastInMemoryDataHandle> fastCodecInt16MissingFactory("int16_missing");
CodecFactory<CodecInt8Missing,FastInMemoryDataHandle> fastCodecInt8MissingFactory("int8_missing");
CodecFactory<CodecInt16String,FastInMemoryDataHandle> fastCodecInt16StringFactory("int16_string");
CodecFactory<CodecInt8String,FastInMemoryDataHandle> fastCodecInt8StringFactory("int8_string");

CodecFactory<CodecConstant,PrettyFastInMemoryDataHandle> prettyFastCodecConstantFactory("constant");
CodecFactory<CodecConstantString,PrettyFastInMemoryDataHandle> prettyFastCodecConstantStringFactory("constant_string");
CodecFactory<CodecConstantOrMissing,PrettyFastInMemoryDataHandle> prettyFastCodecConstantOrMissingFactory("constant_or_missing");
CodecFactory<CodecRealConstantOrMissing,PrettyFastInMemoryDataHandle> prettyFastCodecRealConstantOrMissingFactory("real_constant_or_missing");
CodecFactory<CodecChars,PrettyFastInMemoryDataHandle> prettyFastCodecCharsFactory("chars");
CodecFactory<CodecLongReal,PrettyFastInMemoryDataHandle> prettyFastCodecLongRealFactory("long_real");
CodecFactory<CodecShortReal,PrettyFastInMemoryDataHandle> prettyFastCodecShortRealFactory("short_real");
CodecFactory<CodecInt32,PrettyFastInMemoryDataHandle> prettyFastCodecInt32Factory("int32");
CodecFactory<CodecInt16,PrettyFastInMemoryDataHandle> prettyFastCodecInt16Factory("int16");
CodecFactory<CodecInt8,PrettyFastInMemoryDataHandle> prettyFastCodecInt8Factory("int8");
CodecFactory<CodecInt16Missing,PrettyFastInMemoryDataHandle> prettyFastCodecInt16MissingFactory("int16_missing");
CodecFactory<CodecInt8Missing,PrettyFastInMemoryDataHandle> prettyFastCodecInt8MissingFactory("int8_missing");
CodecFactory<CodecInt16String,PrettyFastInMemoryDataHandle> prettyFastCodecInt16StringFactory("int16_string");
CodecFactory<CodecInt8String,PrettyFastInMemoryDataHandle> prettyFastCodecInt8StringFactory("int8_string");

} // namespace codec
} // namespace odb

