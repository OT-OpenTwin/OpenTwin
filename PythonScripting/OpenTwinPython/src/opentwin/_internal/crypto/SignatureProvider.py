# @otlicense

"""Provides cryptographic signature operations."""

# External dependencies
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey

class SignatureProvider:
    """Provides Ed25519 signing using DER-encoded PKCS#8 private keys.

    This class performs the cryptographic operation independently of the
    mechanism used to obtain the private key.
    """

    @staticmethod
    def sign(private_key_data: bytes, data: bytes) -> bytes:
        """Create an Ed25519 signature.

        Args:
            private_key_data: Unencrypted DER-encoded PKCS#8 private key.
            data: Data to sign.

        Returns:
            The 64-byte Ed25519 signature.

        Raises:
            TypeError: If either argument is not bytes.
            ValueError: If the key cannot be decoded or is not an Ed25519 key.
        """
        if not isinstance(private_key_data, bytes):
            raise TypeError("Private key data must be bytes.")

        if not isinstance(data, bytes):
            raise TypeError("Data must be bytes.")

        private_key = serialization.load_der_private_key(
            private_key_data,
            password=None,
        )

        if not isinstance(private_key, Ed25519PrivateKey):
            raise ValueError("Private key is not an Ed25519 key.")

        return private_key.sign(data)
