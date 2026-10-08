//! Encode sample data supplied as one raw slice per column into an ODB-2
//! file.
//!
//! Usage: `cargo run --example odc_encode_custom -- <out.odb>`

use odc::{Bit, ColumnType, EncodeSource, RawColumn, WriteOptions};

const NROWS: usize = 20;

/// Fill a list by cycling through the values of a pool.
fn cycle<T: Copy>(pool: &[T]) -> Vec<T> {
    (0..NROWS).map(|i| pool[i % pool.len()]).collect()
}

/// Pack strings into fixed-width NUL-padded cells.
fn pack_strings(values: impl IntoIterator<Item = String>, width: usize) -> Vec<u8> {
    let mut data = vec![0_u8; NROWS * width];
    for (row, value) in values.into_iter().enumerate() {
        data[row * width..row * width + value.len()].copy_from_slice(value.as_bytes());
    }
    data
}

fn main() -> odc::Result<()> {
    let Some(path) = std::env::args().nth(1) else {
        eprintln!("Usage:");
        eprintln!("    odc_encode_custom <odb2 output file>");
        std::process::exit(1);
    };

    // Set up one data array per column, including missing values
    let expver = pack_strings((0..NROWS).map(|_| "xxxx".into()), 8);
    let date = [20_210_524_i64; NROWS];
    let statid = pack_strings((0..NROWS).map(|i| format!("stat{i:02}")), 8);
    let wigos = pack_strings((0..NROWS).map(|i| format!("0-12345-0-678{i:02}")), 16);
    #[allow(clippy::cast_precision_loss)]
    let obsvalue: Vec<f64> = (0..NROWS).map(|i| 12.3456 * i as f64).collect();
    let integer_missing = cycle(&[1234, 4321, odc::integer_missing_value()]);
    let double_missing = cycle(&[12.34, 43.21, odc::double_missing_value()]);
    let bitfield_values = cycle(&[0b0000_0001, 0b0000_1011, 0b0110_1011]);

    let columns = [
        RawColumn {
            name: "expver",
            column_type: ColumnType::String,
            data: EncodeSource::Str {
                data: &expver,
                width: 8,
            },
        },
        RawColumn {
            name: "date@hdr",
            column_type: ColumnType::Integer,
            data: EncodeSource::I64(&date),
        },
        RawColumn {
            name: "statid@hdr",
            column_type: ColumnType::String,
            data: EncodeSource::Str {
                data: &statid,
                width: 8,
            },
        },
        RawColumn {
            name: "wigos@hdr",
            column_type: ColumnType::String,
            data: EncodeSource::Str {
                data: &wigos,
                width: 16,
            },
        },
        RawColumn {
            name: "obsvalue@body",
            column_type: ColumnType::Real,
            data: EncodeSource::F64(&obsvalue),
        },
        RawColumn {
            name: "integer_missing",
            column_type: ColumnType::Integer,
            data: EncodeSource::I64(&integer_missing),
        },
        RawColumn {
            name: "double_missing",
            column_type: ColumnType::Real,
            data: EncodeSource::F64(&double_missing),
        },
        RawColumn {
            name: "bitfield_column",
            column_type: ColumnType::Bitfield,
            data: EncodeSource::I64(&bitfield_values),
        },
    ];

    let mut options = WriteOptions::default();

    // Define the named bit groups of the bitfield column
    options.bitfields.insert(
        "bitfield_column".into(),
        vec![
            Bit {
                name: "flag_a".into(),
                size: 1,
                offset: 0,
            },
            Bit {
                name: "flag_b".into(),
                size: 2,
                offset: 1,
            },
            Bit {
                name: "flag_c".into(),
                size: 3,
                offset: 3,
            },
            Bit {
                name: "flag_d".into(),
                size: 1,
                offset: 6,
            },
        ],
    );

    // Add some key/value metadata to the frame
    options
        .properties
        .insert("encoded_by".into(), "odc_example".into());

    odc::write_odb_raw(&columns, &path, &options)?;

    println!("Written {NROWS} rows to {path}");

    Ok(())
}
