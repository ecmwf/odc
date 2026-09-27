// odc Settings bridge — implementation.

#include "SettingsWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include "odc/api/Odb.h"

namespace odc_bridge {

void SettingsWrapper::treat_integers_as_doubles(bool flag) {
    odc::api::Settings::treatIntegersAsDoubles(flag);
}

int64_t SettingsWrapper::integer_missing_value() {
    return static_cast<int64_t>(odc::api::Settings::integerMissingValue());
}

void SettingsWrapper::set_integer_missing_value(int64_t value) {
    odc::api::Settings::setIntegerMissingValue(static_cast<long>(value));
}

double SettingsWrapper::double_missing_value() {
    return odc::api::Settings::doubleMissingValue();
}

void SettingsWrapper::set_double_missing_value(double value) {
    odc::api::Settings::setDoubleMissingValue(value);
}

rust::String SettingsWrapper::version() {
    return rust::String(odc::api::Settings::version());
}

rust::String SettingsWrapper::gitsha1() {
    return rust::String(odc::api::Settings::gitsha1());
}

}  // namespace odc_bridge
