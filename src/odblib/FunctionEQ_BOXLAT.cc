#include "FunctionEQ_BOXLAT.h"
#include "EqRegionCache.h"


namespace odb {
namespace sql {
namespace expression {
namespace function {


FunctionEQ_BOXLAT::FunctionEQ_BOXLAT(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionEQ_BOXLAT::FunctionEQ_BOXLAT(const FunctionEQ_BOXLAT& other)
: FunctionExpression(other.name_, other.args_)
{}

FunctionEQ_BOXLAT::~FunctionEQ_BOXLAT() {}

SQLExpression* FunctionEQ_BOXLAT::clone() const { return new FunctionEQ_BOXLAT(*this); }

double FunctionEQ_BOXLAT::eval(bool& missing) const
{
    double lat_degrees = args_[0]->eval(missing);
    double lon_degrees = args_[1]->eval(missing);
    lon_degrees = lon_degrees; // variable not used
    double resol = args_[2]->eval(missing);
    double res=0.;
    EqRegionCache p;
    res = p.get_midlat(resol, lat_degrees);
    return res;
}

static FunctionMaker<FunctionEQ_BOXLAT> make_EQ_BOXLAT("eq_boxlat", 3);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
