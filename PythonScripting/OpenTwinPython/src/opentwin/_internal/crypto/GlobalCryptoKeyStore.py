# @otlicense

"""Provides process-wide access to the configured cryptographic key store."""

# Python standard
from threading import Lock

# OpenTwin internal
from opentwin._internal.crypto.CryptoKeyStore import CryptoKeyStore
from opentwin._internal.crypto.CryptoKeyStoreFileSystem import CryptoKeyStoreFileSystem

class GlobalCryptoKeyStore:
    """Provides lazy, thread-safe access to one key-store implementation.

    The concrete implementation is selected in ``get()``. Consumers depend
    only on the ``CryptoKeyStore`` interface, allowing the implementation to
    be replaced centrally without changing calling code.
    """

    _instance = None
    _lock = Lock()

    @classmethod
    def get(cls) -> CryptoKeyStore:
        """Return the process-wide key-store instance.

        The configured implementation is constructed on the first call.
        Subsequent calls return the same instance.

        Returns:
            The configured ``CryptoKeyStore`` implementation.
        """
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = CryptoKeyStoreFileSystem()

        return cls._instance
