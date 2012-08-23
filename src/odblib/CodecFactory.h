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

template <typename DATAHANDLE>
class AbstractCodecFactory {
public:
	static Codec* getCodec(const string& name, bool differentByteOrder) {
		return codecFactories[name]->create(differentByteOrder);
	}

	static Codec* loadCodec(DATAHANDLE *dh, bool differentByteOrder) {
		Codec::loadCodecs();
		string name;
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
		Codec *codec = factory->load(dh, differentByteOrder);
		return codec;
	}

	
	template<typename BYTEORDER>
		static void save(Codec *codec, DataStream<BYTEORDER,DATAHANDLE> &f);

	virtual Codec* create(bool sourceHasDifferentByteOrder) = 0;

	virtual Codec* load(DataHandle *, bool sourceHasDifferentByteOrder) = 0;
	virtual Codec* load(FastInMemoryDataHandle*, bool sourceHasDifferentByteOrder) { NOTIMP; return 0; } //= 0;

	virtual void save(Codec *, DataHandle *, bool sourceHasDifferentByteOrder) = 0;
	
protected:
	AbstractCodecFactory(const string& name) { codecFactories[name] = this; }

private:
	static map<string, AbstractCodecFactory<DATAHANDLE> *> codecFactories;
};


template <template <typename> class CODEC,typename DATAHANDLE>
class CodecFactory : public AbstractCodecFactory<DATAHANDLE> {
public:
	CodecFactory (const string& name) : AbstractCodecFactory<DATAHANDLE>(name) { }

	virtual Codec* create(bool sourceHasDifferentByteOrder)
	{
		if (sourceHasDifferentByteOrder)
			return new CODEC<OtherByteOrder>; 
		else
			return new CODEC<SameByteOrder>;
	}

	virtual Codec* load(DataHandle *dh, bool sourceHasDifferentByteOrder)
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

	virtual void save(Codec *codec, DataHandle *dh, bool sourceHasDifferentByteOrder)
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


#endif // odblib_CodecFactory_H
