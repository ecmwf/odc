/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TIME_HEADER_H
#define TIME_HEADER_H

// C++ header files
#include <string>
#include <iostream>

// Forward declarations
namespace utils {
  class Duration;
}

namespace utils {

const int SEC_MIN     =    60;
const int SEC_HOUR    =  3600;
const int SEC_DAY     = 86400;
const int HOUR_DAY    =    24;
const int MIN_HOUR    =    60;
const int MIN_DAY     =  1440;

// Forward declarations
    // ---- No forward declarations ---- //

//! This class represents time, and provides methods to manipulate time
/*! 
 * DateTime is represented internally as UTC quantized to the nearest second.
 */

class DateTime {

//! << and >> can be used to output and set the time as ISO 8601 strings
  friend std::ostream& operator<<(std::ostream&, const DateTime&);
  friend std::istream& operator>>(std::istream&, DateTime&);

public:

// -- Constructors
  DateTime(); // sets the date to 0000-00-00T00:00:00Z

  DateTime(const std::string &); // sets the date given an ISO 8601 string

  DateTime(const int&,const int &,const int &,const int &,const int &,const int &); // sets the date given YYYY,MM,DD,hh,mm,ss

// -- Destructor
  //  ~DateTime()  -- not required. This is a simple class.

// -- Methods

  //! Set the time from an ISO 8601 format string: ${date}T${time}Z
  //! where date is YYYYMMDD or YYYY-MM-DD and time is hhmmss or hh:mm:ss
  void set(const std::string &);

  //! Set the date and time from integers YYYY,MM,DD,hh,mm,ss
  void set(const int &, const int &, const int &,
           const int &, const int &, const int &);

  //! Convert the time to ISO 8601 format: YYYY-MM-DDThh:mm:ssZ
  std::string toString() const;

  //! Get the date and time as integers
  void get(int &, int &, int &, int &, int &, int &) const;

  // Functions from eclib
  void addDays(const int &);
  void addHours(const int &);
  void addMinutes(const int &);
  void addSeconds(const int &);
  unsigned long long dateToJulian() const;
  void julianToDate(const unsigned long long &);
  int hmsToSeconds() const;
  void secondToHms(const int &);

  int daysDateMinusDate(const DateTime &) const; // return the number of days
  int hoursDateMinusDate(const DateTime &) const; // return the number of hours
  int minutesDateMinusDate(const DateTime&) const;
  int secondsDateMinusDate(const DateTime&) const;

  // Operators to add/subtract a Duration to/from a DateTime
  DateTime& operator+=(const Duration & );
  DateTime& operator-=(const Duration & );
  const DateTime operator+(const Duration & ) const;
  const DateTime operator-(const Duration & ) const;

  // Difference in seconds between two DateTimes
  const Duration operator-(const DateTime&) const;

  // Comparison operators
  bool operator==(const DateTime&) const;
  bool operator!=(const DateTime&) const;
  bool operator<(const DateTime&) const;
  bool operator<=(const DateTime&) const;
  bool operator>(const DateTime&) const;
  bool operator>=(const DateTime&) const;

private:

// -- Copy allowed
// DateTime(const DateTime&); -- default shallow copy is OK
// DateTime& operator=(const DateTime&); -- default assignment is OK

  int eatChars (std::istream &, int);
  bool isLeapYear (int);
  bool valid();

// -- Members

  int year_;
  int month_;
  int day_;
  int hour_;
  int minute_;
  int second_;

// -- Overridden methods

};

} //namespace utils

#endif
