#include "FunctionRGG_BOXLAT.h"
#include "RggRegionCache.h"


namespace odb {
namespace sql {
namespace expression {
namespace function {


FunctionRGG_BOXLAT::FunctionRGG_BOXLAT(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionRGG_BOXLAT::FunctionRGG_BOXLAT(const FunctionRGG_BOXLAT& other)
: FunctionExpression(other.name_, other.args_)
{}

SQLExpression* FunctionRGG_BOXLAT::clone() const { return new FunctionRGG_BOXLAT(*this); }

FunctionRGG_BOXLAT::~FunctionRGG_BOXLAT() {}

double FunctionRGG_BOXLAT::eval(bool& missing) const
{
    double lat_degrees = args_[0]->eval(missing);
    double lon_degrees = args_[1]->eval(missing);
    lon_degrees = lon_degrees; // not used variable
    double resol = args_[2]->eval(missing);
    double res=0.;
    RggRegionCache p;
    res = p.get_midlat(resol, lat_degrees);
    return res;
}


static FunctionMaker<FunctionRGG_BOXLAT> make_RGG_BOXLAT("rgg_boxlat", 3);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
