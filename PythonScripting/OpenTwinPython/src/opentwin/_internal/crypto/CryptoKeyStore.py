# @otlicense

"""Defines the abstract interface for signing operations."""

# Python standard
from abc import ABC, abstractmethod


class CryptoKeyStore(ABC):
    """Abstract interface for a signing-capable cryptographic key store.

    Implementations are responsible for signing data with the key identified
    by ``key_id``. The private-key material and the mechanism used to perform
    the signature operation remain implementation details.
    """

    @abstractmethod
    def sign(self, key_id: str, data: bytes) -> bytes:
        """Sign data using the key identified by ``key_id``.

        Args:
            key_id: Hexadecimal identifier of the key to use.
            data: Data to sign.

        Returns:
            The resulting signature as bytes.

        Raises:
            ValueError: If the key identifier or input data is invalid.
            FileNotFoundError: If the requested key cannot be found.
            RuntimeError: If the signing operation fails.
        """
        raise NotImplementedError
