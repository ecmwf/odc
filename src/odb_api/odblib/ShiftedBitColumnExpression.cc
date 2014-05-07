/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

namespace odb {
namespace sql {
namespace expression {
/*
double ShiftedBitColumnExpression::eval(bool& missing) const
{
	if(value_->second) missing = true;
	unsigned long x = static_cast<unsigned long>(value_->first);
	return (x & mask_) >> bitShift_;
}

void ShiftedBitColumnExpression::print(std::ostream& s) const { s << columnName_ << "#" << shift_; }
*/

} // namespace expression
} // namespace sql
} // namespace odb

