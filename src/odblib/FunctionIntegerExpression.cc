/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <math.h>

#include "odblib/FunctionFactory.h"
#include "odblib/FunctionIntegerExpression.h"

#define RMDI   -2147483647
#define NMDI    2147483647
#define ftrunc(x) ((x) -fmod((x), 1))
#define F90nint(x) ( ((x) > 0) ? (int)((x) + 0.5) : (int)((x) - 0.5) )

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionIntegerExpression::FunctionIntegerExpression(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args)
{}

FunctionIntegerExpression::~FunctionIntegerExpression() {}

const odb::sql::type::SQLType* FunctionIntegerExpression::type() const
{
	return &odb::sql::type::SQLType::lookup("integer");
}

void FunctionIntegerExpression::output(ostream& s) const
{
	bool missing;
    double v = eval(missing);

	//Log::info() << "FunctionIntegerExpression::output: v=" << v << ", missing=" << missing << endl;

    s << static_cast<long long int>(v);
}

//===============================

#include <math.h>

template<double (*T)(double)> 
class MathFunctionIntegerExpression_1 : public FunctionIntegerExpression {
	double eval(bool& missing) const { return T(args_[0]->eval(missing)); }
	SQLExpression* clone() const { return new MathFunctionIntegerExpression_1<T>(this->name_,this->args_); }
public:
	MathFunctionIntegerExpression_1(const string& name,const expression::Expressions& args)
// TODO: FIXME: simillarly we need to propagate missing values to function expressions from their arguments everywhere
	: FunctionIntegerExpression(name, args) { this->missingValue_ = args_[0]->missingValue(); }
};

/*

template<class T> 
class MathFunctionIntegerExpression_2 : public FunctionIntegerExpression {
	double eval(bool& missing) const { return T()(args_[0]->eval(missing),args_[1]->eval(missing)); }
	SQLExpression* clone() const { return new MathFunctionIntegerExpression_2<T>(name_,args_); }
public:
	MathFunctionIntegerExpression_2(const string& name,const expression::Expressions& args)
	: FunctionIntegerExpression(name,args) {}
};

template<class T> 
class MathFunctionIntegerExpression_3 : public FunctionIntegerExpression {
	double eval(bool& missing) const { return T()(args_[0]->eval(missing),args_[1]->eval(missing),args_[2]->eval(missing)); }
	SQLExpression* clone() const { return new MathFunctionIntegerExpression_3<T>(name_,args_); }
public:
	MathFunctionIntegerExpression_3(const string& name,const expression::Expressions& args)
	: FunctionIntegerExpression(name,args) {}
};

template<class T> 
class MathFunctionIntegerExpression_4 : public FunctionIntegerExpression {
	double eval(bool& missing) const { return T()(args_[0]->eval(missing),args_[1]->eval(missing),args_[2]->eval(missing),args_[3]->eval(missing)); }
	SQLExpression* clone() const { return new MathFunctionIntegerExpression_4<T>(name_,args_); }
public:
	MathFunctionIntegerExpression_4(const string& name,const expression::Expressions& args)
	: FunctionIntegerExpression(name,args) {}
};

template<class T> 
class MathFunctionIntegerExpression_5 : public FunctionIntegerExpression {
	double eval(bool& missing) const { return T()(args_[0]->eval(missing),args_[1]->eval(missing),args_[2]->eval(missing),args_[3]->eval(missing),args_[4]->eval(missing)); }
	SQLExpression* clone() const { return new MathFunctionIntegerExpression_5<T>(name_,args_); }
public:
	MathFunctionIntegerExpression_5(const string& name,const expression::Expressions& args)
	: FunctionIntegerExpression(name,args) {}
};
*/

#define DEFINE_MATH_INT_FUNC_1F(FuncName, Name) \
/*struct math_1_##FuncName { double operator()(double val) const { return FuncName(val); } }; */ \
static FunctionMaker<MathFunctionIntegerExpression_1<FuncName> > make_1_##FuncName(#Name,1)

