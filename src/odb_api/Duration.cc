/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <locale>         // std::locale, std::isdigit
#include <stdexcept>

#include "eckit/eckit.h"
#include "odb_api/Duration.h"

using std::string;
using std::istringstream;
using std::ostringstream;
using std::istream;
using std::ostream;
using std::locale;
using std::invalid_argument;

namespace utils {

Duration::Duration() : seconds_(0) {}

Duration::Duration(const int64_t s) : seconds_(s) {}

Duration::Duration(const std::string & s) { this->set(s); }

ostream& operator<<(std::ostream& output, const Duration& d) {
    output << d.toString();
    return output;
}

istream& operator>>(std::istream& input, Duration& d) {
    std::string duration;
    input >> duration;
    d.set(duration);
    return input;
}

void Duration::set(const std::string & str) {
  this ->seconds_ = 0;

  try {
    istringstream is(str);

    char c = is.get();
    if (!is.good()) throw str;

    // strip off any initial '-' and/or 'P'

    bool negative = false;
    if (c == '-') {
      negative = true;
      c = is.get();
      if (!is.good()) throw str;
    }

    if (c != 'P') throw str;

    // We now expect either 'T', or an integer followed by 'D'

    c = is.get();
    if (!is.good()) throw str;

    if (c != 'T') {
      is.unget();
      std::string days = eatDigits(is);    
      if (days.length()==0) throw str;
      c = is.get();
      if (!is.good() || c != 'D') throw str;
      istringstream mys(days);
      int ndays;
      mys >> ndays;
      if (mys.fail()) throw str;
      this ->seconds_ = 86400*ndays;
    } else {
      is.unget();
    }

    // Now, we can have the end of the std::string, or a 'T'

    c = is.get();
    if (!is.eof()) {
      if (c != 'T') throw str;  

      // Now we can have an integer followed by 'H', 'M' or 'S'
      
      bool hoursdone = false;
      bool minsdone = false;
      bool secsdone = false;

      for (int i=0; i<3; i++) {
        c = is.peek();
        if (is.eof()) break;
        std::string num = eatDigits(is);
        c = is.get();
        if (!is.good()) throw str;

        if (c == 'H' && !hoursdone && !minsdone && !secsdone) {
          istringstream mys(num);
          int hours;
          mys >> hours;
          if (mys.fail()) throw str;
          this ->seconds_ += 3600*hours;
          hoursdone = true;
        }
        else if (c == 'M' && !minsdone && !secsdone) {
          istringstream mys(num);
          int mins;
          mys >> mins;
          if (mys.fail()) throw str;
          this ->seconds_ += 60*mins;
          minsdone = true;
        }
        else if (c == 'S' && !secsdone) {
          istringstream mys(num);
          int secs;
          mys >> secs;
          if (mys.fail()) throw str;
          this ->seconds_ += secs;
          secsdone = true;
        }
        else throw str;
      }
    }

    c = is.peek();
    if (!is.eof()) throw str;
    
    if (negative) this->negate();

  }
  catch(...) {
    std::string message="Badly formed duration string: ";
    message.append(str);
    throw invalid_argument(message);
  }
}

std::string Duration::eatDigits(std::istream & is) {
  std::string str;
  char c;
  locale loc;
  while (isdigit(c = is.get(),loc)) {
    if (!is.good()) throw;
    str.push_back(c);
  }
  is.unget();
  return str;
}

int64_t Duration::toSeconds () const {return seconds_;}

std::string Duration::toString () const {
  ostringstream os;
  int64_t remainder = seconds_;

  if (remainder < 0) {
    os << "-P";
    remainder = -remainder;
  } else {
    os << "P";
  }

  int days = remainder/86400;
  if (days != 0 ) {
    os << days << "D";
    remainder = remainder - 86400*days; 
  }

  if (remainder != 0) {
    os << "T";
  }

  int hours = remainder/3600;
  if (hours != 0) {
    os << hours << "H";
    remainder = remainder - 3600*hours; 
  }

  int minutes = remainder/60;
  if (minutes != 0) {
    os << minutes << "M";
    remainder = remainder - 60*minutes;
  }

  if (remainder != 0 || seconds_ == 0) {
    os << remainder << "S";
  }

  return os.str();
}

    bool Duration::operator==(const Duration& other) const {
      return this->seconds_ == other.seconds_;
    }

    bool Duration::operator!=(const Duration& other) const {
      return this->seconds_ != other.seconds_;
    }

    bool Duration::operator<(const Duration& other) const {
      return this->seconds_ < other.seconds_;
    }

    bool Duration::operator<=(const Duration& other) const {
      return this->seconds_ <= other.seconds_;
    }

    bool Duration::operator>(const Duration& other) const {
      return this->seconds_ > other.seconds_;
    }

    bool Duration::operator>=(const Duration& other) const {
      return this->seconds_ >= other.seconds_;
    }

    int Duration::operator%(const Duration& other) const {
      return this->seconds_ % other.seconds_;
    }

} //namespace
