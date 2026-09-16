# odc

Safe Rust wrapper for ECMWF's [odc](https://github.com/ecmwf/odc) (ODB-2 encoder/decoder) C++ library.

ODB-2 data decodes into [Polars](https://pola.rs/) data frames and encodes from them, following the same model as [pyodc](https://github.com/ecmwf/pyodc). For raw FFI bindings, see the lower-level `odc-sys` crate.

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

```sh
cargo run --example read -- data.odb
cargo run --example write -- out.odb
```

## Features

### Build strategy (mutually exclusive)

- `vendored` - Build odc and its dependencies (eckit) from source.
- `system` - Link against system-installed odc.

`vendored` is enabled by default.

## License

Apache-2.0
