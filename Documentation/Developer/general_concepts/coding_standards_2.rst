Coding Standards
================

.. warning::
    This style guide has been copied from another project and adjusted.
    It should serve as a baseline for further discussions only.

OpenTwin uses modern C++20. smart pointers, lambdas, and C++20 multithreading primitives.

Quick Note
----------

The great thing about "standards" is that there are many to chose from: `ISO <https://isocpp.org/wiki/faq/coding-standards>`_, `[Sutter & Stroustrup] <https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md>`_, `[ROS] <http://wiki.ros.org/CppStyleGuide>`_, `[LINUX] <https://www.kernel.org/doc/Documentation/process/coding-style.rst>`_, `[Google's] <https://google.github.io/styleguide/cppguide.html>`_, `[Microsoft's] <https://msdn.microsoft.com/en-us/library/888a6zcz.aspx>`_, `[CERN's] <http://atlas-computing.web.cern.ch/atlas-computing/projects/qa/draft_guidelines.html>`_, `[GCC's] <https://gcc.gnu.org/wiki/CppConventions>`_, `[ARM's] <http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0475c/CJAJAJCJ.html>`_, `[LLVM's] <http://llvm.org/docs/CodingStandards.html>`_ and probably 
thousands of others.
Unfortunately most of these can't even agree on something as basic as how to name a class or a constant.
This is probably due to the fact that these standards often carry lots of legacy issues due to supporting existing code bases.
The intention behind this document is to create guidance that remains as close to ISO, Sutter &amp; Stroustrup and ROS while resolving as many conflicts, disadvantages and inconsistencies as possible among them.

Naming Conventions
------------------

.. list-table:: 
    :header-rows: 1

    *   - Code Element
        - Style
        - Note
    *   - Namespace
        - under\_scored
        - Differentiate from class names
    *   - Class name
        - CamelCase
        - To differentiate from STL types which ISO recommends (do not use "C" or "T" prefixes) 
    *   - Function name 
        - camelCase 
        - Lower case start is almost universal except for .Net world
    *   - Parameters/Locals 
        - \_camelCase
        - Vast majority of standards recommends this because \_ is more readable to (C++ crowd :)) ... (although not much to Java/.Net crowd) 
    *   - Member variables 
        - m_camelCase
        - It makes it very easy to differentiate between class members and everything else. We also use a second prefix for globals. These are the only two.
    *   - Enums and its members
        - CamelCase
        - Most except very old standards agree with this one 
    *   - Globals
        - g\_under\_scored
        - You shouldn't have these in first place!
    *   - Constants
        - UPPER\_CASE
        - Very contentious and we just have to pick one here, unless if is a private constant in class or method, then use naming for Members or Locals 
    *   - File names
        - Match case of class name in file 
        - Lot of pro and cons either way but this removes inconsistency in auto generated code (important for ROS) 

Header Files
------------

Use a #pragma once at the beginning of all header files.

.. code:: c++

    // MyHeader.h
    #pragma once

    #include ...

The `#pragma once` is not supported if same header file exists at multiple places (which might be possible under ROS build system!).

Bracketing
----------

Inside function or method body place opening curly bracket on same line.
Outside that the Namespace, Class and methods levels use separate line for the opening bracket. 
Notice that curlies are not required if you have single statement, but complex statements are easier to keep correct with the braces.
Try to use the curlies for single statements aswell to match the look.

.. code:: c++

    int main(int _argc, char* _argv[])
    {
        while (x == y) {
            f0();
            if (cont()) {
                f1();
            } else if (foo()) {
                f2();
            } else {
                f3();
            }

            if (x > 100) {
                break;
            }
        }
    }

Const Functions
---------------

When adding functions which don't modify or dont allow to modify any content of the object make the functions const.

.. code::c++
    
    class A {
        int getX(void) { return m_x; };       // wrong: function should be const.
        int getX(void) const { return m_x; }; // correct.
    }

Const and References
--------------------

Add the const suffinx to a class function When returning a writable or read only reference to an object.

.. code:: c++

    class A {
    public:
        const std::string& getName(void) { return m_name; };       // wrong, function should be const

        std::string& getName(void) { return m_name; };             // correct (writeable reference so the function can't be const)
        const std::string& getName(void) const { return m_name; }; // correct

    private:
        std::string m_string;

Overriding
----------

When overriding a virtual method, use the override suffix.

.. code:: c++

    class A {
    public:
        virtual void foo(void) {};
    };

    class B : public A {
    public:
        virtual void foo(void) {};          // wrong, use the override suffix
        virtual void foo(void) override {}; // correct
    }

Pointers
--------

This is really about memory management.  A simulator has much performance critical code, so we try and avoid overloading the memory manager
with lots of calls to new/delete.  We also want to avoid too much copying of things on the stack, so we pass things by reference when ever possible.
But when the object really needs to live longer than the call stack you often need to allocate that object on
the heap, and so you have a pointer.  Now, if management of the lifetime of that object is going to be tricky we recommend using 
`[C++ 11 smart pointers] <https://cppstyle.wordpress.com/c11-smart-pointers/>`_. 
But smart pointers do have a cost, so don't use them blindly everywhere.  For private code 
where performance is paramount, raw pointers can be used.  Raw pointers are also often needed when interfacing with legacy systems
that only accept pointer types, for example, sockets API.  But we try to wrap those legacy interfaces as
much as possible and avoid that style of programming from leaking into the larger code base.  

Check if you can use const everywhere, for example, `const float * const foo()`.
Avoid using prefix or suffix to indicate pointer types in variable names, i.e. use `my_obj` instead of `myobj_ptr` except in cases where it might make sense to differentiate variables better, for example, `int mynum = 5; int* mynum_ptr = mynum;`

.. code:: c++
    
    class B {
        const void* foo(void) const { return m_a; };       // wrong, you could add the const suffix to the pointer
        const void* const foo(void) const { return m_a; }; // correct 
    };

Null Checking
-------------

When checking pointer if it is null you could use `if(myptr)`.

.. code:: c++
    
    if (ptr) { // Check if pointer is null
               // Do stuff
    }

Indentation
-----------

The C++ code base uses four spaces for indentation (not tabs).

Line Breaks
-----------

Files should be committed with Unix line breaks. When working on Windows, git can be configured to checkout files with Windows line breaks and automatically convert from Windows to Unix line breaks when committing by running the following command:

.. code::
    
    git config --global core.autocrlf true

When working on Linux, it is preferable to configure git to checkout files with Unix line breaks by running the following command:

.. code::

    git config --global core.autocrlf input

For more details on this setting, see `[AirSim] <https://docs.github.com/en/get-started/getting-started-with-git/configuring-git-to-handle-line-endings>`_.
