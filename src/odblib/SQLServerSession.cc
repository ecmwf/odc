/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Log.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLSession.h"
#include "SQLServerSession.h"
#include "SQLServerOutput.h"
#include "SQLStatement.h"
#include "SQLParser.h"
#include "SQLSelectFactory.h"

namespace odb {
namespace sql {

SQLServerSession::SQLServerSession(istream& in,ostream& out):
	in_(in),
	out_(out),
	swap_(false),
	last_(0),
	next_(0)
{}

SQLServerSession::~SQLServerSession()
{
	for(map<int,SQLStatement*>::iterator j = prepared_.begin(); j != prepared_.end(); ++j)
		delete (*j).second;
}

SQLOutput* SQLServerSession::defaultOutput()
{
	return new SQLServerOutput(*this,swap_);
}

void SQLServerSession::serve()
{
	string command;
	while(in_ >> command)
	{
		Log::info() << "SQLServerSession [" << command << "]" << endl;

		last_ = 0;
	
		try {
		
			if(command == "LOGIN")		  commandLOGIN();
			else if(command == "DO")      commandDO();
			else if(command == "PREPARE") commandPREPARE();
			else if(command == "EXECUTE") commandEXECUTE();
			else if(command == "DESTROY") commandDESTROY();
			else if(command == "PARAM")   commandPARAM();
			else throw UserError("Unknow command",command);

			Log::info() << "Send success" << endl;
			out_ << "000 OK" << endl << flush;

		}
		catch(exception& e)
		{
			Log::info() << "Send error" << e.what() << endl;
			out_ << "999 " << e.what() << endl << flush;
		}
	}
}

void SQLServerSession::statement(SQLStatement *sql)
{
	last_ = sql;
}

void SQLServerSession::commandDO()
{
	parseStatement();
	try {
		if(last_) execute(*last_);
	}
	catch(...)
	{
		delete last_;
		last_ = 0;
		throw;
	}
}

void SQLServerSession::commandLOGIN()
{
	// Check endian
	const char*            byte = "BYTE";
	const unsigned long *endian = reinterpret_cast<const unsigned long*>(byte);

	unsigned long remote_endian;
	in_ >> remote_endian;

	if(remote_endian == *endian)
	{
		Log::info() << "Local and remote endian match" << endl;
		swap_ = false;
	}
	else
	{
		Log::info() << "Local and remote endian don't match " << *endian << " " << remote_endian << endl;
		swap_ = true;
	}

}

void SQLServerSession::commandPREPARE()
{
	parseStatement();
	if(!last_)
	{
		out_ << "100 0" << endl;
		return;
	}

	prepared_[++next_] = last_;

	out_ << "100 " << next_ << endl;
	//out_ << "101 " << data_.localHost() << endl;
	out_ << "102 " << data_.localPort() << endl;

	expression::Expressions output; // = last_->output();

	out_ << "103 " <<  output.size() << endl;
	for(size_t i = 0; i < output.size() ; i++ )
	{
		cout << "104 " << output[i]->title()           << endl;
		//cout << "105 " << output[i]->type()->getKind() << endl;
		out_ << "104 " << output[i]->title()           << endl;
		//out_ << "105 " << output[i]->type()->getKind() << endl;
	}

	Log::info() << "commandPREPARE " << next_ << " " << data_.localHost() << " " << data_.localPort() << endl;
	last_ = 0;
}

void SQLServerSession::commandPARAM()
{
	int count;
	in_ >> count;
	for(int i = 0; i < count ; i++)
	{
		double d;
		in_ >> d;
		setParameter(i,d);
	}
}

void SQLServerSession::commandEXECUTE()
{
	int which;
	in_ >> which;

	if(which == 0)
		return;

	ASSERT(prepared_.find(which) != prepared_.end());

	// We need a mutex, so we don;t mixup sockets

	execute(*prepared_[which]);

}

void SQLServerSession::commandDESTROY()
{
	int which;
	in_ >> which;

	if(which == 0)
		return;

	ASSERT(prepared_.find(which) != prepared_.end());
	SQLStatement *sql = prepared_[which];
	delete sql;
	prepared_.erase(which);
}

void SQLServerSession::parseStatement()
{
	string sql;
	string x;

	while(sql.find(";") == string::npos )
	{
		in_ >> x;
		sql += " ";
		sql += x;
	}

	Log::info() << sql << endl;

	SQLParser::parseString(sql, static_cast<DataHandle*>(0), odb::sql::SQLSelectFactory::instance().config());

}

} // namespace sql
} // namespace odb
