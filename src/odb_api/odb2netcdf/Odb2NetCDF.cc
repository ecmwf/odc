#include "Odb2NetCDF.h"

#include <iostream>
#include <netcdfcpp.h>
#include <algorithm>

#include <odb_api/Reader.h>
#include <odb_api/Select.h>

#include "eckit/exception/Exceptions.h"

/// @author Anne Fouilloux

using namespace std;
using namespace eckit;

/// Replaces all occurences of '@' with '_'
string patchName(const string& s)
{
    string r (s);
    replace(r.begin(), r.end(), '@', '_');
    return r;
}

Odb2NetCDF::Odb2NetCDF(const string & inputfile, const string & outputfile)
: inputfile_(inputfile), outputfile_(outputfile)
{}

Odb2NetCDF::~Odb2NetCDF() {}

Odb2NetCDF_1D::Odb2NetCDF_1D(const string & inputfile, const string & outputfile)
: Odb2NetCDF(inputfile,outputfile)
{
    Log::info() << "The following files will be opened and read: " << endl;
    Log::info() << "ODB filename = " <<  inputfile << endl;
    Log::info() << "Output NetCDF filename = " << outputfile << endl;
}

vector<NcVar*> Odb2NetCDF_1D::createVariables(NcFile& dataFile, const odb::MetaData& columns, NcDim* xDim)
{
    vector<NcVar*> vars;
    for (size_t i(0); i < columns.size(); ++i)
    {
        const odb::Column& column (*(columns[i]));
        NcVar* v;
        switch (column.type())
        {
            case odb::INTEGER:
            case odb::BITFIELD:
                v = dataFile.add_var(patchName(column.name()).c_str(), ncInt, xDim);
                v->add_att(_FillValue, int (column.missingValue()));
                break;
            case odb::REAL:
                v = dataFile.add_var(patchName(column.name()).c_str(), ncFloat, xDim);
                v->add_att(_FillValue, float (column.missingValue()));
                break;
            case odb::DOUBLE:
                v = dataFile.add_var(patchName(column.name()).c_str(), ncDouble, xDim);
                v->add_att(_FillValue, double (column.missingValue()));
                break;
            case odb::STRING:
                v = dataFile.add_var(patchName(column.name()).c_str(), ncChar, xDim);
                //v->add_att(_FillValue, ""); // TODO: missing value for strings ????
                break;
            case odb::IGNORE:
            default:
                Log::error() << "Unknown column type: name=" << column.name() << ", type=" << column.type() << endl;
                ASSERT("Unknown type" && false);
                break;
        }
        vars.push_back(v);
    }
    return vars;
}

void Odb2NetCDF_1D::convert() {
    // Create the file. The Replace parameter tells netCDF to overwrite
    // this file, if it already exists.
    NcFile dataFile(outputfile().c_str(), NcFile::Replace);

    // You should always check whether a netCDF file creation or open
    // constructor succeeded.
    if (! dataFile.is_valid())
        throw UserError("Could not open file"); // TODO: find appropriate exception

    Log::info() << "Conversion to NetCDF 1D" << endl;

    //////////////////////////////           ODB from MARS             //////////////////////////////
    NcDim* xDim (dataFile.add_dim("hdrlen"));
    dataFile.add_att("Conventions", "CF-1.6"); 

    odb::Reader odb(inputfile());
    odb::Reader::iterator it (odb.begin());

    vector<NcVar*> vars (createVariables(dataFile, it->columns(), xDim));

    union { double n; char b[sizeof(double) + 1]; } buffer;
    memset(&buffer, 0, sizeof(buffer));

    size_t nrows (0);
    for(; it != odb.end(); ++it, ++nrows)
        for (int i(0); i < it->columns().size(); ++i)
        {
            buffer.n = ((*it)[i]);
            switch (it->columns()[i]->type()) 
            {
                case odb::STRING:
                    vars[i]->put_rec(buffer.b, nrows);
                    break;
                default:
                    vars[i]->put_rec(&buffer.n, nrows);
                    break;
            }
        }
    Log::info() << "Converted " << nrows << " row(s)." << endl;
}

