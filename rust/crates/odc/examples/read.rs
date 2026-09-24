//! Stream an ODB-2 file frame by frame, inspecting column metadata and
//! properties before decoding into Polars `DataFrame`s.
//!
//! Usage: `cargo run --example read -- <file.odb>`
//! (e.g. `docs/_static/example.odb` from the odc repository)

use odc::{ColumnType, DecodeOptions, Reader};

fn main() -> odc::Result<()> {
    let path = std::env::args().nth(1).expect("usage: read <file.odb>");

    let reader = Reader::from_path(&path)?;
    for (i, frame) in reader.frames().enumerate() {
        let frame = frame?;
        println!(
            "frame {i}: {} rows x {} columns",
            frame.row_count(),
            frame.column_count()
        );
        for column in frame.columns() {
            print!("  {}: {:?}", column.name, column.column_type);
            if column.column_type == ColumnType::Bitfield {
                let bits: Vec<&str> = column.bitfield.iter().map(|b| b.name.as_str()).collect();
                print!(" [{}]", bits.join(", "));
            }
            println!();
        }
        for (key, value) in frame.properties() {
            println!("  property {key} = {value}");
        }

        // Decode everything...
        let df = frame.dataframe()?;
        println!("{df}");

        // ...or only selected columns.
        if let Some(column) = frame.columns().first() {
            let options = DecodeOptions {
                columns: Some(vec![column.name.clone()]),
                ..DecodeOptions::default()
            };
            let df = frame.dataframe_with(&options)?;
            println!("{df}");
        }
    }
    Ok(())
}
