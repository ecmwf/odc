//! End-to-end write→read round-trips across dtypes, missing-value
//! patterns and encoding options.

use odc::polars::prelude::*;
use odc::{Bit, ColumnType, Error, ReadOptions, ReaderOptions, WriteOptions};
use std::path::PathBuf;
use tempfile::TempDir;

fn write(df: &DataFrame, options: &WriteOptions) -> odc::Result<(TempDir, PathBuf)> {
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("roundtrip.odb");
    odc::write_odb(df, &path, options)?;
    Ok((dir, path))
}

fn roundtrip_with(
    df: &DataFrame,
    options: &WriteOptions,
    read: &ReadOptions,
) -> odc::Result<DataFrame> {
    let (_dir, path) = write(df, options)?;
    odc::read_odb_single(&path, read)
}

fn roundtrip(df: &DataFrame) -> odc::Result<DataFrame> {
    roundtrip_with(df, &WriteOptions::default(), &ReadOptions::default())
}

fn assert_same(actual: &DataFrame, expected: &DataFrame) {
    assert!(
        actual.equals_missing(expected),
        "expected:\n{expected}\ngot:\n{actual}"
    );
}

#[test]
fn int64_with_nulls() -> odc::Result<()> {
    let df = df!("x" => [Some(1_i64), None, Some(-5), Some(1_000_000)])?;
    assert_same(&roundtrip(&df)?, &df);
    Ok(())
}

#[test]
fn int64_without_nulls() -> odc::Result<()> {
    let df = df!("x" => [1_i64, 2, 3], "y" => [1.5_f64, -2.5, 0.0])?;
    assert_same(&roundtrip(&df)?, &df);
    Ok(())
}

#[test]
fn smaller_integers_and_booleans_widen_to_int64() -> odc::Result<()> {
    let df = df!(
        "i32" => [Some(-320_000_i32), None, Some(7)],
        "u32" => [Some(320_000_u32), None, Some(7)],
        "bool" => [Some(true), None, Some(false)],
    )?;
    let expected = df!(
        "i32" => [Some(-320_000_i64), None, Some(7)],
        "u32" => [Some(320_000_i64), None, Some(7)],
        "bool" => [Some(1_i64), None, Some(0)],
    )?;
    assert_same(&roundtrip(&df)?, &expected);
    Ok(())
}

#[test]
fn float64_with_nulls() -> odc::Result<()> {
    let df = df!("x" => [Some(274.5_f64), None, Some(-1.25), Some(f64::MIN_POSITIVE)])?;
    assert_same(&roundtrip(&df)?, &df);
    Ok(())
}

#[test]
fn float32_roundtrips_as_real() -> odc::Result<()> {
    let df = df!("x" => [Some(1.5_f32), None, Some(-3.25)])?;
    let out = roundtrip(&df)?;
    assert_eq!(out.column("x")?.dtype(), &DataType::Float32);
    assert_same(&out, &df);
    Ok(())
}

#[test]
fn all_null_columns() -> odc::Result<()> {
    let df = df!(
        "i" => [None::<i64>, None, None],
        "f" => [None::<f64>, None, None],
    )?;
    assert_same(&roundtrip(&df)?, &df);
    Ok(())
}

#[test]
fn nulls_in_first_and_last_row() -> odc::Result<()> {
    let df = df!(
        "i" => [None, Some(2_i64), None],
        "f" => [None, Some(2.5_f64), None],
    )?;
    assert_same(&roundtrip(&df)?, &df);
    Ok(())
}

// The ODB sentinel values are indistinguishable from missing data once
// encoded, so writing one as a real value reads back as null.
#[test]
fn integer_sentinel_value_reads_back_as_null() -> odc::Result<()> {
    let df = df!("x" => [odc::integer_missing_value(), 1])?;
    let expected = df!("x" => [None, Some(1_i64)])?;
    assert_same(&roundtrip(&df)?, &expected);
    Ok(())
}

#[test]
fn double_sentinel_value_reads_back_as_null() -> odc::Result<()> {
    let df = df!("x" => [odc::double_missing_value(), 1.5])?;
    let expected = df!("x" => [None, Some(1.5_f64)])?;
    assert_same(&roundtrip(&df)?, &expected);
    Ok(())
}

// Strings have no missing-value sentinel: nulls encode as empty strings.
#[test]
fn strings_widths_and_nulls() -> odc::Result<()> {
    let df = df!(
        "s" => [Some("seven77"), Some("eight888"), Some("nine99999"), Some("héllo"), Some(""), None],
    )?;
    let expected = df!(
        "s" => ["seven77", "eight888", "nine99999", "héllo", "", ""],
    )?;
    assert_same(&roundtrip(&df)?, &expected);
    Ok(())
}

