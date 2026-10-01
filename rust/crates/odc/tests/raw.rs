//! Raw buffer access: `Frame::decode_into` and `write_odb_raw`.

use odc::polars::prelude::*;
use odc::{
    Bit, CellColumn, ColumnType, DecodeTarget, EncodeSource, Error, RawColumn, ReadOptions,
    StridedColumn, WriteOptions,
};
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

#[test]
fn raw_encode_roundtrip() -> odc::Result<()> {
    let ints = [1_i64, odc::integer_missing_value(), 3];
    let floats = [0.5_f64, odc::double_missing_value(), 2.5];
    let mut cells = [0_u8; 24];
    cells[..2].copy_from_slice(b"ab");
    cells[8..16].copy_from_slice(b"cdefghij");
    let columns = [
        RawColumn {
            name: "i",
            column_type: ColumnType::Integer,
            data: EncodeSource::I64(&ints),
        },
        RawColumn {
            name: "f",
            column_type: ColumnType::Double,
            data: EncodeSource::F64(&floats),
        },
        RawColumn {
            name: "s",
            column_type: ColumnType::String,
            data: EncodeSource::Str {
                data: &cells,
                width: 8,
            },
        },
    ];

    let dir = tempfile::tempdir()?;
    let path = dir.path().join("raw.odb");
    odc::write_odb_raw(&columns, &path, &WriteOptions::default())?;

    let out = odc::read_odb_single(&path, &ReadOptions::default())?;
    let expected = df!(
        "i" => [Some(1_i64), None, Some(3)],
        "f" => [Some(0.5_f64), None, Some(2.5)],
        "s" => ["ab", "cdefghij", ""],
    )?;
    assert!(
        out.equals_missing(&expected),
        "expected:\n{expected}\ngot:\n{out}"
    );
    Ok(())
}

#[test]
fn raw_encode_bitfield() -> odc::Result<()> {
    let flags = [0b01_i64, 0b11];
    let mut options = WriteOptions::default();
    options.bitfields.insert(
        "flags".into(),
        vec![Bit {
            name: "active".into(),
            size: 1,
            offset: 0,
        }],
    );
    let columns = [RawColumn {
        name: "flags",
        column_type: ColumnType::Bitfield,
        data: EncodeSource::I64(&flags),
    }];

    let dir = tempfile::tempdir()?;
    let path = dir.path().join("raw.odb");
    odc::write_odb_raw(&columns, &path, &options)?;

    let frame = first_frame(&path)?;
    let column = frame.column("flags").expect("column exists");
    assert_eq!(column.column_type, ColumnType::Bitfield);
    assert!(
        frame
            .dataframe()?
            .equals(&df!("flags" => [0b01_i64, 0b11])?)
    );
    Ok(())
}

#[test]
fn raw_encode_rejects_bad_columns() -> odc::Result<()> {
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("raw.odb");
    let a = [1_i64, 2];
    let b = [1.5_f64];

    let columns = [
        RawColumn {
            name: "a",
            column_type: ColumnType::Integer,
            data: EncodeSource::I64(&a),
        },
        RawColumn {
            name: "b",
            column_type: ColumnType::Double,
            data: EncodeSource::F64(&b),
        },
    ];
    let result = odc::write_odb_raw(&columns, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { column, .. }) if column == "b"));

    let columns = [RawColumn {
        name: "a",
        column_type: ColumnType::Double,
        data: EncodeSource::I64(&a),
    }];
    let result = odc::write_odb_raw(&columns, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { column, .. }) if column == "a"));

    let cells = [0_u8; 12];
    let columns = [RawColumn {
        name: "s",
        column_type: ColumnType::String,
        data: EncodeSource::Str {
            data: &cells,
            width: 12,
        },
    }];
    let result = odc::write_odb_raw(&columns, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let result = odc::write_odb_raw(&[], &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::EmptyDataFrame)));
    Ok(())
}