//----------------------------------------------------------------
Odb2NetCDF_2D::Odb2NetCDF_2D(const string & inputfile, const string & outputfile)
  : Odb2NetCDF(inputfile,outputfile) {

  fileNameHdr_ = inputfile + "_hdr.odb";
  fileNameBody_ = inputfile  + "_body.odb";

  Log::info() << "The following files will be opened and read: " << endl;
  Log::info() << "Header filename = " <<  fileNameHdr_ << endl;
  Log::info() << "Body filename = " << fileNameBody_ << endl;
  Log::info() << "Output filename = " << outputfile << endl;
}

//----------------------------------------------------------------
void Odb2NetCDF_2D::convert() {
  // Create the file. The Replace parameter tells netCDF to overwrite
  // this file, if it already exists.
  NcFile dataFile(outputfile().c_str(), NcFile::Replace);

  // You should always check whether a netCDF file creation or open
  // constructor succeeded.
  if (! dataFile.is_valid())
      throw UserError ("Couldn't open file!");

    Log::info() << "Conversion to NetCDF 2D" << endl;
    // Check how many channel/bodylen
    string sql = "select distinct vertco_reference_1 from \"" + fileNameBody_ + "\" order by vertco_reference_1;";
    odb::Select odbs(sql);
    int nmaxchannel = 0;
    for (odb::Select::iterator its = odbs.begin();  its != odbs.end(); ++its, ++nmaxchannel);

    Log::info() << " There are  = " <<  nmaxchannel << " channels" << endl;

    // When we create netCDF dimensions, we get back a pointer to an
    // NcDim for each one.
    NcDim* xDim = dataFile.add_dim("hdrlen");
    NcDim* yDim = dataFile.add_dim("maxbodylen", nmaxchannel);

    NcVar *colChannel;

    int * channel = new int [nmaxchannel];
    int i=0;

    for (odb::Select::iterator its = odbs.begin();  its != odbs.end(); ++its, ++i)
      {
	if (i==0)
	  colChannel = dataFile.add_var(its->columns()[0]->name().c_str(), ncInt, yDim);
	channel[i] = ((*its)[0]);
      }

    colChannel->put(channel, nmaxchannel);

    //////////////////////////////           HDR             //////////////////////////////

    odb::Reader odb_hdr(fileNameHdr_);
    odb::Reader::iterator it_hdr = odb_hdr.begin();
    NcVar **colHdr;
    colHdr = new NcVar * [it_hdr->columns().size()];

    for (int i=0;i<it_hdr->columns().size();++i) {
      switch(it_hdr->columns()[i]->type())
	{
	case odb::INTEGER:
	case odb::BITFIELD:
	  colHdr[i] = dataFile.add_var(it_hdr->columns()[i]->name().c_str(), ncInt, xDim);
	  colHdr[i]->add_att(_FillValue,(int)it_hdr->columns()[i]->missingValue());
	  break;
	case odb::REAL:
	  colHdr[i] = dataFile.add_var(it_hdr->columns()[i]->name().c_str(), ncFloat, xDim);
	  colHdr[i]->add_att(_FillValue, (float)it_hdr->columns()[i]->missingValue());
	  break;
	case odb::STRING:
	  //        colHdr[i] = dataFile.add_var(it_hdr->columns()[i]->name().c_str(), ncInt, xDim);
	  //        colHdr[i]->add_att();
	  break;
	case odb::IGNORE:
	default:
	  ASSERT("Unknown type" && false);
	  break;
	}
    }

    int nrows=0;
    double nr;
    for(; it_hdr != odb_hdr.end(); ++it_hdr)
      {
	++nrows;
	for (int i=0;i<it_hdr->columns().size();++i) {
	  nr = ((*it_hdr)[i]);
	  colHdr[i]->put(&nr, 1);
	  colHdr[i]->set_cur(nrows);
	}
      }

    //////////////////////////////           BODY             //////////////////////////////


    odb::Reader odb_body(fileNameBody_);
    odb::Reader::iterator it_body = odb_body.begin();
    NcVar **colBody;
    colBody = new NcVar * [it_body->columns().size()];

    int index_channel=-1;
    int index_seqno=-1;
    for (int i=0;i<it_body->columns().size();++i) {
      if (it_body->columns()[i]->name() == "vertco_reference_1" || it_body->columns()[i]->name() == "vertco_reference_1@body")
	    index_channel = i;
      if (it_body->columns()[i]->name() == "seqno" || it_body->columns()[i]->name() == "seqno@hdr")
	    index_seqno = i;
    }

    Log::info() << " index_vertco_reference_1 = " << index_channel << endl;
    Log::info() << " index_seqno = " << index_seqno << endl;

    if (index_channel != -1 && index_seqno != -1) {
      for (int i=0;i<it_body->columns().size();++i) {
	if ((it_body->columns()[i]->name() != "seqno" && it_body->columns()[i]->name() != "vertco_reference_1") &&
        (it_body->columns()[i]->name() != "seqno@hdr" && it_body->columns()[i]->name() != "vertco_reference_1@body")) {
	  switch(it_body->columns()[i]->type())
	    {
	    case odb::INTEGER:
	    case odb::BITFIELD:
	      colBody[i] = dataFile.add_var(it_body->columns()[i]->name().c_str(), ncInt, xDim, yDim);
	      colBody[i]->add_att(_FillValue,(int)it_body->columns()[i]->missingValue());
	      break;
	    case odb::REAL:
	      colBody[i] = dataFile.add_var(it_body->columns()[i]->name().c_str(), ncFloat, xDim, yDim);
	      colBody[i]->add_att(_FillValue,(float) it_body->columns()[i]->missingValue());
	      break;
	    case odb::STRING:
	      //            colBody[i] = dataFile.add_var(it_body->columns()[i]->name().c_str(), ncInt, xDim, yDim);
	      break;
	    case odb::IGNORE:
	    default:
	      ASSERT("Unknown type" && false);
	      break;
	    }
	}
      }

      int nrows = 0;
      int nchannel = 0;
      double nd[it_body->columns().size()][nmaxchannel];
      double lnd[nmaxchannel];
      long icurrent_seqno=-1;
      int icurrent_channel=-1;
      for(; it_body != odb_body.end(); ++it_body)
	{
	  if (((*it_body)[index_seqno]) != icurrent_seqno) {
	    if (nrows > 0) {  // not the first time
	      for (int i=0;i<it_body->columns().size();++i) {
	if ((it_body->columns()[i]->name() != "seqno" && it_body->columns()[i]->name() != "vertco_reference_1") &&
        (it_body->columns()[i]->name() != "seqno@hdr" && it_body->columns()[i]->name() != "vertco_reference_1@body")) {
		  // copy to a local array (this specific ODB column) to write in netCDF
		  for (int j=0; j<nmaxchannel; ++j)
		    lnd[j] = nd[i][j];
		  colBody[i]->put_rec(&lnd[0], nrows-1);  // nrows -1 because we start to write at line 0 and not 1
		}
	      }
	    }
	    ++nrows;
	    icurrent_seqno = ((*it_body)[index_seqno]);
	    icurrent_channel =  0;
	    nchannel=0;
	    // initialize to missing value nd for all columns and all channel
	    for (int i=0;i<it_body->columns().size();++i) {
	      for (int j=0; j< nmaxchannel; ++j) {
		nd[i][j] = it_body->columns()[i]->missingValue();
	      }
	    }
	  }
	  while (icurrent_channel < nmaxchannel && channel[icurrent_channel] < ((*it_body)[index_channel])) {
	    ++icurrent_channel;
	    ++nchannel;
	  }

	  for (int i=0;i<it_body->columns().size();++i) {
	if ((it_body->columns()[i]->name() != "seqno" && it_body->columns()[i]->name() != "vertco_reference_1") &&
        (it_body->columns()[i]->name() != "seqno@hdr" && it_body->columns()[i]->name() != "vertco_reference_1@body")) {
	      nd[i][icurrent_channel] = ((*it_body)[i]);
	    }
	  }
	  ++nchannel;
	  ++icurrent_channel;
	}
      for (int i=0;i<it_body->columns().size();++i) {
	if ((it_body->columns()[i]->name() != "seqno" && it_body->columns()[i]->name() != "vertco_reference_1") &&
        (it_body->columns()[i]->name() != "seqno@hdr" && it_body->columns()[i]->name() != "vertco_reference_1@body")) {
	  for (int j=0; j<nmaxchannel; ++j)
	    lnd[j] = nd[i][j];
	  for (int j=0; j<nmaxchannel; ++j)
	    Log::info() << nrows << " lnd[" << j << "]=" << lnd[j] << " " ;
	  Log::info() << endl;
	  colBody[i]->put_rec(&lnd[0], nrows-1);
	}
      }

    }
}
