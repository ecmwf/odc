# Changelog for odc

## 1.6.0

* Add a new LongConstantString codec which permits encoding constant columns where the constant is a string > 8 characters in length.
    * This saves 1 byte per row compared the previous way these columns were encoded.
    * A python implementation has been added to pyodc at the same time.
    * Decoding data using this codec will work straight away.
    * Encoding data with the new codec is disabled by default and can be enabled with the environment variable "ODC_ENABLE_WRITING_LONG_STRING_CODEC=1".
    * At some point in a future release, encoding will be enabled by default.