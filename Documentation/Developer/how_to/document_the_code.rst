.. _document_the_code:

Document the code
=================

OpenTwin uses `Doxygen <https://www.doxygen.nl/manual/index.html>`_ to generate the Code.
There are many ways to document the code by following the Doxygen style guide.
Here we have listed a guide that OpenTwin agreed on to use for all of the files in the OpenTwin projects.

The '//!' prefix should be used to indicate a `Doxygen <https://www.doxygen.nl/manual/index.html>`_ comment.
The advantage the '//!' prefix is that a code block containing multiple methods or blocks of `Doxygen <https://www.doxygen.nl/manual/index.html>`_ comments can be easily commented out by using `/* ... */`.

To address a `Doxygen <https://www.doxygen.nl/manual/index.html>`_ keyword use the `@` prefix.

Files
-----

At the beginning of the file the file name, the creation date and the contributing author(s) should be listed.

.. code:: c++

    //! @file MyFile.h
    //! @author Name
    //     or
    //! @authors Name1, Name2
    //! @date DD.MM.YYYY

Classes
-------

Every class should contain the class and the brief keyword.
The class keyword is used to create references in the documentation.
The brief should contain a short text description of the class.

.. code:: c++

    //! @class MyClass
    //! @brief The MyClass is used to store two integers.
    //! Here a more detailed description could be added
    //! which may span over multiple lines and also contain
    //! #Markdown
    //! syntax.
    class MyClass {
        ...
    };

Functions
---------

Function comments should be added to the header file.
Some IDEs support the `Doxygen <https://www.doxygen.nl/manual/index.html>`_ format and will display it in the ToolTips.

Every function should have a `\brief`, and `\param` for every parameter.

.. code:: c++

    //! @brief Switches the color to red.
    //! Detailed description
    //! @param _delay The delay in ms before the swich will happen
    void switchToRed(bool _delay);

Boxes
-----

To display a box around the text use either `\note`, `\question` or `\warning`.

.. code:: c++

    //! @brief Shuts down the system.
    //! A detailed description how the shutdown will be performed.
    //! @warning After calling this function no more requests can be queued.
    //! @param _force If true the shutdown will be forced even if a task requests to cancel the shutdown.
    bool shutdown(bool _force);

ToDo
----

A todo can be added anywhere in the code (source and header).

.. code:: c++

    void foo(void) {
        //! @todo Add functionality
    }



Markdown
--------

Here is a little cheat sheat for markdow comments.
Note that the spaces after the prefixes are mandatory.
Also note that markdown uses spaces for the indent (which should be used anyway).

.. code::

    # Level 1 Header
    ## Level 2 Header
        ...
    ###### Level 6 Header


    Regular text.
    More regular text.
    
        Code block.
        More code.

    - - -     // This is a horizontal ruler

    > Multiline
    > block quote

    *text emphasis*
    **strong text emphasis**

    ~~striketrough text~~

    - Bulletlist Item

        Detailed Text

    - Bulletlist Item 2
        + Child item 1
        + Child item 2
    - Bulletlist Item 3
        1. Numbered child 1 list 1
        1. Numbered child 2 list 1
        2. Numbered child 1 list 2
        3. Numbered child 1 list 3


