/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODBBehavior_h
#define ODBBehavior_h

#include "eclib/ContextBehavior.h"

namespace eclib {
	class DHSLogger;
}

namespace odb {

class ODBBehavior : public eclib::ContextBehavior {
public:
    ODBBehavior();
    ~ODBBehavior();
    
protected: // methods

    virtual void initialize();
    virtual void finalize();
    
    virtual string runName() const;
    virtual void runName( const string& name ); 
    
    virtual long taskId() const;
    
    virtual eclib::Logger* createInfoLogger();
    virtual eclib::Logger* createDebugLogger();
    virtual eclib::Logger* createWarningLogger();
    virtual eclib::Logger* createErrorLogger();

    virtual eclib::DHSLogger* getLogger();
    
private: // members
    
    long taskId_;
    ostream& out_;
    string name_;
    
};

} // namespace odb

#endif 