#[test]
fn row_major_roundtrip() -> odc::Result<()> {
    let columns = [
        CellColumn {
            name: "i",
            column_type: ColumnType::Integer,
            size: 8,
        },
        CellColumn {
            name: "f",
            column_type: ColumnType::Double,
            size: 8,
        },
        CellColumn {
            name: "s",
            column_type: ColumnType::String,
            size: 16,
        },
    ];

    let ints = [1_i64, odc::integer_missing_value(), 3];
    let floats = [0.5_f64, odc::double_missing_value(), 2.5];
    let strings = ["one", "twotwotwotwo", ""];
    let mut cells = vec![0_u64; 12];
    for row in 0..3 {
        cells[row * 4] = u64::from_ne_bytes(ints[row].to_ne_bytes());
        cells[row * 4 + 1] = floats[row].to_bits();
        let mut bytes = [0_u8; 16];
        bytes[..strings[row].len()].copy_from_slice(strings[row].as_bytes());
        let mut lo = [0_u8; 8];
        let mut hi = [0_u8; 8];
        lo.copy_from_slice(&bytes[..8]);
        hi.copy_from_slice(&bytes[8..]);
        cells[row * 4 + 2] = u64::from_ne_bytes(lo);
        cells[row * 4 + 3] = u64::from_ne_bytes(hi);
    }

    let dir = tempfile::tempdir()?;
    let path = dir.path().join("row_major.odb");
    odc::write_odb_row_major(&cells, &columns, &path, &WriteOptions::default())?;

    let out = odc::read_odb_single(&path, &ReadOptions::default())?;
    let expected = df!(
        "i" => [Some(1_i64), None, Some(3)],
        "f" => [Some(0.5_f64), None, Some(2.5)],
        "s" => ["one", "twotwotwotwo", ""],
    )?;
    assert!(
        out.equals_missing(&expected),
        "expected:\n{expected}\ngot:\n{out}"
    );
    Ok(())
}

#[test]
fn row_major_invalid_input() -> odc::Result<()> {
    let columns = [
        CellColumn {
            name: "i",
            column_type: ColumnType::Integer,
            size: 8,
        },
        CellColumn {
            name: "f",
            column_type: ColumnType::Double,
            size: 8,
        },
    ];
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("unused.odb");

    let cells = [0_u64; 3];
    let result = odc::write_odb_row_major(&cells, &columns, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidCellLayout(_))));

    let bad_size = [CellColumn {
        name: "i",
        column_type: ColumnType::Integer,
        size: 16,
    }];
    let result = odc::write_odb_row_major(&cells, &bad_size, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let result = odc::write_odb_row_major(&[], &columns, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::EmptyDataFrame)));
    Ok(())
}

#[test]
fn strided_roundtrip() -> odc::Result<()> {
    let ints = [1_i64, odc::integer_missing_value(), 3];
    let floats = [0.5_f64, odc::double_missing_value(), 2.5];
    let strings = ["one", "twotwotwotwo", ""];

    // Column-major cells: ints, floats, then 16-byte strings.
    let mut cells = vec![0_u64; 12];
    for row in 0..3 {
        cells[row] = u64::from_ne_bytes(ints[row].to_ne_bytes());
        cells[3 + row] = floats[row].to_bits();
        let mut bytes = [0_u8; 16];
        bytes[..strings[row].len()].copy_from_slice(strings[row].as_bytes());
        let mut lo = [0_u8; 8];
        let mut hi = [0_u8; 8];
        lo.copy_from_slice(&bytes[..8]);
        hi.copy_from_slice(&bytes[8..]);
        cells[6 + row * 2] = u64::from_ne_bytes(lo);
        cells[7 + row * 2] = u64::from_ne_bytes(hi);
    }
    let columns = [
        StridedColumn {
            name: "i",
            column_type: ColumnType::Integer,
            size: 8,
            offset: 0,
            stride: 8,
        },
        StridedColumn {
            name: "f",
            column_type: ColumnType::Double,
            size: 8,
            offset: 24,
            stride: 8,
        },
        StridedColumn {
            name: "s",
            column_type: ColumnType::String,
            size: 16,
            offset: 48,
            stride: 16,
        },
    ];

    let dir = tempfile::tempdir()?;
    let path = dir.path().join("strided.odb");
    odc::write_odb_strided(&cells, &columns, 3, &path, &WriteOptions::default())?;

    let out = odc::read_odb_single(&path, &ReadOptions::default())?;
    let expected = df!(
        "i" => [Some(1_i64), None, Some(3)],
        "f" => [Some(0.5_f64), None, Some(2.5)],
        "s" => ["one", "twotwotwotwo", ""],
    )?;
    assert!(
        out.equals_missing(&expected),
        "expected:\n{expected}\ngot:\n{out}"
    );

    // Decode back into a row-major layout and compare the raw cells.
    let reader = odc::Reader::from_path(&path)?;
    let frames: Vec<odc::Frame> = reader.frames().collect::<odc::Result<_>>()?;
    let out_columns = [
        StridedColumn {
            name: "i",
            column_type: ColumnType::Integer,
            size: 8,
            offset: 0,
            stride: 32,
        },
        StridedColumn {
            name: "f",
            column_type: ColumnType::Double,
            size: 8,
            offset: 8,
            stride: 32,
        },
        StridedColumn {
            name: "s",
            column_type: ColumnType::String,
            size: 16,
            offset: 16,
            stride: 32,
        },
    ];
    let mut out_cells = vec![0_u64; 12];
    let rows = frames[0].decode_strided(&mut out_cells, &out_columns, 1)?;
    assert_eq!(rows, 3);
    for row in 0..3 {
        assert_eq!(out_cells[row * 4], cells[row]);
        assert_eq!(out_cells[row * 4 + 1], cells[3 + row]);
        assert_eq!(out_cells[row * 4 + 2], cells[6 + row * 2]);
        assert_eq!(out_cells[row * 4 + 3], cells[7 + row * 2]);
    }
    Ok(())
}

