/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Jan 2009
/// @date Aug 2018

#ifndef odc_sql_ODAOutput_H
#define odc_sql_ODAOutput_H


#include "eckit/sql/SQLOutput.h"
#include "odc/core/MetaData.h"

#include "odc/DispatchingWriter.h"
#include "odc/WriterBufferingIterator.h"

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

template <typename WRITER>
class ODAOutput : public eckit::sql::SQLOutput {
public:

    ODAOutput(WRITER*);
    //	ODAOutput(WRITER*, const MetaData&);
    virtual ~ODAOutput();  // Change to virtual if base class

private:  // methods

    virtual void print(std::ostream&) const override;

    // -- Members

    std::unique_ptr<WRITER> writer_;

    typename WRITER::iterator it_;
    int col_;
    std::vector<size_t> columnSizes_;
    std::vector<double> missingValues_;

    //    MetaData metaData_;

    unsigned long long count_;
    bool initted_;

    // -- Overridden methods
    virtual void reset() override;
    virtual void flush() override;
    virtual bool output(const eckit::sql::expression::Expressions&) override;
    virtual void preprepare(eckit::sql::SQLSelect&) override;
    virtual void prepare(eckit::sql::SQLSelect&) override;
    virtual void cleanup(eckit::sql::SQLSelect&) override;
    virtual void updateTypes(eckit::sql::SQLSelect&) override;
    virtual unsigned long long count() override;

    void initUpdateTypes(eckit::sql::SQLSelect&);

    // Overridden (and removed) functions

    virtual void outputReal(double, bool) override;
    virtual void outputDouble(double, bool) override;
    virtual void outputInt(double, bool) override;
    virtual void outputUnsignedInt(double, bool) override;
    virtual void outputString(const char*, size_t, bool) override;
    virtual void outputBitfield(double, bool) override;
    void outputNumber(double, bool);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql

//----------------------------------------------------------------------------------------------------------------------

// We explicitly instantiate this template in the .cc file, so don't instantiate in each
// translation unit.

class WriterBufferingIterator;
template <typename ITERATOR>
class Writer;
class DispatchingWriter;

extern template class sql::ODAOutput<Writer<WriterBufferingIterator>>;
extern template class sql::ODAOutput<DispatchingWriter>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
