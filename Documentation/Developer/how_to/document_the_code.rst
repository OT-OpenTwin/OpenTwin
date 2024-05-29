.. _document_the_code:

Document the code
=================

OpenTwin uses `Doxygen <https://www.doxygen.nl/manual/index.html>`_ to generate the Code.
There are many ways to document the code by following the Doxygen style guide.
Here we have listed a guide that OpenTwin agreed on to use for all of the files in the OpenTwin projects.

The '//!' prefix should be used to indicate a `Doxygen <https://www.doxygen.nl/manual/index.html>`_ comment.
The advantage the '//!' prefix is that a code block containing multiple methods or blocks of `Doxygen <https://www.doxygen.nl/manual/index.html>`_ comments can be easily commented out by using ``/* ... */``.

To address a `Doxygen <https://www.doxygen.nl/manual/index.html>`_ keyword use the ``\`` prefix.

Detailed descriptions can be added without a keyword in the lines after the ``\brief``.

Classes
-------

Every class should have a ``\brief`` set.
The class keyword is used to create references in the documentation.
The brief should contain a short text description of the class which is a ``single`` line.

.. code:: c++

    //! \class MyClass
    //! \brief The MyClass is used to store two integers.
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

Every function should have a ``\brief`` set aswell as a ``\param`` for every parameter unless the function/parameter name is self-explanatory (e.g. setter and getter methods).
If the return is not described in the ``\brief`` or detailed description add a ``\return``.

.. code:: c++

    //! \fn bool switchToRed(bool _delay)
    //! \brief Brief description.
    //! Detailed description.
    //! Detailed description.
    //! \param _delay Parameter description.
    //! \return Return value description.
    bool switchToRed(bool _delay);

Class Members
-------------

Class members should have a ``\brief`` set. Adding ``\var`` allows to reference that value.

.. code:: c++

    class A {
        int m_int; //! \brief Brief description.

        //! \var m_dbl
        //! \brief Description.
        //! Detailed description.
        //! Detailed description.
        double m_dbl;
    };

Enums
-----

Enums should have a ``\brief`` set for the enum and every entry in the enum.

.. code:: c++

    //! \enum MyEnum
    //! \brief Brief description.
    //! Detailed description.
    //! Detailed description.
    enum MyEnum {
        MyValue, //! \brief Description

        //! \brief Brief description
        //! Detailed description.
        //! Detailed description.
        MySecondValue
    };

Macros
------

Macros or preprocessor definitions should have a ``\brief`` set.

.. code:: c++

    //! \def MY_MACRO
    //! \brief Brief description.
    //! Detailed description.
    //! Detailed description.
    #define MY_MACRO

Namespaces
----------

Notes Questions and Warnings
----------------------------

To display a box around the text use either ``\note``, ``\question`` or ``\warning``.

.. code:: c++

    //! \brief Brief description.
    //! Detailed description.
    //! Detailed description.
    //! \warning Warning box content.
    //! Detailed description.
    //! \note Note box content.
    //! \param _force Parameter description.
    //! \return Return value decription.
    bool shutdown(bool _force);

Doxygen Keywords
----------------

Here is a little cheat sheet for the common doxygen keywords.

