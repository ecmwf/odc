use std::env;
use std::path::{Path, PathBuf};

const ODC_VERSION: &str = "1.6.3";

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=cpp/OdcBridge.h");
    println!("cargo:rerun-if-changed=cpp/OdcBridge.cc");
    println!("cargo:rerun-if-env-changed=ODC_DIR");
    println!("cargo:rerun-if-env-changed=DOCS_RS");

    if bindman_utils::is_docs_rs() {
        return;
    }

    bindman_utils::validate_build_mode(cfg!(feature = "system"), cfg!(feature = "vendored"));

    if cfg!(feature = "system") {
        build_system();
    } else {
        build_vendored();
    }
}

/// Generate `odc_exceptions.{h,rs}` covering odc's own subclasses
/// (`ODBDecodeError` + its subclasses, via recursive walk in `odc/core/Exceptions.h`)
/// plus eckit's exceptions inherited from eckit-sys.
fn generate_exceptions(include: &Path) {
    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));

    let own = vec![bindman_build::ExceptionSource {
        header: include.join("odc/core/Exceptions.h"),
        include_path: "odc/core/Exceptions.h".to_string(),
        cpp_namespace: "odc::core".to_string(),
        message_prefix: "odc".to_string(),
        base_class: "eckit::Exception".to_string(),
        recursive: true,
    }];

    let inherited = bindman_build::collect_dep_exception_sources();

    bindman_build::generate_exception_bridge(&bindman_build::ExceptionBridgeConfig {
        primary_namespace: "odc",
        out_dir: &out_dir,
        own: &own,
        inherited: &inherited,
    });

    bindman_build::publish_exception_sources(&own, &out_dir);
}

#[cfg(feature = "system")]
fn build_system() {
    let crate_dir =
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set"));
    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));

    let eckit_include = env::var("DEP_ECKIT_SYS_INCLUDE").expect("DEP_ECKIT_SYS_INCLUDE not set");
    let eckit_cpp_dir = env::var("DEP_ECKIT_SYS_CPP_DIR").expect("DEP_ECKIT_SYS_CPP_DIR not set");

    let (root, odc_include, lib_dir) = bindman_utils::cmake_find_package("odc", ODC_VERSION);

    generate_exceptions(&odc_include);

    println!("cargo:rustc-link-search=native={}", lib_dir.display());
    println!("cargo:rustc-link-lib=dylib=odccore");

    cxx_build::bridge("src/lib.rs")
        .file(crate_dir.join("cpp/OdcBridge.cc"))
        .include(&odc_include)
        .include(&eckit_include)
        .include(&eckit_cpp_dir)
        .include(crate_dir.join("cpp"))
        .include(&out_dir) // for odc_exceptions.h (generated)
        .flag_if_supported("-std=c++17")
        .compile("odc_sys_bridge");

    bindman_utils::link_cpp_stdlib();

    println!("cargo:root={}", root.display());
    println!("cargo:include={}", odc_include.display());

    bindman_build::check_cpp_api(&odc_include, &crate_dir.join("src/lib.rs"));
}

#[cfg(not(feature = "system"))]
fn build_system() {
    unreachable!("build_system called without system feature");
}

#[cfg(feature = "vendored")]
fn build_vendored() {
    use std::fs;
    use std::process::Command;

    const ECBUILD_REPO: &str = "https://github.com/ecmwf/ecbuild.git";
    const ECBUILD_TAG: &str = "3.13.1";
    const ODC_REPO: &str = "https://github.com/ecmwf/odc.git";

    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));
    let src_dir = out_dir.join("src");
    let build_dir = out_dir.join("build");
    let install_dir = out_dir.join("install");

    fs::create_dir_all(&src_dir).expect("Failed to create src directory");
    fs::create_dir_all(&build_dir).expect("Failed to create build directory");

    let eckit_root = env::var("DEP_ECKIT_SYS_ROOT").expect("DEP_ECKIT_SYS_ROOT not set");
    let eckit_cpp_dir = env::var("DEP_ECKIT_SYS_CPP_DIR").expect("DEP_ECKIT_SYS_CPP_DIR not set");

    let ecbuild_src = bindman_utils::git_clone(ECBUILD_REPO, ECBUILD_TAG, &src_dir.join("ecbuild"));
    let odc_src = bindman_utils::git_clone(ODC_REPO, ODC_VERSION, &src_dir.join("odc"));

    let ecbuild_bin = ecbuild_src.join("bin/ecbuild");
    let num_jobs = bindman_utils::build_parallelism();

    let cmake_prefix_path = eckit_root.clone();

    let mut cmd = Command::new(&ecbuild_bin);
    cmd.current_dir(&build_dir)
        .arg(format!("--prefix={}", install_dir.display()))
        .arg("--")
        .arg(&odc_src)
        .arg(format!("-DCMAKE_PREFIX_PATH={cmake_prefix_path}"))
        .arg(format!(
            "-DCMAKE_BUILD_TYPE={}",
            bindman_utils::cmake_build_type()
        ))
        .arg("-DENABLE_TESTS=OFF")
        .arg("-DBUILD_TESTING=OFF")
        .arg("-DENABLE_DOCS=OFF")
        .arg("-DENABLE_FORTRAN=OFF")
        .arg("-DENABLE_PYTHON=OFF");

    #[cfg(target_os = "macos")]
    cmd.arg("-DCMAKE_INSTALL_NAME_DIR=@rpath");

    bindman_utils::run_command(&mut cmd, "ecbuild configure odc");

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--build", ".", "--parallel", &num_jobs])
            .current_dir(&build_dir),
        "cmake build odc",
    );

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--install", "."])
            .current_dir(&build_dir),
        "cmake install odc",
    );

    let include_dir = install_dir.join("include");
    let crate_dir =
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set"));
    let lib_dir = bindman_utils::resolve_lib_dir(&install_dir);

    generate_exceptions(&include_dir);

    cxx_build::bridge("src/lib.rs")
        .file(crate_dir.join("cpp/OdcBridge.cc"))
        .include(&include_dir)
        .include(format!("{eckit_root}/include"))
        .include(&eckit_cpp_dir)
        .include(crate_dir.join("cpp"))
        .include(&out_dir) // for odc_exceptions.h (generated)
        .flag_if_supported("-std=c++17")
        .compile("odc_sys_bridge");

    println!("cargo:rustc-link-search=native={}", lib_dir.display());
    println!("cargo:rustc-link-lib=dylib=odccore");
    bindman_utils::link_cpp_stdlib();

    println!("cargo:root={}", install_dir.display());
    println!("cargo:include={}", include_dir.display());

    bindman_build::check_cpp_api(&include_dir, &crate_dir.join("src/lib.rs"));
}

#[cfg(not(feature = "vendored"))]
fn build_vendored() {
    unreachable!("build_vendored called without vendored feature");
}
