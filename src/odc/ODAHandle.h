/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODAHandle.h
/// @author Piotr Kuchta - ECMWF Aug 2011

#ifndef ODAHandle_H
#define ODAHandle_H

#include "eckit/io/Offset.h"
#include "eckit/log/Log.h"

#include "odc/LibOdc.h"
#include "odc/ODATranslator.h"

namespace odc {


class ODAHandle {
public:

    ODAHandle(eckit::Offset, eckit::Offset);
    ~ODAHandle();

    void addValue(const std::string& columnName, double v);

    template <typename T>
    void getValue(const std::string& name, T& value) {
        value = ODATranslator<T>()(values_[name]);
        LOG_DEBUG_LIB(odc::LibOdc) << "ODAHandle::getValue('" << name << "',=>" << value << ")" << std::endl;
    }

    void print(std::ostream&) const;

    eckit::Offset start() { return start_; }
    void start(const eckit::Offset& n) { start_ = n; }

    eckit::Offset end() { return end_; }
    void end(const eckit::Offset& n) { end_ = n; }

private:

    // No copy allowed
    ODAHandle(const ODAHandle&);
    ODAHandle& operator=(const ODAHandle&);

    eckit::Offset start_;
    eckit::Offset end_;
    std::map<std::string, double> values_;

    friend std::ostream& operator<<(std::ostream& s, const ODAHandle& p) {
        p.print(s);
        return s;
    }
};

}  // namespace odc

#endif
