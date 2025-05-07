/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef odc_utility_Tracer_H
#define odc_utility_Tracer_H

#include <iosfwd>
#include <string>

namespace odc {
namespace utility {

//----------------------------------------------------------------------------------------------------------------------

class Tracer {
public:

    Tracer(std::ostream&, const std::string&);
    ~Tracer();

private:

    std::ostream& out_;
    std::string message_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace utility
}  // namespace odc

#endif
