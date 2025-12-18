from typing import Any


def GetPropertyValue(
    absoluteEntityName: str,
    propertyName: str
) -> Any:
    """
    Get the value of a requested property from a requested entity.
    """
    ...


def GetTableCellValue(
    absoluteEntityName: str,
    row: int,
    column: int
) -> Any:
    """
    Get the value of a cell from a requested entity.
    Row and column must be >= 0.
    """
    ...


def SetPropertyValue(
    absoluteEntityName: str,
    propertyName: str,
    value: Any
) -> bool:
    """
    Set the property value of a requested property from a requested entity.
    Returns True on success.
    """
    ...


def Flush() -> bool:
    """
    Apply all changes on entity properties.
    Returns True on success.
    """
    ...


def FlushEntity(
    absoluteEntityName: str
) -> bool:
    """
    Apply all changes on the requested entity.
    Returns True on success.
    """
    ...


def GetPythonScript(
    absoluteScriptName: str
) -> Any:
    """
    Executes a Python script stored within OpenTwin and returns its entry point.
    """
    ...


def GetPortData(
    portName: str
) -> str:
    """
    For Block Items.
    Gets data chunks from the ingoing ports as json string.
    """
    ...


def GetPortMetaData(
    portName: str
) -> Any:
    """
    For Block Items.
    Gets meta data from the ingoing ports as json string.
    """
    ...


def SetPortData(
    portName: str,
    value: str
) -> bool:
    """
    For Block Items.
    Sets data chunks as json string for the outgoing ports.
    Returns True on success.
    """
    ...


def SetPortMetaData(
    portName: str,
    value: str
) -> bool:
    """
    For Block Items.
    Sets metadata as json string of the outgoing ports.
    Returns True on success.
    """
    ...
