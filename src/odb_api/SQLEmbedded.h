/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLEmbedded.h
// Piotr Kuchta - ECMWF September 2016

#ifndef SQLEmbedded_H
#define SQLEmbedded_H

#include "ecml/core/ExecutionContext.h"

//#include "odb_api/Expressions.h"
#include "odb_api/SQLStatement.h"
#include "odb_api/SQLAST.h"

namespace odb {
namespace sql {
// Forward declarations

class SQLDatabase;

class SQLEmbedded : public SQLStatement {
public:
	SQLEmbedded(const EmbeddedAST&);
	virtual ~SQLEmbedded(); 

	virtual unsigned long long execute(ecml::ExecutionContext*);
	virtual expression::Expressions output() const;

    const std::string& code() { return code_; }

protected:
	virtual void print(std::ostream&) const; 	

private:
// No copy allowed
	SQLEmbedded(const SQLEmbedded&);
	SQLEmbedded& operator=(const SQLEmbedded&);

	friend std::ostream& operator<<(std::ostream& s,const SQLEmbedded& p)
		{ p.print(s); return s; }

    std::string code_;

};

} // namespace sql 
} // namespace odb 

#endif
