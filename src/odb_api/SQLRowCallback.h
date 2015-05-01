/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLRowCallback.h
/// Piotr Kuchta - ECMWF March 2015

#ifndef SQLRowCallback_H
#define SQLRowCallback_H

#include "odb_api/MetaData.h"

namespace odb {
namespace sql {

class SQLRowCallback {
public:
	virtual ~SQLRowCallback();

    virtual void operator()(const double*, size_t, MetaData&, void *aux=0) = 0;

};

} // namespace sql
} // namespace odb

#endif
