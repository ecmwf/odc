/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odblib_CodecFactory_H
#define odblib_CodecFactory_H

#include "eckit/exception/Exceptions.h"
#include "odc/UnsafeInMemoryDataHandle.h"
#include "odc/DataStream.h"


namespace eckit {
class DataHandle;
}

namespace odc {
namespace codec {

class Codec;
//template<class A, class B> class DataStream;

class CodecFactoryBase {
protected:
    static void loadCodecs();
};


template <typename DATAHANDLE>
class AbstractCodecFactory : public CodecFactoryBase {
public:
	static Codec* getCodec(const std::string& name, bool differentByteOrder) {
		return codecFactories[name]->create(differentByteOrder);
	}

	static Codec* loadCodec(DATAHANDLE *dh, bool differentByteOrder) {
        loadCodecs();
		std::string name;
		if (differentByteOrder)
		{
			DataStream<OtherByteOrder,DATAHANDLE> ds(dh);
			ds.readString(name);
		}
		else
		{
			DataStream<SameByteOrder,DATAHANDLE> ds(dh);
			ds.readString(name);
		}
		AbstractCodecFactory *factory = codecFactories[name];
		if (factory == 0)
		{
            std::stringstream ss;
			ss << "Unknown codec '" << name << "'. You may need to use a newer version of ODB API.";
			throw eckit::UserError(ss.str());
		}
		Codec *codec = factory->load(dh, differentByteOrder);
		return codec;
	}

	
	template<typename BYTEORDER>
		static void save(Codec *codec, DataStream<BYTEORDER,DATAHANDLE> &f);

	virtual Codec* create(bool sourceHasDifferentByteOrder) = 0;

	virtual Codec* load(eckit::DataHandle *, bool sourceHasDifferentByteOrder) = 0;
	virtual Codec* load(FastInMemoryDataHandle*, bool sourceHasDifferentByteOrder) { NOTIMP; return 0; } //= 0;

	virtual void save(Codec *, eckit::DataHandle *, bool sourceHasDifferentByteOrder) = 0;
	
protected:
	AbstractCodecFactory(const std::string& name) { codecFactories[name] = this; }

private:
	static std::map<std::string, AbstractCodecFactory<DATAHANDLE> *> codecFactories;
};


template <template <typename> class CODEC,typename DATAHANDLE>
class CodecFactory : public AbstractCodecFactory<DATAHANDLE> {
public:
	CodecFactory (const std::string& name) : AbstractCodecFactory<DATAHANDLE>(name) { }

	virtual Codec* create(bool sourceHasDifferentByteOrder)
	{
		if (sourceHasDifferentByteOrder)
			return new CODEC<OtherByteOrder>; 
		else
			return new CODEC<SameByteOrder>;
	}

	virtual Codec* load(eckit::DataHandle *dh, bool sourceHasDifferentByteOrder)
	{
		if(sourceHasDifferentByteOrder)
		{
			CODEC<OtherByteOrder> *codec = new CODEC<OtherByteOrder>;
			codec->load(dh);
			return codec;
		}
		else
		{
			CODEC<SameByteOrder> *codec = new CODEC<SameByteOrder>;
			codec->load(dh);
			return codec;
		}
	}

	virtual void save(Codec *codec, eckit::DataHandle *dh, bool sourceHasDifferentByteOrder)
	{
		if(sourceHasDifferentByteOrder)
		{
			CODEC<OtherByteOrder> *theCodec = static_cast<CODEC<OtherByteOrder> *>(codec);
			theCodec->save(dh);
		}
		else
		{
			CODEC<SameByteOrder> *theCodec = static_cast<CODEC<SameByteOrder> *>(codec);
			theCodec->save(dh);
		}
	}
};

}
}


#endif // odblib_CodecFactory_H
