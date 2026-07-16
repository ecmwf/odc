// odc Settings bridge — wraps `odc::api::Settings`.
#pragma once

#include "rust/cxx.h"

#include <cstdint>

namespace odc_bridge {

/// Wraps `odc::api::Settings` — process-global settings and version info.
/// Static-only; never instantiated.
class SettingsWrapper {
public:

    SettingsWrapper() = delete;

    static void treat_integers_as_doubles(bool flag);
    static int64_t integer_missing_value();
    static void set_integer_missing_value(int64_t value);
    static double double_missing_value();
    static void set_double_missing_value(double value);
    static rust::String version();
    static rust::String gitsha1();
};

}  // namespace odc_bridge
