# @otlicense

class OpenTwinError(Exception):
    """Base exception for all OpenTwin API errors."""

################################################################################################################################

# Authentication errors

class AuthenticationError(OpenTwinError):
    pass

class InvalidCredentialsError(AuthenticationError):
    pass

################################################################################################################################

# Communication errors

class CommunicationError(OpenTwinError):
    pass

class CommunicationTimeoutError(CommunicationError):
    pass

class InvalidResponseError(CommunicationError):
    pass

class InvalidRequestError(CommunicationError):
    pass

class ConnectionError(CommunicationError):
    pass

################################################################################################################################

# Project errors

class ProjectError(OpenTwinError):
    pass


class ProjectNotFoundError(ProjectError):
    pass

################################################################################################################################
