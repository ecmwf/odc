#include "FunctionJULIAN.h"
#include "DateTime.h"

#define RMDI   -2147483647
#define trunc(x) ((x) -fmod((x), 1))

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionJULIAN::FunctionJULIAN(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args)
{}

FunctionJULIAN::FunctionJULIAN(const FunctionJULIAN& other)
: FunctionExpression(other.name_, other.args_)
{}


SQLExpression* FunctionJULIAN::clone() const { return new FunctionJULIAN(*this); }


FunctionJULIAN::~FunctionJULIAN() {}

double FunctionJULIAN::eval(bool& missing) const
{
    int indate = (int) args_[0]->eval(missing);
    int intime = (int) args_[1]->eval(missing);
    int year_target = indate/10000;
    int month_target = (indate%10000)/100;
    int day_target = indate%100;
    int hour_target = intime/10000;
    int min_target = (intime%10000)/100;
    int sec_target = intime%100;

    utils::DateTime d1(year_target, month_target, day_target,
                   hour_target, min_target, sec_target);

    return d1.dateToJulian();
}

static FunctionMaker<FunctionJULIAN> make_JULIAN("julian",2);
static FunctionMaker<FunctionJULIAN> make_JD("jd",2);
static FunctionMaker<FunctionJULIAN> make_JULIAN_DATE("julian_date",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

