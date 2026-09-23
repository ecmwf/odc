//! Encode sample data laid out as one row-major block of 8-byte cells into
//! an ODB-2 file.
//!
//! Usage: `cargo run --example odc_encode_row_major -- <out.odb>`

use odc::{Bit, ColumnType, RowMajorColumn, WriteOptions};

const NROWS: usize = 20;
const NCELLS: usize = 9;

const fn int_cell(value: i64) -> u64 {
    u64::from_ne_bytes(value.to_ne_bytes())
}

const fn real_cell(value: f64) -> u64 {
    value.to_bits()
}

/// Pack a string into consecutive NUL-padded 8-byte cells.
fn set_str(cells: &mut [u64], value: &str) {
    let bytes = value.as_bytes();
    for (i, cell) in cells.iter_mut().enumerate() {
        let mut chunk = [0_u8; 8];
        let start = (i * 8).min(bytes.len());
        let end = (i * 8 + 8).min(bytes.len());
        chunk[..end - start].copy_from_slice(&bytes[start..end]);
        *cell = u64::from_ne_bytes(chunk);
    }
}

fn main() -> odc::Result<()> {
    let Some(path) = std::env::args().nth(1) else {
        eprintln!("Usage:");
        eprintln!("    odc_encode_row_major <odb2 output file>");
        std::process::exit(1);
    };

    let integer_pool = [1234, 4321, odc::integer_missing_value()];
    let double_pool = [12.34, 43.21, odc::double_missing_value()];
    let bitfield_pool = [0b0000_0001, 0b0000_1011, 0b0110_1011];

    // Fill in the data array of rows x cells with scratch values
    let mut data = vec![0_u64; NROWS * NCELLS];
    for (i, row) in data.chunks_mut(NCELLS).enumerate() {
        set_str(&mut row[0..1], "xxxx"); // expver
        row[1] = int_cell(20_210_524); // date@hdr
        set_str(&mut row[2..3], &format!("stat{i:02}")); // statid@hdr
        set_str(&mut row[3..5], &format!("0-12345-0-678{i:02}")); // wigos@hdr
        #[allow(clippy::cast_precision_loss)]
        {
            row[5] = real_cell(12.3456 * i as f64); // obsvalue@body
        }
        row[6] = int_cell(integer_pool[i % 3]); // integer_missing
        row[7] = real_cell(double_pool[i % 3]); // double_missing
        row[8] = int_cell(bitfield_pool[i % 3]); // bitfield_column
    }

    // Define all column names, their types, and their cell sizes
    //   Column `wigos@hdr` is a 16-byte string column, hence takes 2 cells in the array => NCELLS=9
    let columns = [
        RowMajorColumn {
            name: "expver",
            column_type: ColumnType::String,
            size: 8,
        },
        RowMajorColumn {
            name: "date@hdr",
            column_type: ColumnType::Integer,
            size: 8,
        },
        RowMajorColumn {
            name: "statid@hdr",
            column_type: ColumnType::String,
            size: 8,
        },
        RowMajorColumn {
            name: "wigos@hdr",
            column_type: ColumnType::String,
            size: 16,
        },
        RowMajorColumn {
            name: "obsvalue@body",
            column_type: ColumnType::Real,
            size: 8,
        },
        RowMajorColumn {
            name: "integer_missing",
            column_type: ColumnType::Integer,
            size: 8,
        },
        RowMajorColumn {
            name: "double_missing",
            column_type: ColumnType::Real,
            size: 8,
        },
        RowMajorColumn {
            name: "bitfield_column",
            column_type: ColumnType::Bitfield,
            size: 8,
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

    odc::write_odb_row_major(&data, &columns, &path, &options)?;

    println!("Written {NROWS} rows to {path}");

    Ok(())
}