//#define DEFINE_MATH_INT_FUNC_2F(FuncName, Name) \
//struct math_2_##FuncName { double operator()(double v1,double v2) const { return FuncName(v1,v2); } }; \
//static FunctionMaker<MathFunctionIntegerExpression_2<math_2_##FuncName> > make_2_##FuncName(#Name,2)

//#define DEFINE_MATH_INT_FUNC_3F(FuncName, Name) \
//struct math_3_##FuncName { double operator()(double v1,double v2,double v3) const { return FuncName(v1,v2,v3); } }; \
//static FunctionMaker<MathFunctionIntegerExpression_3<math_3_##FuncName> > make_3_##FuncName(#Name,3)

//#define DEFINE_MATH_INT_FUNC_4F(FuncName, Name) \
//struct math_4_##FuncName { double operator()(double v1,double v2,double v3,double v4) const { return FuncName(v1,v2,v3,v4); } }; \
//static FunctionMaker<MathFunctionIntegerExpression_4<math_4_##FuncName> > make_4_##FuncName(#Name,4)

//#define DEFINE_MATH_INT_FUNC_5(FuncName, Name) \
//struct math_5_##FuncName { double operator()(double v1,double v2,double v3,double v4,double v5) const { return FuncName(v1,v2,v3,v4,v5); } }; \
//static FunctionMaker<MathFunctionIntegerExpression_5<math_5_##FuncName> > make_5_##FuncName(#Name,5)


//--------------------------------------------------------------
inline double year(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)((int)((x)/10000)) : (double)NMDI);}
inline double month(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(((int)((x)/100))%100) :
(double)NMDI);}
inline double day(double x)   { return ((fabs(x) != fabs((double) RMDI)) ? (double)(((int)(x))%100) : (double)NMDI);}
inline double hour(double x)   {return ((fabs(x) != fabs((double) RMDI)) ? (double)((int)((x)/10000)) : (double)NMDI);}
inline double minute(double x) {return ((fabs(x) != fabs((double) RMDI)) ? (double)(((int)((x)/100))%100) :
(double)NMDI);}
inline double minutes(double x) {return minute(x);}
inline double second(double x) {return ((fabs(x) != fabs((double)RMDI)) ? (double)(((int)(x))%100) : (double)NMDI);}
inline double seconds(double x) {return second(x);}


inline double Func_ftrunc(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(ftrunc(x)) : (double) NMDI); }
inline double Func_dnint(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(F90nint(x)) : (double)NMDI); }
inline double Func_dint(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(ftrunc(x)) : (double) NMDI);}
inline double Func_ceil(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(ceil(x)) : (double) NMDI);}
inline double Func_floor(double x) { return ((fabs(x) != fabs((double) RMDI)) ? (double)(floor(x)) : (double) NMDI);}

void FunctionIntegerExpression::registerIntegerFunctions()
{

	DEFINE_MATH_INT_FUNC_1F(year,year);
	DEFINE_MATH_INT_FUNC_1F(month,month);
	DEFINE_MATH_INT_FUNC_1F(day,day);
	DEFINE_MATH_INT_FUNC_1F(hour,hour);
	DEFINE_MATH_INT_FUNC_1F(minute,minute);
	DEFINE_MATH_INT_FUNC_1F(minutes,minutes);
	DEFINE_MATH_INT_FUNC_1F(second,second);
	DEFINE_MATH_INT_FUNC_1F(seconds, seconds);
	DEFINE_MATH_INT_FUNC_1F(Func_ceil,ceil);
	DEFINE_MATH_INT_FUNC_1F(Func_floor,floor);
	DEFINE_MATH_INT_FUNC_1F(Func_ftrunc,trunc);
	DEFINE_MATH_INT_FUNC_1F(Func_dint,int);
	DEFINE_MATH_INT_FUNC_1F(Func_dnint,nint);
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

