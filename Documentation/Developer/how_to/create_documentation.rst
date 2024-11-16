Create Documentation
####################

.. _target to top of document:

Build the Documentation
***********************

Syntax
******


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
    Note that there is a space before the |br|


Formulas
========

Formulas can be embedded into the documentation:

.. code-block:: RST
    :linenos:

    Inline formula :math:`a^2 + b^2 = d^2`

Inline formula :math:`a^2 + b^2 = d^2`


Centered:

.. code-block:: RST
    :linenos:

    .. math::
        a^2 + b^2 = d^2

.. math::
    a^2 + b^2 = d^2

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
        :emphasize-lines: 3

        void my_function() {
            printf("just a test");
            my_function();
            int i = 6;
        }

reults in:

.. code-block:: C++
    :linenos:
    :emphasize-lines: 3

    void my_function() {
        printf("just a test");
        my_function();
        int i = 6;
    }

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

Internal
--------

References to other pages, chaper, sections, etc. must be defined explicitly.

.. code-block:: RST

    .. _name_of_the_reference:

    Note that the undescore prefix is mandatory.
    
These references can then be referenced via:

.. code-block:: RST

    :ref:`Display text <name_of_the_reference>`


External
--------

.. code-block:: RST

    External web pages like http://www.google.de will automaticall result in a hyperlink.

    To add a text that will result in a hyperlink use:
    `Google <http://www.google.de>`_