#[test]
fn strided_invalid_input() -> odc::Result<()> {
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("unused.odb");
    let cells = [0_u64; 4];

    let short_stride = [StridedColumn {
        name: "s",
        column_type: ColumnType::String,
        size: 16,
        offset: 0,
        stride: 8,
    }];
    let result = odc::write_odb_strided(&cells, &short_stride, 2, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let overflowing = [StridedColumn {
        name: "i",
        column_type: ColumnType::Integer,
        size: 8,
        offset: 0,
        stride: 16,
    }];
    let result = odc::write_odb_strided(&cells, &overflowing, 3, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::InvalidBuffer { .. })));

    let result = odc::write_odb_strided(&cells, &overflowing, 0, &path, &WriteOptions::default());
    assert!(matches!(result, Err(Error::EmptyDataFrame)));
    Ok(())
}

#[test]
fn cell_layout_roundtrip() -> odc::Result<()> {
    let ints = [1_i64, 2, 3];
    let floats = [0.5_f64, 1.5, 2.5];

    // Column-major cells: three ints, then three doubles.
    let mut cells = vec![0_u64; 6];
    for row in 0..3 {
        cells[row] = u64::from_ne_bytes(ints[row].to_ne_bytes());
        cells[3 + row] = floats[row].to_bits();
    }
    let columns = [
        CellColumn {
            name: "i",
            column_type: ColumnType::Integer,
            size: 8,
        },
        CellColumn {
            name: "f",
            column_type: ColumnType::Double,
            size: 8,
        },
    ];

    let dir = tempfile::tempdir()?;
    let path = dir.path().join("cells.odb");
    odc::write_odb_column_major(&cells, &columns, &path, &WriteOptions::default())?;

    let out = odc::read_odb_single(&path, &ReadOptions::default())?;
    let expected = df!("i" => ints, "f" => floats)?;
    assert!(
        out.equals_missing(&expected),
        "expected:\n{expected}\ngot:\n{out}"
    );

    let reader = odc::Reader::from_path(&path)?;
    let frames: Vec<odc::Frame> = reader.frames().collect::<odc::Result<_>>()?;

    let mut back = vec![0_u64; 6];
    frames[0].decode_column_major(&mut back, &columns, 1)?;
    assert_eq!(back, cells);

    let mut rows = vec![0_u64; 6];
    frames[0].decode_row_major(&mut rows, &columns, 1)?;
    for row in 0..3 {
        assert_eq!(rows[row * 2], cells[row]);
        assert_eq!(rows[row * 2 + 1], cells[3 + row]);
    }
    Ok(())
}
