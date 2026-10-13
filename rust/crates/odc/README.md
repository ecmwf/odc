# odc

Safe Rust wrapper for ECMWF's [odc](https://github.com/ecmwf/odc) (ODB-2 encoder/decoder) C++ library.

ODB-2 data decodes into [Polars](https://pola.rs/) data frames and encodes from them, following the same model as [pyodc](https://github.com/ecmwf/pyodc). For raw FFI bindings, see the lower-level [`odc-sys`](https://crates.io/crates/odc-sys) crate.

## Installation

Add the crate to your `Cargo.toml`:

```toml
[dependencies]
odc = "1.6"
```

The default `vendored` feature builds the odc C++ library (and its eckit
dependency) from source, which requires CMake and a C++17 compiler.

## Usage

```rust
use odc::{ReadOptions, WriteOptions};

fn main() -> odc::Result<()> {
    // Decode a whole file into a single Polars DataFrame.
    let df = odc::read_odb_single("data.odb", &ReadOptions::default())?;
    println!("{df}");

    // Encode it back.
    odc::write_odb(&df, "copy.odb", &WriteOptions::default())?;
    Ok(())
}
```

For streaming access, open a `Reader` and iterate its frames, inspecting column metadata and properties before deciding what to decode. See the `examples/` directory:

- `read` — stream a file frame by frame, inspect metadata, decode all or selected columns.
- `write` — build a `DataFrame` and encode it, including a bitfield column and frame properties.
- `handles` — encode and decode through eckit `DataHandle`s: explicit write handles and in-memory buffers.

```sh
cargo run --example read -- data.odb
cargo run --example write -- out.odb
cargo run --example handles -- out.odb
```

## Features

### Build strategy (mutually exclusive)

- `vendored` - Build odc and its dependencies (eckit) from source.
- `system` - Link against system-installed odc.

`vendored` is enabled by default.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
