# @otlicense

"""Implements a local file-system-backed cryptographic key store."""

# Python standard
from pathlib import Path

# OpenTwin internal
from opentwin._internal.system.DirectoryManager import DirectoryManager
from opentwin._internal.crypto.CryptoKeyStore import CryptoKeyStore
from opentwin._internal.crypto.SignatureProvider import SignatureProvider

class CryptoKeyStoreFileSystem(CryptoKeyStore):
    
    def __init__(self) -> None:
        self._root_directory = DirectoryManager.get_local_user_data_directory() / "CryptoKeyStore"

    def sign(self, key_id: str, data: bytes) -> bytes:
        if not isinstance(key_id, str):
            raise TypeError("Key ID must be a string.")

        if not key_id:
            raise ValueError("Key ID must be a non-empty string.")

        if not isinstance(data, bytes):
            raise TypeError("Data must be bytes.")

        if not data:
            raise ValueError("Data must be a non-empty bytes object.")

        keyPath = self._getKeyPath(key_id)

        private_key_path = keyPath / "private.key"
        private_key_data = private_key_path.read_bytes()

        return SignatureProvider.sign(private_key_data, data)

    def _getKeyPath(self, key_id: str) -> Path:
        return self._root_directory / (key_id.encode("utf-8").hex())