.. list-table:: Common Doxygen Commands and Keywords
   :header-rows: 1

   * - Command/Keyword
     - Description
     - Example
   * - **a**
     - Documents a parameter or return value as an argument.
     - .. code:: c++
       
        //! Foo uses (\a _arg) to determine what to do.
        void foo(int _arg);

   * - **addtogroup**
     - Creates a new group or adds entities to a group.
     - .. code:: c++
       
        //! \addtogroup TestGroup @{
        bool foo(void);
        bool boo(void);
        //! @}

   * - **attention**
     - Documents a note or additional information that requires special attention.
     - .. code:: c++
       
        //! \attention Attention text

   * - **brief**
     - Provides a brief description of an entity.
     - .. code:: c++
       
        //! \brief Brief description.
        void foo(void);

   * - **bug**
     - Documents a known bug.
     - .. code:: c++
       
        //! \bug This is doing nothing

   * - **c**
     - Marks text as code.
     - .. code:: c++
       
        //! Foo uses an (\c int) as a argument.
        void foo(int _arg);

   * - **class**
     - Documents a class.
     - .. code:: c++
       
        //! \class MyClass
        class MyClass {};

   * - **code**
     - Indicates the start of a code block.
     - .. code:: c++
       
        //! \code
        //! Code line
        //! Code line
        //! \endcode

   * - **copybrief**
     - Copies the brief description from another entity.
     - .. code:: c++
       
        //! \brief Brief description that will also be copied to B.
        class A {};
        
        //! \brief Brief description for B.
        //! \copybrief A
        class B : public A {};

   * - **copydetails**
     - Copies the detailed description from another entity.
     - .. code:: c++
       
        //! \brief Brief description
        //! Detailed description that will be copied to B.
        class A {};
        
        //! \brief Brief description for B.
        //! \copydetails A
        class B : public A {};

   * - **def**
     - Documents a macro or preprocessor definition.
     - .. code:: c++
       
        //! \def MY_MACRO
        #define MY_MACRO

   * - **defgroup**
     - Defines a group.
     - .. code:: c++
       
        //! \defgroup MyGroup Brief group description.

   * - **deprecated**
     - Marks an entity as deprecated.
     - .. code:: c++
       
        //! \deprecated Use boo instead.
        [[deprecated("Use boo instead")]]
        void foo(void) {
          ...

   * - **enum**
     - Documents an enumeration.
     - .. code:: c++
       
        //! \enum MyEnum
        enum MyEnum {};

   * - **enumclass**
     - Documents a scoped enumeration.
     - .. code:: c++
       
        //! \enumclass MyEnumClass
        enum class MyEnumClass {};

   * - **file**
     - Documents a file.
     - .. code:: c++
       
        //! \file MyFile.h
        
   * - **fn**
     - Documents a function.
     - .. code:: c++
       
        //! \fn void foo(void)
        void foo(void);

   * - **ingroup**
     - Assigns an entity to a group.
     - .. code:: c++
       
        //! \ingroup TestGroup

   * - **label**
     - Creates a unique identifier to the specified entity allowing to reference it later.
     - .. code:: c++
       
        //! \label label_name
        void foo(void);

   * - **link**
     - Creates a hyperlink to another entity.
     - .. code:: c++
       
        //! For more information check \link void boo(void) \endlink
        void foo(void);

   * - **mainpage**
     - Documents the main page of the project.
     - .. code:: c++
       
        //! \mainpage The Main Page Title

   * - **namespace**
     - Documents a namespace.
     - .. code:: c++
       
        //! \namespace MyNamespace
        namespace MyNamespace {}

   * - **namespacealias**
     - Documents a namespace alias.
     - .. code:: c++
       
        //! \namespacealias MyAlias
        namespace MyAlias = test;

   * - **note**
     - Documents a note or additional information.
     - .. code:: c++
       
        //! \note Note text

   * - **param**
     - Documents a function parameter.
     - .. code:: c++
       
        //! \param _myParam Brief parameter description.
        void foo(int _myParam);

   * - **page**
     - Documents a page (e.g., overview, tutorial).
     - .. code:: c++
       
        //! \page MyPageName Page Title
        //! Brief page description

   * - **pagebreak**
     - Inserts a page break in the documentation.
     - .. code:: c++
       
        //! \pagebreak

   * - **ref**
     - Creates a cross-reference to another entity.
     - .. code:: c++
       
        //! For more information check out \ref label_name.

   * - **relates**
     - Indicates a related function or class. Is used to establish relationship between different entities if not done automatically.
     - .. code:: c++
       
        class MyClass {};

        //! \relates MyClass
        void foo(MyClass _arg);

   * - **return**
     - Documents the return value of a function.
     - .. code:: c++
       
        //! \return Brief return description.
        bool foo(void);

   * - **see**
     - Provides a cross-reference to another entity.
     - .. code:: c++
       
        void MyClass::foo(void);

        //! \see MyClass, foo
        void boo(void);

   * - **since**
     - Documents the version when an entity was introduced.
     - .. code:: c++
       
        //! \since YYYY-MM-DD

   * - **struct**
     - Documents a struct.
     - .. code:: c++
       
        //! \struct MyStruct
        struct MyStruct {};

   * - **tparam**
     - Documents a template parameter.
     - .. code:: c++
       
        //! \tparam T Brief template parameter description.
        template <class T> void foo(T _arg) {}

   * - **todo**
     - Marks a to-do item in the documentation.
     - .. code:: c++
       
        void foo(void) {
          //! \todo Think of functionality
        }

   * - **typedef**
     - Documents a type definition.
     - .. code:: c++
       
        //! \typedef MyType
        typedef int MyType;

   * - **union**
     - Documents a union.
     - .. code:: c++
       
        //! \union MyUnion
        union MyUnion {};

   * - **unionclass**
     - Documents a scoped union.
     - .. code:: c++
       
        //! \unionclass MyUnionClass
        union class MyUnionClass {};

   * - **until**
     - Documents the version until which an entity exists.
     - .. code:: c++
       
        //! \until YYYY-MM-DD

   * - **var**
     - Documents a variable or member variable.
     - .. code:: c++
       
        class A {
          //! \var m_var
          int m_var;
        };

   * - **warning**
     - Documents a warning.
     - .. code:: c++
       
        //! \warning Warning text


Markdown Syntax
---------------

Here is a little cheat sheet for markdow comments.
Note that the spaces after the prefixes are mandatory.
Also note that markdown uses spaces for the indent (which should be used anyway).

.. code::

    # Level 1 Header
    ## Level 2 Header
        ```
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


