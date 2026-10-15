//! Print information extracted from the header of each frame in ODB-2
//! files, without decoding the data.
//!
//! Usage: `cargo run --example odc_header -- <file.odb> [<file2.odb> ...]`

use odc::{ColumnType, Reader, ReaderOptions};

const fn type_name(column_type: ColumnType) -> &'static str {
    match column_type {
        ColumnType::Integer => "integer",
        ColumnType::Real => "real",
        ColumnType::String => "string",
        ColumnType::Bitfield => "bitfield",
        ColumnType::Double => "double",
        _ => "ignore",
    }
}

fn main() -> odc::Result<()> {
    let paths: Vec<String> = std::env::args().skip(1).collect();
    if paths.is_empty() {
        eprintln!("Usage:");
        eprintln!("    odc_header <odb2 file 1> [<odb2 file 2> ...]");
        std::process::exit(1);
    }

    // Iterate over all frames in the stream in non-aggregated mode.
    let options = ReaderOptions {
        aggregated: false,
        row_limit: None,
    };

    for path in &paths {
        println!("File: {path}");

        let reader = Reader::from_path_with(path, &options)?;
        for (i, frame) in reader.frames().enumerate() {
            let frame = frame?;

            println!(
                "  Frame: {}, Row count: {}, Column count: {}",
                i + 1,
                frame.row_count(),
                frame.column_count()
            );

            for (key, value) in frame.properties() {
                println!("  Property: {key} => {value}");
            }

            for (col, column) in frame.columns().iter().enumerate() {
                println!(
                    "    Column: {}, Name: {}, Type: {}, Size: {}",
                    col + 1,
                    column.name,
                    type_name(column.column_type),
                    column.decoded_size
                );

                for (bf, bit) in column.bitfield.iter().enumerate() {
                    println!(
                        "      Bitfield: {}, Name: {}, Offset: {}, Nbits: {}",
                        bf + 1,
                        bit.name,
                        bit.offset,
                        bit.size
                    );
                }
            }

            println!();
        }
    }

    Ok(())
}
