//! Extract metadata from a sequence of frames to use as an index for
//! archival, without decoding the data.
//!
//! Usage: `cargo run --example odc_index -- <file.odb>`

use odc::{ColumnType, Frame, Reader, ReaderOptions, Span};

// Define which columns will be used as index keys
const INDEX_KEYS: [&str; 3] = ["key1", "key2", "key3"];

fn key_types(frame: &Frame) -> Vec<ColumnType> {
    INDEX_KEYS
        .iter()
        .filter_map(|key| frame.column(key))
        .map(|column| column.column_type)
        .collect()
}

fn write_index(span: &Span, types: &[ColumnType], offset: u64, length: u64) -> odc::Result<()> {
    println!("Archival unit: offset={offset} length={length}");
    print!("  Key: ");

    // Dump the index values without decoding the frame data
    for (key, column_type) in INDEX_KEYS.iter().zip(types) {
        let value = match *column_type {
            ColumnType::Integer | ColumnType::Bitfield => span.integer_values(key)?[0].to_string(),
            ColumnType::Real | ColumnType::Double => span.real_values(key)?[0].to_string(),
            _ => span.string_values(key)?[0].clone(),
        };
        print!("{key}={value} ");
    }

    println!();
    Ok(())
}

fn main() -> odc::Result<()> {
    let Some(path) = std::env::args().nth(1) else {
        eprintln!("Usage:");
        eprintln!("    odc_index <odb2 file>");
        std::process::exit(1);
    };

    // Open supplied path in non-aggregated mode
    let options = ReaderOptions {
        aggregated: false,
        row_limit: None,
    };
    let reader = Reader::from_path_with(&path, &options)?;

    // Enforce the constant values constraint
    let only_constant = true;

    let mut current: Option<(Span, Vec<ColumnType>, u64, u64)> = None;

    // Iterate over frames
    for frame in reader.frames() {
        let frame = frame?;

        // Get index values for the frame
        let span = frame.span(&INDEX_KEYS, only_constant)?;

        current = match current {
            // If the index values are the same, just increase the length
            Some((last, types, offset, length)) if last == span => {
                Some((last, types, offset, length + span.length()))
            }
            // If the index values differ, output the last set
            Some((last, types, offset, length)) => {
                write_index(&last, &types, offset, length)?;
                let (offset, length) = (span.offset(), span.length());
                Some((span, key_types(&frame), offset, length))
            }
            // Remember the first set of index values
            None => {
                let (offset, length) = (span.offset(), span.length());
                Some((span, key_types(&frame), offset, length))
            }
        };
    }

    // Output last set of index values
    if let Some((span, types, offset, length)) = current {
        write_index(&span, &types, offset, length)?;
    }

    Ok(())
}
