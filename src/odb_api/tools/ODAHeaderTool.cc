/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ODAHeaderTool.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"

using namespace eckit;

namespace odb {
namespace tool {

typedef odb::MetaDataReader<odb::MetaDataReaderIterator> MDReader;

class MDPrinter {
public:
	virtual void print(std::ostream&, MDReader::iterator &) = 0;
	virtual void printSummary(std::ostream&) {};
};

class VerbosePrinter : public MDPrinter {
public:
	VerbosePrinter() : headerCount_() {}
	void print(std::ostream& o, MDReader::iterator &r)
	{
        o << std::endl << "Header " << ++headerCount_ << ". "
			<< "Begin offset: " << (**r).blockStartOffset() << ", end offset: " << (**r).blockEndOffset()
			<< ", number of rows in block: " << r->columns().rowsNumber() 
			<< ", byteOrder: " << (((**r).byteOrder() == 1) ? "same" : "other")
            << std::endl
			<< r->columns();
	}
private:
	unsigned long headerCount_;
};

class OffsetsPrinter : public MDPrinter {
public:
	OffsetsPrinter() {}
	void print(std::ostream& o, MDReader::iterator &r)
	{
		Offset offset ((**r).blockStartOffset());
		Length length ((**r).blockEndOffset() - (**r).blockStartOffset());
		o << offset << " " << length << " " << r->columns().rowsNumber() << " " << r->columns().size() << std::endl;
	}
private:
	unsigned long headerCount_;
};

class DDLPrinter : public MDPrinter {
public:

    DDLPrinter(const std::string& path) : path_(path) {}

	void print(std::ostream& o, MDReader::iterator &r)
	{
        if (md_.empty() || md_.back() != r->columns())
        {
			md_.push_back(r->columns());
            return;
        }
	}

	void printSummary(std::ostream& o) 
    {
        for (size_t i(0); i < md_.size(); ++i)
            printTable(o, md_[i], "foo", path_);
    }

    static void printTable(std::ostream& o, const odb::MetaData& md, const std::string& tableName, const std::string& path)
    {
        o << "CREATE TABLE " << tableName << " AS (";
        for (size_t i (0); i < md.size(); ++i)
            o << md[i]->name() << " integer,\n";
        o << ") ON '" << path << "';\n";
    }

private:
	std::vector<odb::MetaData> md_;
    const std::string& path_;
};

HeaderTool::HeaderTool (int argc, char *argv[]) : Tool(argc, argv) {}

void HeaderTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

	const std::string db (parameters(1));
    std::ostream& o (std::cout);

	VerbosePrinter verbosePrinter;
	OffsetsPrinter offsetsPrinter;
	DDLPrinter ddlPrinter (db);

	MDPrinter& printer(* 
        (optionIsSet("-offsets") ? static_cast<MDPrinter*>(&offsetsPrinter) : 
         optionIsSet("-ddl")     ? static_cast<MDPrinter*>(&ddlPrinter) :
                                   static_cast<MDPrinter*>(&verbosePrinter)));

	MDReader oda(db);
	MDReader::iterator r(oda.begin());
	MDReader::iterator end(oda.end());

	odb::MetaData metaData(r->columns());
	for(; r != end; ++r)
	{
		ASSERT (r->isNewDataset());
		printer.print(o, r);
		metaData = r->columns();
	}
    printer.printSummary(o);
}

} // namespace tool 
} // namespace odb 

