/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file MDUpdatingIterator.cc
///
/// @author Piotr Kuchta, March 2012

namespace odc {

template <typename T>
MDUpdatingIterator<T>::MDUpdatingIterator(T& ii, const T& end, const std::vector<std::string>& columns,
                                          const std::vector<std::string>& types) :
    ii_(ii),
    end_(end),
    columns_(columns),
    columnIndices_(columns.size()),
    types_(types),
    bitfieldDefs_(),
    md_(ii->columns()),
    data_(0),
    refCount_(0),
    noMore_(false) {
    ASSERT(columns.size() == types.size());
    for (size_t i = 0; i < types.size(); ++i) {
        eckit::Log::info() << columns[i] << " : " << types[i] << std::endl;

        // Only bitfoelds now:
        // [active:1;passive:1;rejected:1;blacklisted:1;use_emiskf_only:1;monthly:1;constant:1;experimental:1;whitelist:]
        ASSERT(types[i].size());
        ASSERT(types[i][0] == '[');
        ASSERT(types[i][types[i].size() - 1] == ']');

        BitfieldDef bf;
        std::vector<std::string> parts(eckit::StringTools::split(";", types[i].substr(1, types[i].size() - 2)));
        for (size_t p = 0; p < parts.size(); ++p) {
            std::vector<std::string> field = eckit::StringTools::split(":", parts[p]);
            bf.first.push_back(field[0]);
            bf.second.push_back(atoi(field[1].c_str()));
        }
        bitfieldDefs_.push_back(bf);
        eckit::Log::info() << "" << i << ": " << columns[i] << " - " << bf.first
                           << std::endl;  // "[" << bf.second << "]" << std::endl;
    }

    update();
}

template <typename T>
MDUpdatingIterator<T>::MDUpdatingIterator(const T& end) :
    ii_(end), end_(end), columnIndices_(), types_(), md_(0), data_(0), refCount_(0), noMore_(true) {}

template <typename T>
MDUpdatingIterator<T>::~MDUpdatingIterator() {}

template <typename T>
MetaData& MDUpdatingIterator<T>::columns() {
    md_ = ii_->columns();
    for (size_t i = 0; i < columns_.size(); ++i)
        md_[md_.columnIndex(columns_[i])]->bitfieldDef(bitfieldDefs_[i]);
    return md_;
}

template <typename T>
void MDUpdatingIterator<T>::update() {
    columns();
    data_ = const_cast<double*>(ii_->data());
}

template <typename T>
bool MDUpdatingIterator<T>::isNewDataset() {
    return ii_->isNewDataset();
}

template <typename T>
bool MDUpdatingIterator<T>::next() {
    if (noMore_)
        return noMore_;
    ++ii_;
    bool r = ii_ != end_;
    if (r) {
        if (ii_->isNewDataset())
            update();
    }
    noMore_ = !r;
    return r;
}

}  // namespace odc
