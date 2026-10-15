// odc Span bridge — implementation.

#include "SpanWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include <set>
#include <string>
#include <utility>

namespace odc_bridge {

namespace {

/// Records the name and value kind of each visited column.
class ColumnVisitor : public odc::api::SpanVisitor {
    rust::Vec<SpanColumn>& out_;

public:

    explicit ColumnVisitor(rust::Vec<SpanColumn>& out) : out_(out) {}

    void operator()(const std::string& name, const std::set<long>&) override {
        out_.push_back(SpanColumn{rust::String(name), odc::api::INTEGER});
    }

    void operator()(const std::string& name, const std::set<double>&) override {
        out_.push_back(SpanColumn{rust::String(name), odc::api::DOUBLE});
    }

    void operator()(const std::string& name, const std::set<std::string>&) override {
        out_.push_back(SpanColumn{rust::String(name), odc::api::STRING});
    }
};

}  // namespace

SpanWrapper::SpanWrapper(odc::api::Span&& span) : span_(std::move(span)) {}

uint64_t SpanWrapper::offset() const {
    return static_cast<uint64_t>(static_cast<long long>(span_.offset()));
}

uint64_t SpanWrapper::length() const {
    return static_cast<uint64_t>(static_cast<long long>(span_.length()));
}

bool SpanWrapper::equals(const SpanWrapper& other) const {
    return span_ == other.span_;
}

rust::Vec<SpanColumn> SpanWrapper::columns() const {
    rust::Vec<SpanColumn> result;
    ColumnVisitor visitor(result);
    span_.visit(visitor);
    return result;
}

rust::Vec<int64_t> SpanWrapper::integer_values(rust::Str column) const {
    rust::Vec<int64_t> values;
    const auto& vals = span_.getIntegerValues(std::string(column));
    values.reserve(vals.size());
    for (long val : vals) {
        values.push_back(val);
    }
    return values;
}

rust::Vec<double> SpanWrapper::real_values(rust::Str column) const {
    rust::Vec<double> values;
    const auto& vals = span_.getRealValues(std::string(column));
    values.reserve(vals.size());
    for (double val : vals) {
        values.push_back(val);
    }
    return values;
}

rust::Vec<rust::String> SpanWrapper::string_values(rust::Str column) const {
    rust::Vec<rust::String> values;
    const auto& vals = span_.getStringValues(std::string(column));
    values.reserve(vals.size());
    for (const auto& val : vals) {
        values.push_back(rust::String(val));
    }
    return values;
}

}  // namespace odc_bridge
