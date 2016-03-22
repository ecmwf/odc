/// @file   Odb2Odb1.cc
/// @author Anne Fouilloux

#include "Odb2Odb1.h"

#include <iostream>
#include <sstream>

#include "odb_api/odbcapi.h"
#include "odb_api/Select.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/MetaDataReader.h"


using namespace std;
using namespace odb;
using namespace eckit;

extern "C" {
//=================================
int typeoftable(char *c_linkname, map<string,int> flaglist) // return ODB_HDR_ALIGNED or ODB_BODY_ALIGNED
//=================================
{
  int type = -1;
  map < string, int> ::iterator it;
  string linkname = c_linkname;
  int pl= linkname.find("(");
  int pr= linkname.find(")");
  int pa= linkname.find("@");
  string table;
  if (pl > 0 && pr > 0) {
    table = linkname.substr(pl+1, pr-pl-1);;
  } else if (pa!=string::npos) {
    table = linkname.substr(pa+1);;
  } else {
    table = linkname;
  }


  size_t startpos = table.find_first_not_of(" \t");
  size_t endpos = table.find_last_not_of(" \t");
  if((string::npos == startpos) || (string::npos == endpos))
      table="";
  else
      table=table.substr(startpos, endpos - startpos + 1);

/*
   for(it = flaglist.begin(); it != flaglist.end(); ++it)
     {
  Log::info() << linkname << " typeoftable table " << table << "|" << it->first << "|" << endl;
   }
*/

  it=flaglist.find(table);
  if (it != flaglist.end()) {
    type = it->second;
//     Log::info() << linkname << " typeoftable table " << table << "|" << it->first << "|" << endl;
  }

  return type;

}

//=================================
int typeoftable_(char *c_linkname, map<string,int> flaglist) // return ODB_HDR_ALIGNED or ODB_BODY_ALIGNED
//=================================
{
  return typeoftable(c_linkname, flaglist);
}

}

