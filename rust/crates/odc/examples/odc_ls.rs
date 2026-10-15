//! Decode all the data in an ODB-2 file and print it to stdout.
//!
//! Usage: `cargo run --example odc_ls -- <file.odb>`

use odc::polars::prelude::*;
use odc::{ColumnInfo, ColumnType, Reader, ReaderOptions};

fn write_header(columns: &[ColumnInfo]) {
    for (i, column) in columns.iter().enumerate() {
        let index = i + 1;
        let width = column.decoded_size + 2 - (index.to_string().len() - 1);
        print!("{index}. {:<width$}\t", column.name);
    }
    println!();
}

fn write_bitfield(value: i64, nbits: usize) {
    print!("{value:0nbits$b}");
}

fn write_data(df: &DataFrame, columns: &[ColumnInfo]) -> odc::Result<()> {
    for row in 0..df.height() {
        for column in columns {
            let series = df.column(&column.name)?.as_materialized_series();
            let width = column.decoded_size;

            match column.column_type {
                ColumnType::Integer => match series.i64()?.get(row) {
                    Some(value) => print!("{value:<width$}"),
                    None => print!("."),
                },
                ColumnType::Bitfield => {
                    let nbits = column
                        .bitfield
                        .iter()
                        .map(|bit| usize::try_from(bit.size).unwrap_or(0))
                        .sum();
                    match series.i64()?.get(row) {
                        Some(value) if value != 0 => write_bitfield(value, nbits),
                        _ => print!("."),
                    }
                }
                ColumnType::Real => match series.f32()?.get(row) {
                    Some(value) => print!("{value:<width$.6}"),
                    None => print!("."),
                },
                ColumnType::Double => match series.f64()?.get(row) {
                    Some(value) => print!("{value:<width$.6}"),
                    None => print!("."),
                },
                ColumnType::String => match series.str()?.get(row) {
                    Some(value) => print!("{value:<width$}"),
                    None => print!("."),
                },
                _ => print!("<unknown>"),
            }
            print!("\t");
        }
        println!();
    }
    Ok(())
}

fn main() -> odc::Result<()> {
    let Some(path) = std::env::args().nth(1) else {
        eprintln!("Usage:");
        eprintln!("    odc_ls <odb2 file>");
        std::process::exit(1);
    };

    // Iterate over logical frames, aggregating consecutive compatible
    // physical frames up to the row limit.
    let options = ReaderOptions {
        aggregated: true,
        row_limit: Some(1_000_000),
    };
    let reader = Reader::from_path_with(&path, &options)?;

    for frame in reader.frames() {
        let frame = frame?;
        write_header(frame.columns());

        // Decode the frame and print the data row by row.
        let df = frame.dataframe()?;
        write_data(&df, frame.columns())?;
    }

    Ok(())
}
