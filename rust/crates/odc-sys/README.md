# odc-sys

Low-level Rust bindings to ECMWF's [odc](https://github.com/ecmwf/odc) (ODB-2 encoder/decoder) C++ library.

This crate provides raw FFI bindings using [cxx](https://cxx.rs/). For a safe, ergonomic API, use the higher-level `odc` crate (planned).

## Features

### Build strategy (mutually exclusive)

- `vendored` - Build odc and its dependencies (eckit) from source.
- `system` - Link against system-installed odc.

`vendored` is enabled by default.

## License

Apache-2.0
