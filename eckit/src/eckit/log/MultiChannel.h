/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file MultiChannel.h
/// @author Tiago Quintino

#ifndef eckit_log_MultiChannel_h
#define eckit_log_MultiChannel_h

#include <vector>
#include <map>
#include <streambuf>

#include "eckit/log/Channel.h"
#include "eckit/log/OStreamHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MultiplexBuffer;

class MultiChannel : public Channel {
public:
    
    /// Constructor
    /// No parameters, add target streams later with add() function
    MultiChannel();
    
    /// Destructor
    ~MultiChannel();
    
    /// remove a stream associated to key
    bool remove( const std::string& k );
    
    /// add a stream, passing ownership
    void add( const std::string& k, std::ostream* s );

    /// add a stream, not passing ownership
    void add( const std::string& k, std::ostream& s );

    /// clears all streams from this channel
    void clear();

protected:
    
    MultiplexBuffer* buff_; ///< storage of output streams
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
