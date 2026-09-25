//! Span access: `Frame::span`.

use odc::polars::prelude::*;
use odc::{ReaderOptions, WriteOptions};
use std::path::{Path, PathBuf};
use tempfile::TempDir;

fn write_stream(frames: &[DataFrame]) -> odc::Result<(TempDir, PathBuf)> {
    let dir = tempfile::tempdir()?;
    let path = dir.path().join("span.odb");
    let mut bytes = Vec::new();
    for (i, df) in frames.iter().enumerate() {
        let part = dir.path().join(format!("part{i}.odb"));
        odc::write_odb(df, &part, &WriteOptions::default())?;
        bytes.extend(std::fs::read(&part)?);
    }
    std::fs::write(&path, bytes)?;
    Ok((dir, path))
}

fn frames(path: &Path) -> odc::Result<Vec<odc::Frame>> {
    let options = ReaderOptions {
        aggregated: false,
        row_limit: None,
    };
    odc::Reader::from_path_with(path, &options)?
        .frames()
        .collect()
}

#[test]
fn span_values_and_equality() -> odc::Result<()> {
    let (_dir, path) = write_stream(&[
        df!(
            "key1" => [1_i64, 1, 1],
            "key3" => ["foo", "foo", "foo"],
            "val1" => [10.5_f64, 20.5, 30.5],
        )?,
        df!(
            "key1" => [1_i64, 1, 1],
            "key3" => ["foo", "foo", "foo"],
            "val1" => [40.5_f64, 50.5, 60.5],
        )?,
        df!(
            "key1" => [2_i64, 2, 2],
            "key3" => ["bar", "bar", "bar"],
            "val1" => [70.5_f64, 80.5, 90.5],
        )?,
    ])?;

    let frames = frames(&path)?;
    let spans: Vec<odc::Span> = frames
        .iter()
        .map(|frame| frame.span(&["key1", "key3"], true))
        .collect::<odc::Result<_>>()?;

    assert_eq!(spans[0].integer_values("key1")?, [1]);
    assert_eq!(spans[0].string_values("key3")?, ["foo"]);
    assert_eq!(spans[2].integer_values("key1")?, [2]);
    assert_eq!(spans[2].string_values("key3")?, ["bar"]);

    assert!(spans[0] == spans[1]);
    assert!(spans[0] != spans[2]);

    assert_eq!(spans[0].offset(), 0);
    assert!(spans[0].length() > 0);
    assert_eq!(spans[1].offset(), spans[0].length());
    Ok(())
}

#[test]
fn span_non_constant_column() -> odc::Result<()> {
    let (_dir, path) = write_stream(&[df!(
        "key1" => [1_i64, 1, 1],
        "val1" => [30.5_f64, 10.5, 20.5],
    )?])?;

    let frames = frames(&path)?;
    assert!(frames[0].span(&["val1"], true).is_err());

    let span = frames[0].span(&["val1"], false)?;
    assert_eq!(span.real_values("val1")?, [10.5, 20.5, 30.5]);
    assert!(span.integer_values("val1").is_err());

    assert!(frames[0].span(&["nope"], false).is_err());
    Ok(())
}
