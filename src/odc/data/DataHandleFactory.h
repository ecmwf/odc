/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2015

#ifndef odc_data_DataHandleFactory_H
#define odc_data_DataHandleFactory_H

#include <string>
#include <map>

#include "eckit/filesystem/PathName.h"

namespace eckit { 
    class DataHandle;
    class MultiHandle;
    class Length; 
}

namespace odc {

class DataHandleFactory {
public:  // Types
    typedef std::map<std::string, DataHandleFactory*> Storage;

public:
    virtual ~DataHandleFactory();

    static eckit::DataHandle* openForRead(const std::string&);
    static eckit::DataHandle* openForWrite(const std::string&, const eckit::Length& = eckit::Length(0));

    static void buildMultiHandle(eckit::MultiHandle&, const std::vector<std::string>&);
    static void buildMultiHandle(eckit::MultiHandle&, const std::string&);

protected:
    DataHandleFactory(const std::string&);

    static std::pair<std::string,std::string> splitPrefix(const std::string&);

    static eckit::DataHandle* makeHandle(const std::string&, const std::string&);

    virtual eckit::DataHandle* makeHandle(const std::string&) const = 0;
    std::string prefix() const;

private:
    DataHandleFactory();
    static Storage& factories();
    static void registerFactories();

    std::string prefix_;
};

} // namespace odc

#endif
