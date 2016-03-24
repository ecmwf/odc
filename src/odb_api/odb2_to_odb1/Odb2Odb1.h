/// @file   Odb2Odb1.h
/// @author Anne Fouilloux

#ifndef ODB2_TO_ODB1_H
#define ODB2_TO_ODB1_H

#include <vector>

#include "eckit/filesystem/PathName.h"
#include "odb_api/CommandLineParser.h"
#include "odb_api/StringTool.h"

#define ODB_tag_delim ";"
#define ODB_NMDI 2147483647

#define ODB_HDR_ALIGNED 1
#define ODB_BODY_ALIGNED 2

using namespace std;

struct c_int_array {
  int len;
  int *array;
};

extern "C" {
  typedef void* fortran_ptr;
  void set_err_trap_f90();
  void mpi_setup_f90();
  void mpi_finalize_f90();
  void mpi_print_info_f90();
  int odb_open_f90(char *, int,  char *,int, int *);
  int odb_close_f90(int, bool);
  int odb_get_no_of_columns_f90(int,const char *, int);
  int odb_create_index_f90(int, const char *, int, void *, int*, int);
  int odb_put_one_row_f90(int, const char *, int, double *, int*, int, int);
  int odb_no_of_local_pools_f90(int);
  int odb_get_local_pools_f90(int, struct c_int_array *);
  int odb_swapout_f90(int, const char *, int, int, bool, bool);
  int ODB_maxcols();
  int typeoftable(char *, map<string,int>); // return ODB_HDR_ALIGNED or ODB_BODY_ALIGNED
  int typeoftable_(char *, map<string,int>); // return ODB_HDR_ALIGNED or ODB_BODY_ALIGNED
  void odb_fill_f90(int, const char *, struct c_int_array *, struct c_int_array *, map<string,int>, const char *, int, int);
  int odb_update_links_f90(int, const char *, int, int, struct c_int_array *, struct c_int_array *, map<string,int>);
}

namespace odb{
namespace tool {

class Odb2Odb1 : public StringTool, public CommandLineParser {
 public:

    static void help(ostream & o)
    {
      o << "Shuffle an odb file into npools outputfiles";
    }

   static void usage(const string & name, ostream & o)
    {
      o << name << " -i <prefix_inputfile> -t <table_list> [-o <dbname>] [-npools <npools>]";
    }

   ~Odb2Odb1();

   Odb2Odb1(int argc, char ** argv);

   string name() { return name_; }
   void name(string s) { name_ = s; }

   string outputfile() { return outputfile_; }
   string prefixInputfile() { return prefixInputfile_; }

   const vector<string> tableList();
   std::string tableList(size_t i) { return tableList()[i]; }

   int npools() { return npools_; }

   void create();
   void fillOneTable(string , const int &);

private:
    string name_;
    string prefixInputfile_;
    string outputfile_;
    eckit::PathName tableListFile_;
    vector<int> hdrlen_;
    vector<int> bodylen_;
    map<string,int> tableList_;
    string masterKey_;

    int npools_;
    int handle_;

    vector< vector<double> > data_;

    // No copy allowed

    Odb2Odb1(const Odb2Odb1&);
    Odb2Odb1& operator=(const Odb2Odb1&);

    void tableListInit();
    void updateLinks(string , const int &);
    void computeHdrCount(const eckit::PathName &db);
    void computeBodyCount(const eckit::PathName &db);

};

}
}

#endif // ODB2_TO_ODB1_H
