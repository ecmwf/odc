/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>
#include <memory>

#include "odb_api/data/DataHandleFactory.h"
#include "odb_api/data/PartFileHandleFactory.h"
#include "odb_api/data/MarsHandleFactory.h"
#include "odb_api/data/FileHandleFactory.h"
#include "odb_api/data/HttpHandleFactory.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/parser/StringTools.h"
#include "eckit/memory/ScopedPtr.h"

using namespace eckit;

namespace odc {

DataHandleFactory::DataHandleFactory(const std::string& prefix)
: prefix_(prefix)
{
    factories()[prefix] = this;
}

DataHandleFactory::~DataHandleFactory()
{
    factories().erase(prefix_);
}

std::string DataHandleFactory::prefix() const { return prefix_; }

DataHandleFactory::Storage &DataHandleFactory::factories()
{
    static Storage factories;
    return factories;
}

DataHandle* DataHandleFactory::makeHandle(const std::string& prefix, const std::string& descriptor)
{
    if (factories().find(prefix) == factories().end())
        throw UserError(std::string("No factory for '") + prefix + "://' data descriptors");

    return factories()[prefix]->makeHandle(descriptor);
}

std::pair<std::string, std::string> DataHandleFactory::splitPrefix(const std::string& handleDescriptor)
{
    const std::string delimiter("://");
    size_t pos (handleDescriptor.find(delimiter));
    if (pos != std::string::npos)
        return make_pair(handleDescriptor.substr(0, pos), handleDescriptor.substr(pos + delimiter.size()));

    // Sugar.
    if (StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "retrieve,")
        || StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "stage,")
        || StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "list,")
        || StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "archive,"))
        return std::make_pair(std::string("mars"), handleDescriptor);

    return std::make_pair(std::string("file"), handleDescriptor);
}

void DataHandleFactory::buildMultiHandle(eckit::MultiHandle& mh, const std::string& dataDescriptor)
{
    registerFactories();
    std::vector<std::string> ds;
    ds.push_back(dataDescriptor);
    buildMultiHandle(mh, ds);
}

void DataHandleFactory::buildMultiHandle(eckit::MultiHandle& mh, const std::vector<std::string>& dataDescriptors)
{
    registerFactories();
    std::vector<DataHandle*> handles;
    for (size_t i(0); i < dataDescriptors.size(); ++i)
    {
        std::pair<std::string,std::string> p (splitPrefix(dataDescriptors[i]));
        mh += makeHandle(p.first, p.second);
    }
}

DataHandle* DataHandleFactory::openForRead(const std::string& s)
{
    registerFactories();
    std::pair<std::string,std::string> p (splitPrefix(s));
    std::auto_ptr<DataHandle> d (makeHandle(p.first, p.second));
    d->openForRead();
    return d.release();
}

DataHandle* DataHandleFactory::openForWrite(const std::string& s, const eckit::Length& length)
{
    registerFactories();
    std::pair<std::string,std::string> p (splitPrefix(s));
    std::auto_ptr<DataHandle> d (makeHandle(p.first, p.second));
    d->openForWrite(length);
    return d.release();
}

void DataHandleFactory::registerFactories()
{
    static PartFileHandleFactory partFileHandleFactory;
    static MarsHandleFactory marsHandleFactory;
    static FileHandleFactory fileHandleFactory;
    static HttpHandleFactory httpHandleFactory;
}

} //namespace odc
