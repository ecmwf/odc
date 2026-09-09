//! Reading ODB-2 data: [`Reader`] and its frame iterator.

use std::path::Path;
use std::sync::Arc;

use parking_lot::Mutex;

use crate::error::{Error, Result};
use crate::frame::Frame;
use crate::init;

/// Options for opening a [`Reader`].
#[derive(Debug, Clone)]
pub struct ReaderOptions {
    /// Aggregate consecutive compatible physical frames into logical frames.
    pub aggregated: bool,
    /// Maximum number of rows to aggregate into one logical frame.
    pub row_limit: Option<i64>,
}

impl Default for ReaderOptions {
    fn default() -> Self {
        Self {
            aggregated: true,
            row_limit: None,
        }
    }
}

/// Shared reader state. Frames decode lazily from the reader's stream, so
/// every [`Frame`] holds an `Arc` of this to keep the stream (and the source
/// handle) alive.
pub struct ReaderShared {
    // Field order matters: the C++ reader borrows the source handle, so
    // `inner` must drop before `_source`.
    pub(crate) inner: Mutex<odc_sys::UniquePtr<odc_sys::ReaderWrapper>>,
    _source: Option<Mutex<eckit::DataHandle<eckit::Closed>>>,
}

/// Owns an ODB-2 data stream and yields its [`Frame`]s.
///
/// # Thread safety
///
/// `Reader` is `Send + Sync`; stream access is serialized through a mutex.
/// Frames may be decoded from other threads while the reader advances.
pub struct Reader {
    shared: Arc<ReaderShared>,
}

impl Reader {
    /// Open an ODB-2 file with default options.
    ///
    /// # Errors
    ///
    /// Fails if the file cannot be opened or is not valid ODB-2.
    pub fn from_path(path: impl AsRef<Path>) -> Result<Self> {
        Self::from_path_with(path, &ReaderOptions::default())
    }

    /// Open an ODB-2 file.
    ///
    /// # Errors
    ///
    /// Fails if the file cannot be opened or is not valid ODB-2.
    pub fn from_path_with(path: impl AsRef<Path>, options: &ReaderOptions) -> Result<Self> {
        init();
        let path = path.as_ref().to_str().ok_or_else(|| {
            Error::Io(std::io::Error::new(
                std::io::ErrorKind::InvalidInput,
                "path is not valid UTF-8",
            ))
        })?;
        let inner = odc_sys::ReaderWrapper::from_path(
            path,
            options.aggregated,
            options.row_limit.unwrap_or(-1),
        )?;
        Ok(Self {
            shared: Arc::new(ReaderShared {
                inner: Mutex::new(inner),
                _source: None,
            }),
        })
    }

    /// Read ODB-2 data from an eckit [`DataHandle`](eckit::DataHandle)
    /// (file, buffer, multi-file, byte range, …).
    ///
    /// The handle must not be open — the reader opens it for reading and it
    /// stays owned by the reader for its whole lifetime.
    ///
    /// # Errors
    ///
    /// Fails if the handle cannot be opened or is not valid ODB-2.
    pub fn from_handle(
        mut handle: eckit::DataHandle<eckit::Closed>,
        options: &ReaderOptions,
    ) -> Result<Self> {
        init();
        let inner = odc_sys::ReaderWrapper::from_handle(
            handle.as_sys_mut()?,
            options.aggregated,
            options.row_limit.unwrap_or(-1),
        )?;
        Ok(Self {
            shared: Arc::new(ReaderShared {
                inner: Mutex::new(inner),
                _source: Some(Mutex::new(handle)),
            }),
        })
    }

    /// Iterator over the frames of the stream.
    ///
    /// The iterator advances the underlying stream: each frame is yielded
    /// once, and a second `frames()` call continues where the first stopped.
    #[must_use]
    pub fn frames(&self) -> Frames {
        Frames {
            shared: Arc::clone(&self.shared),
            exhausted: false,
        }
    }
}

/// Iterator over the [`Frame`]s of a [`Reader`].
pub struct Frames {
    shared: Arc<ReaderShared>,
    exhausted: bool,
}

impl Iterator for Frames {
    type Item = Result<Frame>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.exhausted {
            return None;
        }
        let next = self.shared.inner.lock().pin_mut().next_frame();
        match next {
            Err(e) => {
                self.exhausted = true;
                Some(Err(e.into()))
            }
            Ok(ptr) if ptr.is_null() => {
                self.exhausted = true;
                None
            }
            Ok(ptr) => match Frame::new(ptr, Arc::clone(&self.shared)) {
                Ok(frame) => Some(Ok(frame)),
                Err(e) => {
                    self.exhausted = true;
                    Some(Err(e))
                }
            },
        }
    }
}
