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

    DDLPrinter(const std::string& path, const std::string& tableName)
    : path_(path), tableName_(tableName) {}

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
            printTable(o, md_[i], tableName_, path_);
    }

    static std::string typeName(const odb::Column& c)
    {
        switch (c.type())
        {
            case STRING:   return "STRING";
            case INTEGER:  return "INTEGER";
            case BITFIELD: return "INTEGER";
            case REAL:     return "REAL";
            case DOUBLE:   return "DOUBLE";
            default:
                throw new Exception("unknown type");
        }
    }

    static std::pair<std::string,std::string> typeDefinitionAndName(const std::string& tableName, const odb::Column& c)
    {
        std::stringstream definition;
        std::string type_name (typeName(c));

        if (c.type() == BITFIELD)
        {
            const eckit::sql::BitfieldDef& bd (c.bitfieldDef());
            const std::vector<std::string>& fieldNames (bd.first);
            const std::vector<int>& sizes (bd.second);

            type_name = stripAtTable(tableName, c.name()) + "_at_" + tableName + "_t";

            definition << "CREATE TYPE " << type_name << " AS (";
            for (size_t i(0); i < sizes.size(); ++i)
                definition << fieldNames[i] << " bit" << sizes[i] 
                           << ((i+1 < sizes.size()) ? ", " : "");
            definition << ");\n";
        }

        return make_pair(definition.str(), type_name);
    }


    static std::string stripAtTable(const std::string& tableName, const std::string& columnName)
    {
        std::string suffix ( std::string("@") + tableName ); 

        if (columnName.size() >= suffix.size()
            && columnName.compare(columnName.size() - suffix.size(), suffix.size(), suffix) == 0)
            return columnName.substr(0, columnName.size() - suffix.size());

        return columnName; 
    }

    static void printTable(std::ostream& s, const odb::MetaData& md, const std::string& tableName, const std::string& path)
    {
        std::stringstream create_type, create_table;

        create_table << "CREATE TABLE " << tableName << " AS (\n";
        for (size_t i (0); i < md.size(); ++i)
        {
            std::pair<std::string,std::string> p (typeDefinitionAndName(tableName, *md[i]));
            const std::string& def (p.first), 
                               type (p.second);

            create_type << def;
            create_table << "  " << stripAtTable(tableName, md[i]->name()) << " " << type << ",\n";
        }
        create_table << ") ON '" << path << "';\n";

        s << create_type.str();
        s << create_table.str();
    }

private:
	std::vector<odb::MetaData> md_;
    const std::string path_;
    const std::string tableName_;
};

HeaderTool::HeaderTool (int argc, char *argv[]) : Tool(argc, argv) {}

void HeaderTool::run()
{
    registerOptionWithArgument("-table");
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
	DDLPrinter ddlPrinter (db, optionArgument("-table", std::string("foo")));

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

