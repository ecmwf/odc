#[test]
fn crate_version_matches_repo_version_file() {
    let path = concat!(env!("CARGO_MANIFEST_DIR"), "/../../../VERSION");
    let Ok(version) = std::fs::read_to_string(path) else {
        return;
    };
    assert_eq!(
        version.trim(),
        env!("CARGO_PKG_VERSION"),
        "rust/crates/odc-sys/Cargo.toml version and VERSION file are out of sync"
    );
}
