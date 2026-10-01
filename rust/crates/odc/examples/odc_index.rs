//! Extract metadata from a sequence of frames to use as an index for
//! archival, without decoding the data.
//!
//! Usage: `cargo run --example odc_index -- <file.odb>`

use odc::{Reader, ReaderOptions, Span, SpanValues};

fn write_index(span: &Span, offset: u64, length: u64) -> odc::Result<()> {
    println!("Archival unit: offset={offset} length={length}");
    print!("  Key: ");

    // Dump the index values without decoding the frame data
    for (name, values) in span.columns()? {
        match values {
            SpanValues::Integer(vals) => print!("{name}={} ", vals[0]),
            SpanValues::Real(vals) => print!("{name}={} ", vals[0]),
            SpanValues::String(vals) => print!("{name}={} ", vals[0]),
        }
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

    // Define which columns will be used as index keys
    let index_keys = ["key1", "key2", "key3"];

    // Enforce the constant values constraint
    let only_constant = true;

    let mut current: Option<(Span, u64, u64)> = None;

    // Iterate over frames
    for frame in reader.frames() {
        let frame = frame?;

        // Get index values for the frame
        let span = frame.span(&index_keys, only_constant)?;

        current = match current {
            // If the index values are the same, just increase the length
            Some((last, offset, length)) if last == span => {
                Some((last, offset, length + span.length()))
            }
            // If the index values differ, output the last set
            Some((last, offset, length)) => {
                write_index(&last, offset, length)?;
                let (offset, length) = (span.offset(), span.length());
                Some((span, offset, length))
            }
            // Remember the first set of index values
            None => {
                let (offset, length) = (span.offset(), span.length());
                Some((span, offset, length))
            }
        };
    }

    // Output last set of index values
    if let Some((span, offset, length)) = current {
        write_index(&span, offset, length)?;
    }

    Ok(())
}
