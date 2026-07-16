# odc

Safe Rust wrapper for ECMWF's [odc](https://github.com/ecmwf/odc) (ODB-2 encoder/decoder) C++ library.

ODB-2 data decodes into [Polars](https://pola.rs/) data frames and encodes from them, following the same model as [pyodc](https://github.com/ecmwf/pyodc). For raw FFI bindings, see the lower-level `odc-sys` crate.

## Features

### Build strategy (mutually exclusive)

- `vendored` - Build odc and its dependencies (eckit) from source.
- `system` - Link against system-installed odc.

`vendored` is enabled by default.

## License

Apache-2.0
