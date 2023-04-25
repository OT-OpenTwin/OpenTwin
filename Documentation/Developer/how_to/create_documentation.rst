Create Documentation
====================

=======================
Build the Documentation
=======================


======
Syntax
======


Formulas can be embedded into the documentation:

Either inline as :math:`a^2 + b^2 = d^2`
or centered:

.. math::
    a^2 + b^2 = d^2

.. note:: 
    Notes can be written like this

.. warning::
    Warnings like this

This example shows how to embed code snippets:

.. code-block:: C++
    :linenos:
    :emphasize-lines: 3


    void my_function()
    {
        printf("just a test");
        my_function();
        int i = 6;
    }

Images:

.. image:: ../images/logo.png

And links to external web pages:
http:www.google.de

or also

`[Google] <http://www.google.de>`_

