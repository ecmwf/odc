// odc C++ bridge for Rust FFI — umbrella header pulled in by the
// cxx-generated bridge (`include!("OdcBridge.h")` in lib.rs). Real
// declarations live in the per-topic headers below.
#pragma once

// Note: the auto-generated `rust::behavior::trycatch` lives in
// `odc_exceptions.h`, which lib.rs pulls into the cxx-generated translation
// unit via its own `include!` (before this header). It must not be included
// from here: downstream `-sys` crates have their own generated
// `<ns>_exceptions.h` and must not see odc's transitively, or they would
// have two `trycatch` specializations in one translation unit.

#include "DecoderWrapper.h"
#include "EncoderWrapper.h"
#include "FrameWrapper.h"
#include "ReaderWrapper.h"
#include "SettingsWrapper.h"