#[test]
fn bitfield_override_roundtrip() -> odc::Result<()> {
    let df = df!("flags" => [0b01_i64, 0b11, 0b10])?;
    let mut options = WriteOptions::default();
    options.types.insert("flags".into(), ColumnType::Bitfield);
    options.bitfields.insert(
        "flags".into(),
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

    let (_dir, path) = write(&df, &options)?;
    let reader = odc::Reader::from_path(&path)?;
    let frame = reader.frames().next().expect("stream yields one frame")?;
    let column = frame.column("flags").expect("column exists");
    assert_eq!(column.column_type, ColumnType::Bitfield);
    assert_same(&frame.dataframe()?, &df);
    Ok(())
}

#[test]
fn double_to_real_override() -> odc::Result<()> {
    let df = df!("x" => [Some(1.5_f64), None])?;
    let mut options = WriteOptions::default();
    options.types.insert("x".into(), ColumnType::Real);
    let out = roundtrip_with(&df, &options, &ReadOptions::default())?;
    let expected = df!("x" => [Some(1.5_f32), None])?;
    assert_same(&out, &expected);
    Ok(())
}

#[test]
fn properties_roundtrip() -> odc::Result<()> {
    let df = df!("x" => [1_i64])?;
    let mut options = WriteOptions::default();
    options
        .properties
        .insert("source".into(), "roundtrip test".into());

    let (_dir, path) = write(&df, &options)?;
    let reader = odc::Reader::from_path(&path)?;
    let frame = reader.frames().next().expect("stream yields one frame")?;
    assert_eq!(
        frame.properties().get("source").map(String::as_str),
        Some("roundtrip test")
    );
    Ok(())
}

#[test]
fn rows_per_frame_splits_into_physical_frames() -> odc::Result<()> {
    let df = df!("x" => [1_i64, 2, 3, 4, 5])?;
    let options = WriteOptions {
        rows_per_frame: 2,
        ..Default::default()
    };

    let (_dir, path) = write(&df, &options)?;
    let reader = odc::Reader::from_path_with(
        &path,
        &ReaderOptions {
            aggregated: false,
            row_limit: None,
        },
    )?;
    let rows: Vec<usize> = reader
        .frames()
        .map(|frame| Ok(frame?.row_count()))
        .collect::<odc::Result<_>>()?;
    assert_eq!(rows, [2, 2, 1]);

    assert_same(&odc::read_odb_single(&path, &ReadOptions::default())?, &df);
    Ok(())
}

#[test]
fn column_selection_and_order() -> odc::Result<()> {
    let df = df!("a" => [1_i64], "b" => [2.5_f64], "c" => ["x"])?;
    let read = ReadOptions {
        columns: Some(vec!["c".into(), "a".into()]),
        ..Default::default()
    };
    let out = roundtrip_with(&df, &WriteOptions::default(), &read)?;
    assert_same(&out, &df!("c" => ["x"], "a" => [1_i64])?);
    Ok(())
}

#[test]
fn empty_dataframe_is_rejected() -> odc::Result<()> {
    let df = df!("x" => Vec::<i64>::new())?;
    let result = roundtrip(&df);
    assert!(matches!(result, Err(Error::EmptyDataFrame)));
    Ok(())
}

#[test]
fn unsupported_dtype_is_rejected() -> odc::Result<()> {
    let df = df!("x" => [1_u64, 2])?;
    let result = roundtrip(&df);
    assert!(matches!(
        result,
        Err(Error::UnsupportedDtype { column, .. }) if column == "x"
    ));
    Ok(())
}

#[test]
fn bitfield_override_without_bits_is_rejected() -> odc::Result<()> {
    let df = df!("x" => [1_i64])?;
    let mut options = WriteOptions::default();
    options.types.insert("x".into(), ColumnType::Bitfield);
    let result = roundtrip_with(&df, &options, &ReadOptions::default());
    assert!(matches!(result, Err(Error::InvalidBitfield(column)) if column == "x"));
    Ok(())
}

#[test]
fn incompatible_type_override_is_rejected() -> odc::Result<()> {
    let df = df!("x" => ["s"])?;
    let mut options = WriteOptions::default();
    options.types.insert("x".into(), ColumnType::Integer);
    let result = roundtrip_with(&df, &options, &ReadOptions::default());
    assert!(matches!(
        result,
        Err(Error::InvalidTypeOverride { column, .. }) if column == "x"
    ));
    Ok(())
}

#[test]
fn unknown_column_selection_is_rejected() -> odc::Result<()> {
    let df = df!("x" => [1_i64])?;
    let read = ReadOptions {
        columns: Some(vec!["nope".into()]),
        ..Default::default()
    };
    let result = roundtrip_with(&df, &WriteOptions::default(), &read);
    assert!(matches!(result, Err(Error::ColumnNotFound(column)) if column == "nope"));
    Ok(())
}

#[test]
fn multithreaded_decode_of_integer_columns() -> odc::Result<()> {
    let values: Vec<i64> = (0..1000).map(|i| i - 500).collect();
    let df = df!("x" => &values)?;
    let options = WriteOptions {
        rows_per_frame: 100,
        ..Default::default()
    };

    let (_dir, path) = write(&df, &options)?;
    let reader = odc::Reader::from_path(&path)?;
    let frame = reader.frames().next().expect("stream yields one frame")?;
    let out = frame.dataframe_with(&odc::DecodeOptions {
        columns: None,
        threads: 4,
    })?;
    assert_same(&out, &df);
    Ok(())
}
