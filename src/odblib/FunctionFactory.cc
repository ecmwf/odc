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
#include <limits.h>

#include "eckit/exception/Exceptions.h"

#include "odblib/FunctionExpression.h"
#include "odblib/SQLSelect.h"
#include "odblib/piconst.h"

#include "FunctionAND.h"
#include "FunctionAVG.h"
#include "FunctionCOUNT.h"
#include "FunctionDOTP.h"
#include "FunctionEQ.h"
#include "FunctionEQ_BOXLAT.h"
#include "FunctionEQ_BOXLON.h"
#include "FunctionExpression.h"
#include "FunctionIN.h"
#include "FunctionIntegerExpression.h"
#include "FunctionJOIN.h"
#include "FunctionJULIAN.h"
#include "FunctionMAX.h"
#include "FunctionMIN.h"
#include "FunctionNORM.h"
#include "FunctionNOT_IN.h"
#include "FunctionNOT_NULL.h"
#include "FunctionNULL.h"
#include "FunctionNVL.h"
#include "FunctionOR.h"
#include "FunctionRGG_BOXLAT.h"
#include "FunctionRGG_BOXLON.h"
#include "FunctionRMS.h"
#include "FunctionROWNUMBER.h"
#include "FunctionSTDEV.h"
#include "FunctionSUM.h"
#include "FunctionTDIFF.h"
#include "FunctionTHIN.h"
#include "FunctionTIMESTAMP.h"
#include "FunctionVAR.h"
#include "FunctionFactory.h"
#include "FunctionIntegerExpression.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace expression {
namespace function {

//--------------------------------------------------------------

#define RMDI   -2147483647
#define NMDI    2147483647
const double R_Earth_km   = 180*60 / piconst::pi * 1.852;
const double R_Earth      = 180*60 / piconst::pi * 1.852*100.0;
const double EPS          = 1e-7;
const double D2R          = piconst::pi/180.0;
const double R2D          = 180.0/piconst::pi;

static ThreadSingleton<FunctionFactory> functionFactory_;

struct FFMap : public map<pair<string,int>, FunctionFactoryBase*> { static FFMap& instance(); };
static ThreadSingleton<FFMap> map_;
FFMap& FFMap::instance() { return map_.instance(); }

struct SQLFunctionHelp : public map<pair<string,int>, string> { static SQLFunctionHelp& instance(); };
static ThreadSingleton<SQLFunctionHelp> sqlFunctionsHelp_;
SQLFunctionHelp& SQLFunctionHelp::instance() { return sqlFunctionsHelp_.instance(); }

FunctionFactory& FunctionFactory::instance() { return functionFactory_.instance(); }

FunctionFactoryBase::FunctionFactoryBase(const string& name, int arity, const string& help)
: arity_(arity),
  name_(name),
  help_(help)
{
	pair<string,int> p(name_,arity_);
	//if(!map_) map_ = new map<pair<string,int>,FunctionFactoryBase*>();

	ASSERT(FFMap::instance().find(p) == FFMap::instance().end());
	FFMap::instance()[p] = this;
	SQLFunctionHelp::instance()[p] = help_;
}

FunctionFactory::FunctionInfo& FunctionFactory::functionsInfo()
{
	if (functionInfo_.size() == 0)
		for (map<pair<string,int>,FunctionFactoryBase*>::iterator i = FFMap::instance().begin(); i != FFMap::instance().end(); ++i)
			functionInfo_.push_back(make_pair(make_pair(i->first.first, i->first.second), SQLFunctionHelp::instance()[i->first]));
	return functionInfo_;
}

FunctionFactoryBase::~FunctionFactoryBase()
{

//	pair<string,int> p(name_,arity_);
//	mapa().erase(p);
//	if (mapa().empty())
//	{
//		delete map_;
//		map_ = 0;
//	}
}

FunctionExpression* FunctionFactoryBase::build(const string& name, const expression::Expressions& args)
{
	pair<string,int> p(name,args.size());	
	map<pair<string,int>,FunctionFactoryBase*>::iterator j = FFMap::instance().find(p);

	// Try -1
	if(j == FFMap::instance().end())
	{
		p = pair<string,int>(name,-1);
		j = FFMap::instance().find(p);
	}

	if(j == FFMap::instance().end())
		throw eckit::UserError(name + ": function not defined");

	return (*j).second->make(name,args);

}

FunctionExpression* FunctionFactoryBase::build(const string& name, SQLExpression* arg)
{
	expression::Expressions args;
	args.push_back(arg);
	return build(name,args);
}

FunctionExpression* FunctionFactoryBase::build(const string& name, SQLExpression* arg1, SQLExpression* arg2)
{
	expression::Expressions args;
	args.push_back(arg1);
	args.push_back(arg2);
	return build(name,args);
}

FunctionExpression* FunctionFactoryBase::build(const string& name, SQLExpression* arg1, SQLExpression* arg2, SQLExpression *arg3)
{
	expression::Expressions args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	return build(name,args);
}

//===============================

#include <math.h>

template<double (*T)(double)> 
class MathFunctionExpression_1 : public FunctionExpression {
	double eval(bool& m) const { double v = args_[0]->eval(m); return m ? this->missingValue_ : T(v); }
	SQLExpression* clone() const { return new MathFunctionExpression_1<T>(*this); }
public:
	MathFunctionExpression_1(const string& name, const expression::Expressions& args) : FunctionExpression(name, args) {}
	MathFunctionExpression_1(const MathFunctionExpression_1& o) : FunctionExpression(o) {}
};

template<double (*T)(double, double)> 
class MathFunctionExpression_2 : public FunctionExpression {
	double eval(bool& m) const {
		double left = args_[0]->eval(m);
		if (m) return this->missingValue_;
		double right = args_[1]->eval(m);
		if (m) return this->missingValue_;
		return T(left, right);
	}
	SQLExpression* clone() const { return new MathFunctionExpression_2<T>(*this); }
public:
	MathFunctionExpression_2(const string& name, const expression::Expressions& args) : FunctionExpression(name,args) {}
	MathFunctionExpression_2(const MathFunctionExpression_2& o) : FunctionExpression(o) {}
};

template<double (*T)(double,double,double)> 
class MathFunctionExpression_3 : public FunctionExpression {
	double eval(bool& m) const {
		double a0 = args_[0]->eval(m);
		if (m) return this->missingValue_;
		double a1 = args_[1]->eval(m);
		if (m) return this->missingValue_;
		double a2 = args_[2]->eval(m);
		if (m) return this->missingValue_;
		return T(a0, a1, a2);
	}
	SQLExpression* clone() const { return new MathFunctionExpression_3<T>(*this); }
public:
	MathFunctionExpression_3(const string& name,const expression::Expressions& args) : FunctionExpression(name,args) {}
	MathFunctionExpression_3(const MathFunctionExpression_3& o) : FunctionExpression(o) {}
};

template<double (*T)(double,double,double,double)> 
class MathFunctionExpression_4 : public FunctionExpression {
	double eval(bool& m) const {
		double a0 = args_[0]->eval(m);
		if (m) return this->missingValue_;
		double a1 = args_[1]->eval(m);
		if (m) return this->missingValue_;
		double a2 = args_[2]->eval(m);
		if (m) return this->missingValue_;
		double a3 = args_[3]->eval(m);
		if (m) return this->missingValue_;
		return T(a0, a1, a2, a3);
	}
	SQLExpression* clone() const { return new MathFunctionExpression_4<T>(*this); }
public:
	MathFunctionExpression_4(const string& name,const expression::Expressions& args) : FunctionExpression(name,args) {}
	MathFunctionExpression_4(const MathFunctionExpression_4& o) : FunctionExpression(o) {}
};

template<double (*T)(double,double,double,double,double)> 
class MathFunctionExpression_5 : public FunctionExpression {
	double eval(bool& m) const {
		double a0 = args_[0]->eval(m);
		if (m) return this->missingValue_;
		double a1 = args_[1]->eval(m);
		if (m) return this->missingValue_;
		double a2 = args_[2]->eval(m);
		if (m) return this->missingValue_;
		double a3 = args_[3]->eval(m);
		if (m) return this->missingValue_;
		double a4 = args_[4]->eval(m);
		if (m) return this->missingValue_;
		return T(a0, a1, a2, a3, a4);
	}
	SQLExpression* clone() const { return new MathFunctionExpression_5<T>(*this); }
public:
	MathFunctionExpression_5(const string& name,const expression::Expressions& args) : FunctionExpression(name,args) {}
	MathFunctionExpression_5(const MathFunctionExpression_5& o) : FunctionExpression(o) {}
};

#define DEFINE_MATH_FUNC_1(F,Help) \
static FunctionMaker<MathFunctionExpression_1<F> > make_1_##F(#F,1,Help)

#define DEFINE_MATH_FUNC_1F(FuncName, Name, Help) \
static FunctionMaker<MathFunctionExpression_1<FuncName> > make_1_##FuncName(#Name,1,Help)

#define DEFINE_MATH_FUNC_2(F,Help) \
static FunctionMaker<MathFunctionExpression_2<F> > make_2_##F(#F,2,Help)

#define DEFINE_MATH_FUNC_2F(FuncName, Name, Help) \
static FunctionMaker<MathFunctionExpression_2<FuncName> > make_2_##FuncName(#Name,2,Help)

#define DEFINE_MATH_FUNC_3(F,Help) \
static FunctionMaker<MathFunctionExpression_3<F> > make_3_##F(#F,3,Help)

#define DEFINE_MATH_FUNC_4(F,Help) \
static FunctionMaker<MathFunctionExpression_4<F> > make_4_##F(#F,4,Help)

#define DEFINE_MATH_FUNC_5(F,Help) \
static FunctionMaker<MathFunctionExpression_5<F> > make_5_##F(#F,5,Help)

//--------------------------------------------------------------

#define DEFINE_UNARY(N,T,Help)  static FunctionMaker<MathFunctionExpression_1<T> > make_##T(#N,1,Help)
#define DEFINE_BINARY(N,T,Help) static FunctionMaker<MathFunctionExpression_2<T> > make_##T(#N,2,Help)

inline double abs(double x) { return fabs(x); }
// Note: ODB's trigonometric funcs require args in degrees 
// and return degrees (where applicable)
inline double Func_acos(double x) { return (R2D*acos(x)); }
inline double Func_asin(double x) { return (R2D*asin(x)); }
inline double Func_atan(double x) { return (R2D*atan(x)); }
inline double Func_atan2(double x, double y) { return (R2D*atan2(x,y)); }
inline double Func_cos(double x) { return (cos(D2R*x)); }
inline double Func_sin(double x) { return (sin(D2R*x)); }
inline double Func_tan(double x) { return (tan(D2R*x)); }
inline double mod(double x, double y) { return fmod(x,y); }
inline double Func_pow(double x, double y) { return ((y) == 2 ? (x)*(x) : pow(x,y)); }
inline double ln(double x) { return log(x); }
inline double lg(double x) { return log10(x); }

const double ZERO_POINT=((double)273.15e0);

inline double celsius(double x) { return x - ZERO_POINT; }
inline double k2c(double x) { return x - ZERO_POINT; }
inline double kelvin(double x) { return x + ZERO_POINT; }
inline double c2k(double x) { return x + ZERO_POINT; }
inline double c2f(double x) { return ((9*x)/5) + 32; }
inline double f2c(double x) { return ((x - 32)*5)/9; }
inline double f2k(double x) { return c2k(f2c(x)); }
inline double k2f(double x) { return c2f(k2c(x)); }
inline double fahrenheit(double x) { return c2f(k2c(x)); }


inline double radians(double x) { return x * D2R; }
inline double deg2rad(double x) { return x * D2R; }
inline double degrees(double x) { return x * R2D; }
inline double rad2deg(double x) { return x * R2D; }

inline double speed(double u, double v) { return sqrt(u*u + v*v); }
inline double ff(double u, double v) { return speed(u,v); }

inline double direction(double u, double v) { return fmod(Func_atan2(-u,-v)+360.,360.); }
inline double dir(double u, double v) { return direction(u,v); }
inline double dd(double u, double v) { return direction(u,v); }

/// Distance in meters.
inline double distance(double lat1,double lon1,double lat2,double lon2) 
{ return R_Earth*acos(Func_sin(lat1)*Func_sin(lat2)+Func_cos(lat1)*Func_cos(lat2)*Func_cos(lon1-lon2)); }


inline double km(double x)
{ return R_Earth_km*x; }

/// in kilometers
inline double km(double lat1,double lon1,double lat2,double lon2) 
{ return R_Earth_km*acos(Func_sin(lat1)*Func_sin(lat2)+Func_cos(lat1)*Func_cos(lat2)*Func_cos(lon1-lon2)); }

/// in kilometers
inline double dist(double reflat, double reflon, double refdist_km, double obslat, double obslon) 
{
	return (double)( R_Earth_km *
           acos(Func_cos(reflat) * Func_cos(obslat) * Func_cos(obslon-reflon) +
           Func_sin(reflat) * Func_sin(obslat)) <= (refdist_km) );
}

inline double circle(double x, double x0, double y, double y0, double r)
{ return ( Func_pow(x-x0,2) + Func_pow(y-y0,2) <= Func_pow(r,2) ); }



inline double rad(double reflat, double reflon, double refdeg, double obslat, double obslon)
{
  return (double)(acos(Func_cos(reflat) * Func_cos(obslat) * Func_cos(obslon-reflon) +
               Func_sin(reflat) * Func_sin(obslat) ) <= D2R*refdeg);
}


//--------------------------------------------------------------
inline double between(double x,double a,double b) { return x >= a && x <= b; }
inline double not_between(double x,double a,double b) { return x < a || x > b; }
inline double between_exclude_first(double x,double a,double b) { return x > a && x <= b; }
inline double between_exclude_second(double x,double a,double b) { return x >= a && x < b; }
inline double between_exclude_both(double x,double a,double b) { return x > a && x < b; }
inline double twice(double x) { return 2*x; }


/* No. of bits for "int" */
#define MAXBITS 32

#define MASK_0           0U  /*                                 0 */
#define MASK_1           1U  /*                                 1 */
#define MASK_2           3U  /*                                11 */
#define MASK_3           7U  /*                               111 */
#define MASK_4          15U  /*                              1111 */
#define MASK_5          31U  /*                             11111 */
#define MASK_6          63U  /*                            111111 */
#define MASK_7         127U  /*                           1111111 */
#define MASK_8         255U  /*                          11111111 */
#define MASK_9         511U  /*                         111111111 */
#define MASK_10       1023U  /*                        1111111111 */
#define MASK_11       2047U  /*                       11111111111 */
#define MASK_12       4095U  /*                      111111111111 */
#define MASK_13       8191U  /*                     1111111111111 */
#define MASK_14      16383U  /*                    11111111111111 */
#define MASK_15      32767U  /*                   111111111111111 */
#define MASK_16      65535U  /*                  1111111111111111 */
#define MASK_17     131071U  /*                 11111111111111111 */
#define MASK_18     262143U  /*                111111111111111111 */
#define MASK_19     524287U  /*               1111111111111111111 */
#define MASK_20    1048575U  /*              11111111111111111111 */
#define MASK_21    2097151U  /*             111111111111111111111 */
#define MASK_22    4194303U  /*            1111111111111111111111 */
#define MASK_23    8388607U  /*           11111111111111111111111 */
#define MASK_24   16777215U  /*          111111111111111111111111 */
#define MASK_25   33554431U  /*         1111111111111111111111111 */
#define MASK_26   67108863U  /*        11111111111111111111111111 */
#define MASK_27  134217727U  /*       111111111111111111111111111 */
#define MASK_28  268435455U  /*      1111111111111111111111111111 */
#define MASK_29  536870911U  /*     11111111111111111111111111111 */
#define MASK_30 1073741823U  /*    111111111111111111111111111111 */
#define MASK_31 2147483647U  /*   1111111111111111111111111111111 */
#define MASK_32 4294967295U  /*  11111111111111111111111111111111 */

#define MASK(n) MASK_##n

#define IOR(x,y)   ((x) | (y))
#define IAND(x,y)  ((x) & (y))
#define ISHFTL(x,n) ((x) << (n))
#define ISHFTR(x,n) ((x) >> (n))

#define GET_BITS(x, pos, len)      IAND(ISHFTR((int)(x), pos), MASK(len))
#define CASE_GET_BITS(x, pos, len) \
  case len: rc = GET_BITS(x, pos, len); break


double ibits(double X, double Pos, double Len)
{
  int rc = 0; /* the default */
  X = trunc(X);
  Pos = trunc(Pos);
  Len = trunc(Len);
  if (X   >= INT_MIN && X   <= INT_MAX &&
      Pos >= 0       && Pos <  MAXBITS &&
      Len >= 1       && Len <= MAXBITS) {
    int x = X;
    int pos = Pos;
    int len = Len;
    switch (len) {
      CASE_GET_BITS(x, pos, 1);
      CASE_GET_BITS(x, pos, 2);
      CASE_GET_BITS(x, pos, 3);
      CASE_GET_BITS(x, pos, 4);
      CASE_GET_BITS(x, pos, 5);
      CASE_GET_BITS(x, pos, 6);
      CASE_GET_BITS(x, pos, 7);
      CASE_GET_BITS(x, pos, 8);
      CASE_GET_BITS(x, pos, 9);
      CASE_GET_BITS(x, pos,10);
      CASE_GET_BITS(x, pos,11);
      CASE_GET_BITS(x, pos,12);
      CASE_GET_BITS(x, pos,13);
      CASE_GET_BITS(x, pos,14);
      CASE_GET_BITS(x, pos,15);
      CASE_GET_BITS(x, pos,16);
      CASE_GET_BITS(x, pos,17);
      CASE_GET_BITS(x, pos,18);
      CASE_GET_BITS(x, pos,19);
      CASE_GET_BITS(x, pos,20);
      CASE_GET_BITS(x, pos,21);
      CASE_GET_BITS(x, pos,22);
      CASE_GET_BITS(x, pos,23);
      CASE_GET_BITS(x, pos,24);
      CASE_GET_BITS(x, pos,25);
      CASE_GET_BITS(x, pos,26);
      CASE_GET_BITS(x, pos,27);
      CASE_GET_BITS(x, pos,28);
      CASE_GET_BITS(x, pos,29);
      CASE_GET_BITS(x, pos,30);
      CASE_GET_BITS(x, pos,31);
      CASE_GET_BITS(x, pos,32);
    }

  }
  return (double) rc;
}

double negate_double(double n) { return -n; }
double logical_not_double(double n) { return !n; }
double not_equal_to_double(double l, double r) { return l != r; }
double greater_double(double l, double r) { return l > r; }
double greater_equal_double(double l, double r) { return l >= r; }
double less_double(double l, double r) { return l < r; }
double less_equal_double(double l, double r) { return l <= r; }

double plus_double(double l, double r) { return l + r; }
double minus_double(double l, double r) { return l - r; }

class MultiplyExpression : public FunctionExpression {
	double eval(bool& missing) const
	{
		bool leftMissing = false;
		bool rightMissing = false;
		double left = args_[0]->eval(leftMissing);
		double right = args_[1]->eval(rightMissing);

		// Special case: 0 * anything = 0
		if ( (left == 0 || right == 0) && ! (leftMissing && rightMissing))
		{
			missing = false;
			return 0;
		}
		return (missing = leftMissing | rightMissing)
				? this->missingValue_ 
				: left * right;
	}
	SQLExpression* clone() const { return new MultiplyExpression(name_, 2); }
public:
	MultiplyExpression(const string& name, const expression::Expressions& args)
	: FunctionExpression(name,args) {}
};

double multiplies_double(double l, double r) { return l * r; }

double divides_double(double l, double r) { return l / r; }
double ldexp_double(double l, double r) { return ldexp(l, r); }

FunctionFactory::FunctionFactory() : FunctionFactoryBase("FunctionFactory", -1, "This is not an SQL function")
{
	DEFINE_BINARY(<>,not_equal_to_double,"not equal to");
	DEFINE_BINARY(>,greater_double,"greater");
	DEFINE_BINARY(<,less_double,"less");
	DEFINE_BINARY(>=,greater_equal_double,"greater or equal");
	DEFINE_BINARY(<=,less_equal_double,"less or equal");

	DEFINE_BINARY(+,plus_double,"add");
	DEFINE_BINARY(-,minus_double,"subtract");
	//DEFINE_BINARY(*,multiplies_double);
	static FunctionMaker<MultiplyExpression> make_MultiplyExpression("*",2,"multiply");
	DEFINE_BINARY(/,divides_double,"divide");

	DEFINE_UNARY(-,negate_double,"negate");
	DEFINE_UNARY(not,logical_not_double,"logical not");

	DEFINE_MATH_FUNC_1(abs,"absolute value");
	DEFINE_MATH_FUNC_1(fabs,"absolute value");

	DEFINE_MATH_FUNC_1F(Func_acos, acos, "arc cosine");
	DEFINE_MATH_FUNC_1F(Func_asin, asin, "arc sine");
	DEFINE_MATH_FUNC_1F(Func_atan, atan, "arc tangent of one variable");
	DEFINE_MATH_FUNC_2F(Func_atan2, atan2, "arc tangent of param1/param2 (y/x)");
	DEFINE_MATH_FUNC_1F(Func_cos, cos, "cosine");
	DEFINE_MATH_FUNC_1F(Func_sin, sin, "size");
	DEFINE_MATH_FUNC_1F(Func_tan, tan, "tangent");

	DEFINE_MATH_FUNC_1(exp, "base-e exponential function, e raised to x");
	DEFINE_MATH_FUNC_1(cosh, "hyperbolic cosine");
	DEFINE_MATH_FUNC_1(sinh, "hyperbolic sine");
	DEFINE_MATH_FUNC_1(tanh, "hyperbolic tangent");
	DEFINE_MATH_FUNC_1(log, "natural logarithm");
	DEFINE_MATH_FUNC_1(log10, "base-10 logarithm");
	DEFINE_MATH_FUNC_1(sqrt, "square root");

	DEFINE_MATH_FUNC_2(ldexp_double, "");

	DEFINE_MATH_FUNC_2(mod, "");
	DEFINE_MATH_FUNC_2(fmod, "");
	DEFINE_MATH_FUNC_2(ff, "");
	DEFINE_MATH_FUNC_2(speed, "");
	DEFINE_MATH_FUNC_2(dd, "");
	DEFINE_MATH_FUNC_2(dir, "");

	DEFINE_MATH_FUNC_2F(Func_pow, pow, "");

	DEFINE_MATH_FUNC_1(ln, "");
	DEFINE_MATH_FUNC_1(lg, "");


	DEFINE_MATH_FUNC_1(celsius, "");
	DEFINE_MATH_FUNC_1(k2c, "");
	DEFINE_MATH_FUNC_1(kelvin, "");
	DEFINE_MATH_FUNC_1(c2k, "");
	DEFINE_MATH_FUNC_1(c2f, "");
	DEFINE_MATH_FUNC_1(f2c, "");
	DEFINE_MATH_FUNC_1(f2k, "");
	DEFINE_MATH_FUNC_1(k2f, "");
	DEFINE_MATH_FUNC_1(fahrenheit, "");

	DEFINE_MATH_FUNC_4(distance, "");
	DEFINE_MATH_FUNC_5(dist, "");

	DEFINE_MATH_FUNC_1(km, "");
	DEFINE_MATH_FUNC_4(km, "");

	DEFINE_MATH_FUNC_1(radians, "");
	DEFINE_MATH_FUNC_1(deg2rad, "");
	DEFINE_MATH_FUNC_1(degrees, "");
	DEFINE_MATH_FUNC_1(rad2deg, "");

	DEFINE_MATH_FUNC_1(twice, "");

	DEFINE_MATH_FUNC_3(between, "");
	DEFINE_MATH_FUNC_3(not_between, "");
	DEFINE_MATH_FUNC_3(between_exclude_first, "");
	DEFINE_MATH_FUNC_3(between_exclude_second, "");
	DEFINE_MATH_FUNC_3(between_exclude_both, "");

	DEFINE_MATH_FUNC_3(ibits, "");

	DEFINE_MATH_FUNC_5(circle, "");
	DEFINE_MATH_FUNC_5(rad, "");

	static FunctionMaker<FunctionAND> make_AND("and",2, "");
	static FunctionMaker<FunctionAVG> make_AVG("avg",1, "");
	static FunctionMaker<FunctionCOUNT> make_COUNT("count",1, "");
	static FunctionMaker<FunctionDOTP> make_DOTP("dotp",2, "");
	static FunctionMaker<FunctionEQ> make_EQ("=",2, "");
	static FunctionMaker<FunctionEQ_BOXLAT> make_EQ_BOXLAT("eq_boxlat", 3, "");
	static FunctionMaker<FunctionEQ_BOXLON> make_EQ_BOXLON("eq_boxlon",3, "");
	//#define DEFINE_UNARY(N,T)  static FunctionMaker<MathFunctionExpression_1<T<double> > > make_##T(#N,1)
	//#define DEFINE_BINARY(N,T) static FunctionMaker<MathFunctionExpression_2<T<double> > > make_##T(#N,2)
	static FunctionMaker<FunctionIN> make_IN("in", -1, "");
	static FunctionMaker<FunctionJOIN> make_JOIN("join",2, "");
	static FunctionMaker<FunctionJULIAN> make_JULIAN("julian",2, "");
	static FunctionMaker<FunctionJULIAN> make_JD("jd",2, "");
	static FunctionMaker<FunctionJULIAN> make_JULIAN_DATE("julian_date",2, "");
	static FunctionMaker<FunctionMAX> make_MAX("max",1, "");
	static FunctionMaker<FunctionMIN> make_MIN("min",1, "");
	static FunctionMaker<FunctionNORM> make_NORM("norm",2, "");
	static FunctionMaker<FunctionNOT_IN> make_NOT_IN("not_in", -1, "");
	static FunctionMaker<FunctionNOT_NULL> make_NOT_NULL("not_null",1, "");
	static FunctionMaker<FunctionNULL> make_NULL("null",1, "");
	static FunctionMaker<FunctionNULL> make_ISNULL("isnull",1, "");
	static FunctionMaker<FunctionNVL> make_NVL("nvl",2, "");
	static FunctionMaker<FunctionOR> make_OR("or",2, "");
	static FunctionMaker<FunctionRGG_BOXLAT> make_RGG_BOXLAT("rgg_boxlat", 3, "");
	static FunctionMaker<FunctionRGG_BOXLON> make_RGG_BOXLON("rgg_boxlon",3, "");
	static FunctionMaker<FunctionRMS> make_RMS("rms",1, "");
	static FunctionMaker<FunctionROWNUMBER> make_ROWNUMBER("rownumber", 0, "");
	static FunctionMaker<FunctionSTDEV> make_STDEV("stdev",1, "");
	static FunctionMaker<FunctionSTDEV> make_STDEVP("stdevp",1, "");
	static FunctionMaker<FunctionSUM> make_SUM("sum",1, "");
	static FunctionMaker<FunctionTDIFF> make_TDIFF("tdiff",4, "");
	static FunctionMaker<FunctionTHIN> make_THIN("thin", 2, "");
	static FunctionMaker<FunctionTIMESTAMP> make_TIMESTAMP("timestamp",2, "");
	static FunctionMaker<FunctionVAR> make_VAR("var",1, "");
	static FunctionMaker<FunctionVAR> make_VARP("varp",1, "");

	FunctionIntegerExpression::registerIntegerFunctions();
}


} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

