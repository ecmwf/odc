/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include <string>
//#include <istream>
//#include <sstream>
//#include <iomanip>
//#include <stdexcept>
//#include <iostream>
//#include <stdint.h>
//#include <limits>

//#include "eckit/exception/Exceptions.h"

//#include "odblib/DateTime.h"
//#include "odblib/Duration.h"


using std::string;
using std::istringstream;
using std::ostringstream;
using std::invalid_argument;
using std::ostream;
using std::istream;
using std::setfill;
using std::setw;
using std::numeric_limits;
using utils::Duration;

namespace utils {

// -----------------------------------------------------------------------------

DateTime::DateTime() 
: year_(0), 
  month_(0), 
  day_(0),
  hour_(0), 
  minute_(0), 
  second_(0) 
{
}

// -----------------------------------------------------------------------------

DateTime::DateTime(const std::string & str) {
  this->set(str);
}

// -----------------------------------------------------------------------------
// sets the date given YYYY,MM,DD,hh,mm,ss
DateTime::DateTime(const int &YYYY,const int &MM,const int &DD,
                   const int &hh,const int &mm,const int &ss)
: year_(YYYY),
  month_(MM),
  day_(DD),
  hour_(hh),
  minute_(mm),
  second_(ss)
{
}

// -----------------------------------------------------------------------------

void DateTime::set(const std::string & str) {
  try {
    istringstream datestream(str);

    year_ = eatChars(datestream,4);

    bool dashes = (datestream.peek() == '-');
    if (dashes) datestream.get();

    month_ = eatChars(datestream,2);
    if (dashes) {
      char c=datestream.get();
      if (c != '-') throw str;
    }

    day_ = eatChars(datestream,2);

    char c=datestream.get();
    if (c != 'T') throw str;

    hour_ = eatChars(datestream,2);

    bool colons = (datestream.peek() == ':');
    if (colons) datestream.get();

    minute_ = eatChars(datestream,2);
    if (colons) {
      char c=datestream.get();
      if (c != ':') throw str;
    }

    second_ = eatChars(datestream,2);

    c=datestream.get();
    if (c != 'Z') throw str;

    datestream.peek();
    if (!datestream.eof()) throw str;

    if (!valid()) throw str;

  }
  catch(...) {
    std::string message="Badly formatted date: ";
    message.append(str);
    throw invalid_argument(message);
  }
}

// -----------------------------------------------------------------------------

ostream& operator<<(std::ostream& output, const DateTime& t) {
    output << t.toString();
    return output;
}

// -----------------------------------------------------------------------------

istream& operator>>(std::istream& input, DateTime& t) {
    std::string time;
    input >> time;
    t.set(time);
    return input;
}

// -----------------------------------------------------------------------------

int DateTime::eatChars (std::istream & is, int nchars) {
  // consume nchars characters from the stream and interpret as an integer
  std::string str;
  for (int i=0; i<nchars; ++i) {
    str.append(1, (char) is.get());
  }

  istringstream mys(str);
  int ret;
  mys >> ret;
  if (mys.fail()) throw(str);
  return ret;
}

// -----------------------------------------------------------------------------

bool DateTime::isLeapYear (int year) {
  return ( ( year%4 == 0 && ( year%100 != 0 ) ) || year%400 ) == 0;
}

// -----------------------------------------------------------------------------

bool DateTime::valid() {
  bool good;
  good = year_   >= 0 && year_   <= 9999 && month_  >= 0 && month_  <= 12 &&
         hour_   >= 0 && hour_   <= 23   && minute_ >= 0 && minute_ <= 59 &&
         second_ >= 0 && second_ <= 59   && day_    >= 0;

  if (good) {
    if (month_ == 9 || month_ == 4 || month_ == 6 || month_ == 11) {
        good = (day_ <= 30);
    } else if (month_ != 2) {
        good = (day_ <= 31);
    } else if (isLeapYear(year_)) {
        good = (day_ <= 29); // leap year
    } else {
        good = (day_ <= 28);
    }
  }

  return good;
}

// -----------------------------------------------------------------------------

void DateTime::set(const int & year, const int & month, const int & day,
               const int & hour, const int & minute, const int & second) {

  year_ = year;
  month_ = month;
  day_ = day;
  hour_ = hour;
  minute_ = minute;
  second_ = second;

  if (!valid()) {
    std::string message = "Invalid DateTime: ";
    message.append(this->toString());
    year_ = 0;
    month_ = 0;
    day_ = 0;
    hour_ = 0;
    minute_ = 0;
    second_ = 0;
    throw invalid_argument(message);
  }
}

// -----------------------------------------------------------------------------

std::string DateTime::toString() const {
  ostringstream os;
  os << setfill('0');
  os << setw(4) << year_;
  os.put('-');
  os << setw(2) << month_;
  os.put('-');
  os << setw(2) << day_;
  os.put('T');
  os << setw(2) << hour_;
  os.put(':');
  os << setw(2) << minute_;
  os.put(':');
  os << setw(2) << second_;
  os.put('Z');
  return os.str();
}

// -----------------------------------------------------------------------------

void DateTime::get(int & year, int & month, int & day,
               int & hour, int & minute, int & second) const {
  year = year_;
  month = month_;
  day = day_;
  hour = hour_;
  minute = minute_;
  second = second_;
}
//==============================================================================
unsigned long long DateTime::dateToJulian() const
//==============================================================================
{
  int m1 = 0, m2 = 0, a = 0, b = 0, c = 0;

//
//  Compute the Julian Day number applying the following formula
//
//  julian_day = ( 1461 * ( y + 4800 + ( m - 14 ) / 12 ) ) / 4 +
//               ( 367 * ( m - 2 - 12 * ( ( m - 14 ) / 12 ) ) ) / 12 -
//                   ( 3 * ( ( y + 4900 + ( m - 14 ) / 12 ) / 100 ) ) / 4 +
//                     d - 32075
//

  m1 = (month_ - 14)/12;
  a  = (1461 * (year_ + 4800 + m1))/4;

  b  = (367 * (month_ - 2 - (12 * m1)))/12;

  m2 = (year_ + 4900 + m1)/100;

  c  = (3 * (m2))/4;

  return a + b - c + day_ - 32075;

}
//==============================================================================
void DateTime::julianToDate(const unsigned long long &julian)
//==============================================================================
{
  unsigned long long l     = 0; 
  unsigned long long n     = 0; 
  unsigned long long i     = 0; 
  unsigned long long j     = 0; 
  unsigned long long jdate = 0; 
  unsigned long long day   = 0; 
  unsigned long long month = 0; 
  unsigned long long year  = 0; 

  const unsigned long long JULIAN_MIN = 0LL;

// Modified Julian date

  jdate = julian;

//  if (jdate >= JULIAN_MIN) { // always true : unsigned >= 0

    l = jdate + 68569;
    n = ( 4 * l ) / 146097;
    l = l - ( 146097 * n + 3 ) / 4;
    i = ( 4000 * ( l + 1 ) ) / 1461001;
    l = l - ( 1461 * i ) / 4 + 31;
    j = ( 80 * l ) / 2447;
    day = l - ( 2447 * j ) / 80;
    l = j / 11;
    month = j + 2 - ( 12 * l );
    year = 100 * ( n - 49 ) + i + l;
    //if (numeric_limits<int>::min() <= year <= numeric_limits<int>::max() ) {

    day_ = (int) day;
    month_ = (int) month;
    year_ = (int) year;
    //} 

// }

}
//==============================================================================
int DateTime::hmsToSeconds() const
//==============================================================================
{
 return SEC_HOUR * hour_ + SEC_MIN * minute_ + second_;
}
//==============================================================================
void DateTime::secondToHms(const int & seconds)
//==============================================================================
{

  int local_sec=0;

  if (seconds >= 0 && seconds <= SEC_DAY) {
    local_sec = seconds;
    hour_ = local_sec / SEC_HOUR;
    local_sec %= SEC_HOUR;
    minute_ = local_sec / MIN_HOUR;
    local_sec %= MIN_HOUR;
    second_ = local_sec;
  }
}
//==============================================================================
void DateTime::addDays(const int & days)
//==============================================================================
{
  unsigned long long julian = 0;

  julian = dateToJulian();

  julian += days;

 //   if (numeric_limits<long>::min() <= julian <= numeric_limits<long>::max() ) {
    julianToDate(julian);
 // }
  
}
//==============================================================================
void DateTime::addHours(const int &hours)
//==============================================================================
{
 DateTime fulldate(year_,month_,day_, hour_, minute_, second_);

 unsigned long long julian = dateToJulian();
 int seconds = hmsToSeconds();

 {
   int days = 0;
   int new_hours = 0;
   int new_seconds = 0;

   new_hours = hours;
   days = new_hours / HOUR_DAY;

   julian += days;

   new_hours %= HOUR_DAY;

   new_seconds = new_hours * SEC_HOUR;
   seconds += new_seconds;

   if (seconds < 0) {
     julian--;
     seconds += SEC_DAY;
   }

   if (seconds >= SEC_DAY) {
     julian++;
     seconds -= SEC_DAY;
   }

 }
 julianToDate(julian);
 secondToHms(seconds);
}

//==============================================================================
void DateTime::addMinutes(const int &minutes)
//==============================================================================
{
 unsigned long long julian = dateToJulian();
 int seconds = hmsToSeconds();
 {
  int days = 0;
  int new_minutes = 0;
  int new_seconds = 0;

  new_minutes = minutes;
  days = new_minutes / MIN_DAY;
  julian += days;
  new_minutes %= MIN_DAY;
  new_seconds = new_minutes * SEC_MIN;
  seconds += new_seconds;

  if (seconds < 0) {
    julian--;
    seconds += SEC_DAY;
  }

  if (seconds >= SEC_DAY) {
    julian++;
    seconds -= SEC_DAY;
  }
 }

 julianToDate(julian);
 secondToHms(seconds);
}
//==============================================================================
void DateTime::addSeconds(const int &seconds)
//==============================================================================
{
 unsigned long long julian = dateToJulian();

 int full_seconds = hmsToSeconds();
 {
  int days=0;
  int new_seconds = 0;
  
  new_seconds = seconds;
  days = new_seconds / SEC_DAY;
 
  julian += days;
  new_seconds %= SEC_DAY;

  full_seconds += (int) new_seconds;

  if (full_seconds < 0) {
    julian--;
    full_seconds += SEC_DAY;
  } else if (full_seconds >= SEC_DAY) {
    julian++;
    full_seconds -= SEC_DAY;
  }
 }
 julianToDate(julian);
 secondToHms(full_seconds);
}

//==============================================================================
int DateTime::daysDateMinusDate(const DateTime &d1) const // return the number of days
//==============================================================================
{
  unsigned long long julian1 = dateToJulian();
  unsigned long long julian2 = d1.dateToJulian();

  return (julian1 - julian2);
}

//==============================================================================
int DateTime::hoursDateMinusDate(const DateTime &d1) const
//==============================================================================
// return the number of hours
{
  int days = daysDateMinusDate(d1);

  int s1 = hmsToSeconds();
  int s2 = d1.hmsToSeconds();

  return (days * HOUR_DAY + (s1-s2)/SEC_HOUR);
}

//==============================================================================
int DateTime::minutesDateMinusDate(const DateTime &d1) const
//==============================================================================
{
  int days = daysDateMinusDate(d1);

  int s1 = hmsToSeconds();
  int s2 = d1.hmsToSeconds();

  return (days*MIN_DAY + (s1-s2) / SEC_MIN);
}

//==============================================================================
int DateTime::secondsDateMinusDate(const DateTime &d1) const
//==============================================================================
{
  int days = daysDateMinusDate(d1);

  int s1 = hmsToSeconds();
  int s2 = d1.hmsToSeconds();

  return (days*SEC_DAY + (s1-s2));
}


// -----------------------------------------------------------------------------

DateTime& DateTime::operator+=(const Duration & s) {


  ASSERT(s.toSeconds()>=numeric_limits<int>::min() &&
         s.toSeconds()<=numeric_limits<int>::max());

	int  secs = s.toSeconds();

	addSeconds(secs);
	return *this;
}

// -----------------------------------------------------------------------------

DateTime& DateTime::operator-=(const Duration & s) {
  Duration negs = s;
  negs.negate();
  return this->operator+=(negs);
}

// -----------------------------------------------------------------------------

const DateTime DateTime::operator+(const Duration & s) const {
    DateTime result = *this; // Make a copy of myself.
    result += s;         // Use += to add s to the copy.
    return result;
  }

// -----------------------------------------------------------------------------

const DateTime DateTime::operator-(const Duration & s) const {
    DateTime result = *this; // Make a copy of myself.
    result -= s;         // Use -= to subtract s from the copy
    return result;
  }

// -----------------------------------------------------------------------------

const Duration DateTime::operator-(const DateTime& other) const {
  return Duration((int) secondsDateMinusDate(other));
}

// -----------------------------------------------------------------------------

bool DateTime::operator==(const DateTime& other) const {
  return (year_ == other.year_) && (month_ == other.month_) &&
         (day_ == other.day_) && (hour_ == other.hour_) &&
         (minute_ == other.minute_) && (second_ == other.second_);
}

// -----------------------------------------------------------------------------

bool DateTime::operator!=(const DateTime& other) const {return !(*this == other);}

bool DateTime::operator<(const DateTime& other) const {return (*this - other) < 0;}

bool DateTime::operator<=(const DateTime& other) const {return (*this - other) <= 0;}

bool DateTime::operator>(const DateTime& other) const {return (*this - other) > 0;}

bool DateTime::operator>=(const DateTime& other) const {return (*this - other) >= 0;}

} //namespace
