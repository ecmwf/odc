//! Build a Polars `DataFrame` and encode it as ODB-2 — including a bitfield
//! column and frame properties — then read it back.
//!
//! Usage: `cargo run --example write -- <out.odb>`

use odc::polars::prelude::*;
use odc::{Bit, ColumnType, ReadOptions, WriteOptions};

fn main() -> odc::Result<()> {
    let path = std::env::args().nth(1).expect("usage: write <out.odb>");

    // Int64 columns encode as INTEGER, Float64 as DOUBLE, Float32 as REAL,
    // String as STRING; nulls become ODB missing values.
    let df = df!(
        "expver" => ["0001", "0001", "0001"],
        "seqno@hdr" => [1_i64, 2, 3],
        "obsvalue@body" => [Some(274.5_f64), None, Some(271.9)],
        "flags@body" => [0b01_i64, 0b11, 0b10],
    )?;

    let mut options = WriteOptions::default();
    // Encode `flags@body` as a BITFIELD instead of INTEGER, with named
    // bit groups.
    options
        .types
        .insert("flags@body".into(), ColumnType::Bitfield);
    options.bitfields.insert(
        "flags@body".into(),
        vec![
            Bit {
                name: "active".into(),
                size: 1,
                offset: 0,
            },
            Bit {
                name: "blacklisted".into(),
                size: 1,
                offset: 1,
            },
        ],
    );
    options
        .properties
        .insert("source".into(), "odc write example".into());

    odc::write_odb(&df, &path, &options)?;
    println!("wrote {} rows to {path}", df.height());

    let round_trip = odc::read_odb_single(&path, &ReadOptions::default())?;
    println!("{round_trip}");
    Ok(())
}
