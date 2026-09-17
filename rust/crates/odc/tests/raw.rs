//! Raw buffer access: `Frame::decode_into` and `write_odb_raw`.

use odc::polars::prelude::*;
use odc::{DecodeTarget, Error, WriteOptions};
use std::path::PathBuf;
use tempfile::TempDir;

fn write_df(df: &DataFrame) -> odc::Result<(TempDir, PathBuf)> {
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("raw.odb");
    odc::write_odb(df, &path, &WriteOptions::default())?;
    Ok((dir, path))
}

fn first_frame(path: &PathBuf) -> odc::Result<odc::Frame> {
    let reader = odc::Reader::from_path(path)?;
    reader.frames().next().expect("stream yields one frame")
}

#[test]
fn decode_into_caller_buffers() -> odc::Result<()> {
    let df = df!(
        "i" => [Some(1_i64), None, Some(3)],
        "f" => [Some(0.5_f64), None, Some(2.5)],
        "s" => ["ab", "cdefghij", ""],
    )?;
    let (_dir, path) = write_df(&df)?;
    let frame = first_frame(&path)?;

    let width = frame.column("s").expect("column exists").decoded_size;
    let mut ints = vec![0_i64; 3];
    let mut floats = vec![0.0_f64; 3];
    let mut strings = vec![0_u64; 3 * width / 8];
    let rows = frame.decode_into(
        &mut [
            ("i", DecodeTarget::I64(&mut ints)),
            ("f", DecodeTarget::F64(&mut floats)),
            (
                "s",
                DecodeTarget::Str {
                    data: &mut strings,
                    width,
                },
            ),
        ],
        1,
    )?;
    assert_eq!(rows, 3);

    assert_eq!(ints, [1, odc::integer_missing_value(), 3]);
    let float_bits: Vec<u64> = floats.iter().map(|v| v.to_bits()).collect();
    let expected_bits = [
        0.5_f64.to_bits(),
        odc::double_missing_value().to_bits(),
        2.5_f64.to_bits(),
    ];
    assert_eq!(float_bits, expected_bits);

    let slots = width / 8;
    let cell = |row: usize| -> Vec<u8> {
        strings[row * slots..(row + 1) * slots]
            .iter()
            .flat_map(|slot| slot.to_ne_bytes())
            .collect()
    };
    assert_eq!(&cell(0)[..2], b"ab");
    assert!(cell(0)[2..].iter().all(|&b| b == 0));
    assert_eq!(&cell(1)[..8], b"cdefghij");
    assert!(cell(2).iter().all(|&b| b == 0));
    Ok(())
}

#[test]
fn decode_into_rejects_bad_buffers() -> odc::Result<()> {
    let df = df!("i" => [1_i64], "s" => ["abc"])?;
    let (_dir, path) = write_df(&df)?;
    let frame = first_frame(&path)?;

    let mut floats = vec![0.0_f64; 1];
    let result = frame.decode_into(&mut [("i", DecodeTarget::F64(&mut floats))], 1);
    assert!(matches!(result, Err(Error::InvalidBuffer { column, .. }) if column == "i"));

    let mut short: Vec<i64> = Vec::new();
    let result = frame.decode_into(&mut [("i", DecodeTarget::I64(&mut short))], 1);
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let mut cells = vec![0_u64; 1];
    let result = frame.decode_into(
        &mut [(
            "s",
            DecodeTarget::Str {
                data: &mut cells,
                width: 4,
            },
        )],
        1,
    );
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let mut ints = vec![0_i64; 1];
    let result = frame.decode_into(&mut [("nope", DecodeTarget::I64(&mut ints))], 1);
    assert!(matches!(result, Err(Error::ColumnNotFound(c)) if c == "nope"));
    Ok(())
}
