//! Encode and decode ODB-2 through eckit `DataHandle`s instead of file
//! paths: write via an explicitly opened handle, then decode the same bytes
//! from an in-memory buffer.
//!
//! Usage: `cargo run --example handles -- <out.odb>`

use odc::eckit::DataHandle;
use odc::polars::prelude::*;
use odc::{Reader, ReaderOptions, WriteOptions};

fn main() -> odc::Result<()> {
    let path = std::env::args().nth(1).expect("usage: handles <out.odb>");

    let df = df!(
        "expver" => ["0001", "0001", "0001"],
        "seqno@hdr" => [1_i64, 2, 3],
        "obsvalue@body" => [Some(274.5_f64), None, Some(271.9)],
    )?;

    // Write through an explicitly opened handle — any writable DataHandle
    // works here (file, tee, ...).
    let mut handle = DataHandle::from_path(&path)?.open_for_write(0)?;
    odc::write_odb_to(&df, &mut handle, &WriteOptions::default())?;
    handle.close()?;

    // Decode from an in-memory buffer.
    let bytes = std::fs::read(&path)?;
    let handle = DataHandle::from_buffer(&bytes)?;
    let reader = Reader::from_handle(handle, &ReaderOptions::default())?;
    for frame in reader.frames() {
        println!("{}", frame?.dataframe()?);
    }
    Ok(())
}
