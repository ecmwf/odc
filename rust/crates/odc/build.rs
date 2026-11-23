fn main() {
    // Rpaths for this crate's own test binaries; apps call emit_rpaths() themselves.
    bindman_utils::emit_rpaths();
}