namespace odb {
namespace tool {

//=================================
void Odb2Odb1::tableListInit()
//=================================
{
 Log::info() << "Odb2Odb1::tableListInit " << tableListFile_ << endl;

 vector<string> tablelist = StringTool::readLines(tableListFile_);

 int type = ODB_NMDI;
 for (size_t i = 0; i < tablelist.size(); ++i)
  {
    int idx_hdr=tablelist[i].find("# HDR_ALIGNED TABLES");
    if (idx_hdr!=string::npos) {
      type = ODB_HDR_ALIGNED;
    }
    int idx_body=tablelist[i].find("# BODY_ALIGNED TABLES");
    if (idx_body!=string::npos) {
      type = ODB_BODY_ALIGNED;
    }
    if (idx_hdr == string::npos && idx_body == string::npos) {
      tableList_[tablelist[i]] = type;
    }
  }
}


//=================================
Odb2Odb1::Odb2Odb1(int argc, char ** argv)
//=================================
: tableList_(), CommandLineParser(argc, argv)
{
   registerOptionWithArgument("-i");
   registerOptionWithArgument("-o");
   registerOptionWithArgument("-npools");
   registerOptionWithArgument("-t");
   registerOptionWithArgument("-masterKey");

   mpi_setup_f90();
   set_err_trap_f90();
   if (! optionIsSet("-i") || ! optionIsSet("-t") ) {
     Log::error() << "Usage: ";
     usage(parameters(0), Log::error());
     Log::error() << endl;
   } else {

     prefixInputfile_ = optionArgument("-i", std::string(""));
     masterKey_ = optionArgument("-masterKey", std::string("seqno"));

     if (optionIsSet("-o")) {
       outputfile_ = optionArgument("-o", std::string(""));;
     } else {
       outputfile_ = "ECMA";
     }
     npools_ = optionArgument("-npools",1);


     tableListFile_ = optionArgument("-t", std::string(""));

     mpi_print_info_f90();
     string open_mode="NEW";
     Log::info() << "File to open " << outputfile_.c_str() << " " << outputfile_.size() << endl;
     handle_ = odb_open_f90(const_cast<char *>(outputfile_.c_str()),outputfile_.size(),
                            const_cast<char *>(open_mode.c_str()), open_mode.length(),&npools_);
     tableListInit();
   }
}

//=================================
Odb2Odb1::~Odb2Odb1()
//=================================
{
  //Log::info() <<  "destructor Odb2Odb1 handle_ = " << handle_ << endl;
  if (handle_>0)
    odb_close_f90(handle_,true);

  mpi_finalize_f90();
}


//=================================
void Odb2Odb1::computeHdrCount(const PathName &db)
//=================================
{
    string sql = "select distinct " + masterKey_ + " from \"" + db + "\";";
    odb::Select sodb(sql);

    odb::Select::iterator it = sodb.begin();
    odb::Select::iterator end = sodb.end();
    hdrlen_.clear();
    for( ; it != end; ++it) {
      size_t seqno_index = it->columns().columnIndex(masterKey_);
      hdrlen_.push_back((*it)[seqno_index]);
    }
}

//=================================
void Odb2Odb1::computeBodyCount(const PathName &db)
//=================================
{
    unsigned long long n = 0;

    if ( db != "") {
    typedef MetaDataReader<MetaDataReaderIterator> MDR;
    MDR mdReader(db);
    MDR::iterator it = mdReader.begin();
    MDR::iterator end = mdReader.end();
    for (; it != end; ++it)
    {
        const MetaData &md = it->columns();
        n += md.rowsNumber();
    }
    bodylen_.resize(n,0);
    }
}

//=================================
void Odb2Odb1::updateLinks(string table, const int &np)
//=================================
{
  PathName inputfile;
  std::stringstream out;
  out << np;
  vector<string> sqlBuf;
  PathName sqlFile;
  StringTool::trimInPlace(table);
  int err;
  string sql = table + "_update_links";
  c_int_array c_hdrlen;
  c_hdrlen.len=hdrlen_.size();
  c_hdrlen.array = &hdrlen_[0];
  c_int_array c_bodylen;
  c_bodylen.len=bodylen_.size();
  c_bodylen.array = &bodylen_[0];
  Log::info() << " SQL SELECT: " << sql << " hdrlen_size = " << hdrlen_.size() << " bodylen_size = " << bodylen_.size() << endl;
  err = odb_update_links_f90(handle_, sql.c_str(), sql.size(), np, &c_hdrlen, &c_bodylen, tableList_);
}



//=================================
void Odb2Odb1::create()
//=================================
{
 int nmypools = odb_no_of_local_pools_f90(handle_);
 vector<int> cMypools(nmypools);
 c_int_array arrays;
 arrays.len = cMypools.size();
 arrays.array = &cMypools[0];
 nmypools = odb_get_local_pools_f90(handle_, &arrays);

 cout << " I have " << nmypools << " pools on my processor" << endl;
/*
  for (size_t i = 0; i < tableList_.size(); ++i)
   {
     Log::info() << "Table " << tableList_[i] << endl;
   }
*/

 int np;
 for (int jp = 0 ; jp < nmypools ; jp++)
  {
   np = cMypools[jp];
   PathName inputfile;
   std::stringstream out;
   out << np;
   inputfile = prefixInputfile_  + "." + out.str() + ".odb";
   if (inputfile.exists()) {
     Log::info() << " Processing pools : " << np << " with file " << inputfile << endl;
     computeHdrCount(inputfile);
     computeBodyCount(inputfile);
     c_int_array c_hdrlen;
     c_hdrlen.len=hdrlen_.size();
     c_hdrlen.array = &hdrlen_[0];
     c_int_array c_bodylen;
     c_bodylen.len=bodylen_.size();
     c_bodylen.array = &bodylen_[0];

     odb_fill_f90(handle_, string(inputfile).c_str(), &c_hdrlen,&c_bodylen, tableList_, masterKey_.c_str(), masterKey_.size(), np);
     // update links for this pool
     map<string, int>::const_iterator it;
     for(it = tableList_.begin(); it != tableList_.end(); ++it)
       {
       updateLinks((*it).first, np);
       }
      string dtname="*";
      int err = odb_swapout_f90(handle_, dtname.c_str(), dtname.size(), np, true, true);

    }
  }
}

//=================================
}
}
