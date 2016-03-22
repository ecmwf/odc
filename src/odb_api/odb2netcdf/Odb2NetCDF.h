#ifndef ODB2NETCDF_H
#define ODB2NETCDF_H

#include <string>
#include <vector>
#include <netcdfcpp.h>

/// @author Anne Fouilloux

namespace odb { class MetaData; }

class Odb2NetCDF {
public:
    Odb2NetCDF(const std::string & inputfile, const std::string & outputfile);
    virtual ~Odb2NetCDF();
    virtual void convert()=0;
    std::string & inputfile() { return inputfile_;};
    std::string & outputfile() { return outputfile_;};
private:
    std::string inputfile_;
    std::string outputfile_;
};

class Odb2NetCDF_1D : public Odb2NetCDF {
public:
    Odb2NetCDF_1D(const std::string & inputfile, const std::string & outputfile);
    ~Odb2NetCDF_1D() {};
    virtual void convert();
private:
    std::vector<NcVar*> createVariables(NcFile& dataFile, const odb::MetaData& columns, NcDim*);
};

class Odb2NetCDF_2D : public Odb2NetCDF {
public:
    Odb2NetCDF_2D(const std::string & inputfile, const std::string & outputfile);
    ~Odb2NetCDF_2D() {};
    virtual void convert();
private:
    std::string fileNameHdr_;
    std::string fileNameBody_;
};

#endif
