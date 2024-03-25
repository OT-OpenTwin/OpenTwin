Color Style
===========

.. _globalColorStyle:

Global Color Style
------------------

OpenTwin provides a ``OTWidgets/GlobalColorStyle`` which holds the current color style aswell as all color styles that can be set.
The ``GlobalColorStyle`` emits the ``currentStyleChanged`` signals whenever the current color style changs or gets replaced by a different instance.

.. _colorStyleFile:

Color Style File
----------------

A color style file may be loaded and parsed by the frontend application.
A single color style file contains the color style information, values and a :ref:`dirty<sheetMacros>` style sheet.

Structure
^^^^^^^^^

The ``*.otstyle`` file has the following syntax:

.. code-block::

    name:<style name>
    values:[{...},{...}]
    sheet:
    ...

.. list-table::
    :header-rows: 1

    * - Syntax
      - Description
      
    * - name
      - The unique style name. The name must not be empty and not contain the ``%`` character.
    
    * - values
      - A single line json document containing the color style values.

    * - sheet
      - The rest of the document will handled as a :ref:`dirty<sheetMacros>` style sheet.
        The sheet may not contain any ``%`` character except for the allowed :ref:`macros<sheetMacros>`.

.. _sheetMacros:

Sheet Macros
^^^^^^^^^^^^

The style sheet provided in a :ref:`color style file<colorStyleFile>` is "dirty" which means it might contain macros.
Macros in the style sheet will be evaluated by the :ref:`Global Color Style<globalColorStyle>` when adding the color style.
The following macros may be used in the ``sheet`` section of a color style file.

.. list-table::
    :header-rows: 1

    * - Macro
      - Description
      
    * - %root%
      - This macro will be replaced by the :ref:`evaluated style image root path<evaluatingStyleImagePath>`.

.. _evaluatingStyleImagePath:

Evaluating Style Image Path
"""""""""""""""""""""""""""

Style images paths are evaluated by the :ref:`Global Color Style<globalColorStyle>` when adding the color style.

.. code-block::

    Input line:
    background-image: %root%/images/myBackground.png

    Evaluated line:
    <SearchPath>/<ColorStyleName>/images/myBackground.png
    e.g.
    X:\OpenTwin\Deloyment\ColorStyles\MyColorStyle\images\myBackground.png

The search path is determined by the :ref:`Global Color Style<globalColorStyle>`.
It will use the first directory which contains ``<SearchPath>\<ColorStyleName>``.
Note that the directory is not determined by the input line and the file specified there, but only by the combination of set search paths and the name of the provided color style.

File Example
^^^^^^^^^^^^

This example shows how a color style file may look like.
Note that in this examples the ``values`` section show a shortened example which is invalid.
The correct syntax may be found in ``OTWidgets/ColorStyleValue`` code documentation.

.. code-block::
  :linenos:

  name:Bright
  values:[{"Name":"BorderColor","QSS":"black","Color":{"A":255,"R":0,"G":0,"B":0},...}...]
  sheet:
  /*
  This is an example file
  */

  QWidget {
    color: red;
  }

  QMainWindow {
    backgound-color: white;
    color: black;
  }

  ...

