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
- `handles` — encode and decode through eckit `DataHandle`s: explicit write handles and in-memory buffers.
- `odc_ls` — decode all the data in a file and print it to stdout.
- `odc_header` — print frame metadata without decoding the data.
- `odc_encode_custom` — encode raw column slices, including missing values and bitfields.
- `odc_encode_row_major` — encode a row-major block of 8-byte cells.
- `odc_index` — extract archival-index metadata via spans, without decoding the data.

```sh
cargo run --example read -- data.odb
cargo run --example write -- out.odb
cargo run --example handles -- out.odb
```

The `odc_*` examples mirror the multi-language programs in the [odc documentation](https://odc.readthedocs.io/en/latest/content/usage-examples.html).

## Features

### Build strategy (mutually exclusive)

- `vendored` - Build odc and its dependencies (eckit) from source.
- `system` - Link against system-installed odc.

`vendored` is enabled by default.

## License

Apache-2.0
