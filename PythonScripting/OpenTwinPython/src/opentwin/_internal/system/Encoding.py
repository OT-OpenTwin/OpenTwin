# @otlicense

import base64

class Encoding:

    @staticmethod
    def toBase64Url(data: bytes) -> str:
        """Encode bytes using Base64URL without padding.

        Args:
            data: The bytes to encode.

        Returns:
            The Base64URL-encoded string without padding.
        """
        return base64.urlsafe_b64encode(data).rstrip(b"=").decode("ascii")

    @staticmethod
    def bytesFromBase64Url(encoded: str) -> bytes:
        """Decode a Base64URL string without padding.

        Args:
            encoded: The Base64URL-encoded string.

        Returns:
            The decoded bytes.

        Raises:
            ValueError: If the input is not a valid canonical Base64URL string.
        """
        if len(encoded) % 4 == 1:
            raise ValueError("Invalid Base64Url string length")

        try:
            # Restore padding required by Python's decoder.
            padded = encoded + "=" * (-len(encoded) % 4)

            decoded = base64.b64decode(padded, altchars=b"-_", validate=True)

            # Ensure canonical encoding, including valid trailing bits.
            if Encoding.toBase64Url(decoded) != encoded:
                raise ValueError("Invalid Base64Url encoding")

            return decoded

        except (ValueError, base64.binascii.Error) as exc:
            raise ValueError("Invalid Base64Url string") from exc