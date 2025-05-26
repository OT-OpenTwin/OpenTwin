.. _target to top of document:

Create Documentation
####################

This documentation is created using `Sphinx <https://www.sphinx-doc.org/>`_.

Edit the Documentation
**********************

It is recommended to use `Visual Studio Code <https://code.visualstudio.com/>`_ to edit the documentation.
Use the ``EditInVsCode.bat`` batch file to setup the environment and launch `Visual Studio Code <https://code.visualstudio.com/>`_.

Build the Documentation
***********************

Build manually
==============

To build the Sphinx documentation run the ``Documentation/Developer/MakeHtml.bat`` script.

.. note::

    Note that the code documentation is build separately.

    The ``code_doc_index.xhtml`` file, which is the landing page of the code documentation, will be overwritten by the build with a placeholder file.

To build the whole documentation (Sphinx and Code Documentation) use the ``Scripts/BuildAndTest/BuildDocumentation.bat`` script.

Build in VS Code
================

To build the documentation while editing in `Visual Studio Code <https://code.visualstudio.com/>`_ use the ``Ctrl+Shift+B`` keybind.

The keybind is defined in the ``Documentation/Developer/.vscode/keybindings.json`` file.
The task that is performed when using the keybind is defined in the ``Documentation/Developer/.vscode/tasks.json`` file.

Sphinx Syntax
*************

Documents
=========

Every page of the documentation is generated from a separate ``*.rst`` file.

The main page of the documentation is the ``index.rst`` file.

In the `OpenTwin <https://opentwin.net/>`_ documentation every root category of the documentation has its own folder.
The folder contains the main page file of the category which has the same name as the category.
For example the category ``how_to`` has a folder with the same name and the file ``how_to.rst`` in it.
To now add the ``how_to`` page to the documentation add it to the ``toctree`` in the ``index.rst`` file.

.. code-block:: RST
    :caption: index.rst

    .. toctree::

        how_to/how_to

Any other file that belongs to a certain category should be placed in the categories folder.
The files of the category should then be added to the categories ``toctree`` in the same way the catery file was added to the ``index.rst`` file.

Further nesting of the documentation is also possible.

General
=======

General documentation syntax:

.. code-block:: RST
    :linenos:
    
    This is a regular text.

    When writing multiple lines.
    They will result in a single line.
    If there is no empty line between them.

    To add a line break use..

    ..a empty line between them.

    Also adding |br| at the end of a line will result in a line break:
    This is the first line. |br|
    This is the second line. |br|
    Note that there is a space before the "|br|".

Lists
=====

The following example

.. code-block:: RST
    :linenos:

    - Bulleted list item 1
    - Bulleted list item 2
    
    1. Numbered list item 1
    2. Numbered list item 2

will result in:

- Bulleted list item 1
- Bulleted list item 2
    
1. Numbered list item 1
2. Numbered list item 2

Inline Markup
=============

The following example

.. code-block:: RST
    :linenos:

    This text **is bold**.

    This text *is italic*.

    This is ``something very important``.


will result in:

This text **is bold**.

This text *is italic*.

This is ``something very important``.

Formulas
========

Formulas can be embedded into the documentation:

.. code-block:: RST
    :linenos:

    Inline formula :math:`E = mc^2`

Inline formula :math:`E = mc^2`


Centered:

.. code-block:: RST
    :linenos:

    .. math::
        E = mc^2

.. math::
    E = mc^2

Notes
=====

.. note:: 
    Notes can be written like this

    .. code-block::
        :linenos:

        .. note::
            Note text

Warnings
========

.. warning::
    Warnings can be written like this

    .. code-block::
        :linenos:

        .. warning::
            Warning text

Code Blocks
===========

This example shows how to embed code snippets.

The following RST code:

.. code-block:: RST
    :linenos:

    .. code-block:: C++
        :linenos:
        :emphasize-lines: 2, 5

        void my_function() {
            printf("Stay healthy! <3");
            my_other_function();
            int i = 6;
            printf("Yes, YOU :*");
        }

reults in:

.. code-block:: C++
    :linenos:
    :emphasize-lines: 2, 5

    void my_function() {
        printf("Stay healthy! <3");
        my_other_function();
        int i = 6;
        printf("Yes, YOU :*");
    }

The ``linenos`` will add line numbers to the code block.
Using ``emphasize-lines`` will emphasize the specified lines.

.. note::
    Note that there is a blank line after the code block arguments and the actual code.

Nested Blocks
=============

Code blocks, notes, warnings, etc. can be nested.

Therefore the following RST code

.. code-block:: RST

    .. note::

        In this ``note block`` we add the following ``code block`` :

        .. code-block:: RST
            :linenos:

            Some fancy example

will result in:

.. note::

    In this ``note block`` we add the following ``code block`` :

    .. code-block:: RST
        :linenos:

        Some fancy example

Images
======

Images can be added by using:

.. code-block:: RST

    .. image:: <relative path tho THIS file>

e.g.

.. code-block:: RST

    .. image:: images/logo.png
    
Some image parameters:

.. code-block:: RST

    .. image:: images/logo.png
        :scale: 50%
    
or

.. code-block:: RST

    .. image:: images/logo.png
        :width: 400
        :alt: Alternative text

References
==========

Documents
---------

Other documents in the documentation can be referenced the following way:

.. code-block:: RST

    :doc:`Custom Text </path relative to documentation root>`

Internal References
-------------------

References to any other point in the documentation must be defined explicitly.

.. code-block:: RST

    .. _name_of_the_reference:

These references can then be referenced via:

.. code-block:: RST

    :ref:`Display text <name_of_the_reference>`

.. note::

    Note that the underscore prefix in the reference declaration is mandatory and must not be specified when using the reference.

External Links
--------------

External web pages like https://opentwin.net/ will automaticall result in a hyperlink.

To add a custom text that will result in a hyperlink use:

.. code-block:: RST

    `OpenTwin <https://opentwin.net/>`_

.. note::
    Note that the underscore suffix is mandatory.