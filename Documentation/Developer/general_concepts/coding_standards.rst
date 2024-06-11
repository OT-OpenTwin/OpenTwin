Coding Standards
================

OpenTwin uses modern C++17. smart pointers, lambdas, and C++17 multithreading primitives.
C++20 may be used in scoped code segments (be aware that compiler incompatibilities may occur).

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
    *   - Parameters
        - \_camelCase
        - Vast majority of standards recommends this because \_ is more readable to (C++ crowd :)) ... (although not much to Java/.Net crowd) 
    *   - Locals
        - camelCase
        - Vast majority of standards recommends this. 
    *   - Member variables 
        - m_camelCase
        - It makes it very easy to differentiate between class members and everything else. We also use a second prefix for globals. These are the only two.
    *   - Enums and its members
        - CamelCase
        - Most except very old standards agree with this one 
    *   - Globals
        - g\_camelCase
        - You shouldn't have these in first place!
    *   - Defines
        - UPPER\_CASE
        - Very contentious and we just have to pick one here, unless if is a private constant in class or method, then use naming for Members or Locals 
    *   - File names
        - Match case of class name in file 
        - The file name should match the case of the class (e.g. MyClass -> MyClass.h. maclass.h would be wrong here). Lot of pro and cons either way but this removes inconsistency in auto generated code (important for ROS).

Header Files
------------

Use a #pragma once at the beginning of all header files.

.. code:: c++

    // MyHeader.h
    #pragma once

    #include ...

Bracketing
----------

Inside function or method body place opening curly bracket on same line.
Outside that the Namespace, Class and methods levels use separate line for the opening bracket. 
Notice that curlies are not required if you have single statement, but complex statements are easier to keep correct with the braces.
Try to use the curlies for single statements aswell to match the look.

.. code:: c++

    int main(int _argc, char* _argv[]) {
        while (x == y) {
            f0();
            if (cont()) {
                f1();
            }
            else if (foo()) {
                f2();
            } 
            else {
                f3();
            }

            if (x > 100) {
                break;
            }
        }
    }

Class Basics
------------

Whenever a class has a pointer attribute the constructor, destructor, copy constructor and assignment operator must be declared or explicitly deleted.

.. code:: c++

    class A {
        A();
        A(const A& _other);
        ~virtual A();
        A& operator = (const A& _other);
    private:
        int* m_value;
    };

    class B {
        B();
        B(const A& _other) = delete; // If copy is not allowed explicitly delete the copy constructor
        ~virtual B());
        B& operator = (const B& _other) = delete; // If assignment is not allowed explicitly delete the assignment operator
    private:
        int* m_value;
    };

The `OTCore/OTClassHelper.h` header provides helpers that may be used to explicitly delete default methods.

.. code:: c++

    #include "OTCore/OTClassHelper.h"
    class A {
        OT_DECL_NOCOPY(A) // Deletes the copy constructor and assignment operator
        A();
        ~virtual A();
    private:
        int* m_value;
    };

Class Naming Convention
-----------------------

....

Setter and Getter
-----------------

Setter must always have the "set" and getter always the "get" prefix.

.. code:: c++

    class A {
        void setX(int _x);
        int getX(void) const;
    };

Member Initialization
---------------------

Always initialize members and/or local variables as soon as they are added.

.. code:: c++

    class A {
    private:
        int m_value; // Here we added the m_value to our class.
    public:
        A() 
        : m_value(0) // So we add a initialization to ALL constructors
        {}
    };

    void foo(void) {
        int lclValue; // wrong, ALWAYS initialize variables
        int lvlValue = 0; // correct
    }

Const Functions
---------------

When adding functions which don't modify or dont allow to modify any content of the object make the functions const.

.. code::c++
    
    class A {
        int getX(void) { return m_x; };       // wrong: function should be const.
        int getX(void) const { return m_x; }; // correct.
    }

Const variables
---------------

.. code::c++

    class A {
        const int m_ix; // Correct if the variable should only be initialized and not further modified.
    };

Const and References
--------------------

Add the const suffix to a class function when returning a read only reference to an object.

.. code:: c++

    class A {
    public:
        const std::string& getName(void) { return m_name; };       // wrong, function should be const

        std::string& getName(void) { return m_name; };             // correct (writeable reference so the function can't be const)
        const std::string& getName(void) const { return m_name; }; // correct

    private:
        std::string m_string;

Whenever passing an object as an argument to a method prefer passing a const reference instead of a copy.

.. code::c++

    void foo(const MyClass& _obj);  // We pass a const reference since we don't modify _obj

.. note::
   
   Note that the reference symbol is located next to the type and not to the name.

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

This is really about memory management. A simulator has a lot of performance-critical code, and C++ provides mechanisms for high performance code. 
However, these mechanisms are complex and should be properly understood. Make sure you understand the difference between heap and stack allocation. 
In general, memory handling on the heap is less performant and errors can occur quickly. Typical examples are accessing freed memory (null pointer access) or 
failing to free allocated memory (memory leak). Good practice for preventing null pointer access errors is to set a pointer directly to nullptr whenever it 
does not reference an object, and to add appropriate tests if a pointer is equal to nullptr before it is used. Preventing memory leaks can be achieved in several ways. 
The consideration of how to handle heap-allocated memory differs depending on the scope in which the pointer exists, and how it should be shared outside its scope. First, consider a pointer in the scope of a method. That is, we allocate memory within the scope of a method, and we also want to deallocate the memory:

.. code:: c++

    void foo()
    {
        int* ptr = new int[10];
        ... something is happening ...
        delete ptr;
        ptr = nullptr;
    }

Now, if an exception is thrown between the memory allocation and its release, the scope of the method is left. The exception is then handled at a different place in the code, 
the memory is not freed and we have a memory leak. One possibility would be to frame everything between the memory allocation and the release with a try-catch block. 
The memory could then be freed in the catch block. Fortunately, C++11 provides an STL class that can handle this issue more convenient. The class of objects is called `smart pointers <https://cppstyle.wordpress.com/c11-smart-pointers/>`_ 
and its purpose is to encapsulate the live-time management of heap-allocated memory. 
There are three types, weak, unique and shared. We only recommend the use of the latter two. Also, the appropriate memory allocation methods should be used, i.e:

.. code:: c++

    void foo()
    {
        std::unique_ptr<int[]> ptr = std::make_unique<int[]>(10);
        ... something is happening ...
    }



Smart pointers are not free. They are objects of a class and come with attributes and methods. In performance critical sections, it may be better to use the good old raw pointer.
 Here the C++ operators new/delete should be used, NOT the C operator malloc/free, for various reasons.\\ 
 The second scope for pointers is at the class level, i.e. a pointer as a member of a class. Here, smart pointers are not needed and could add unnecessary complexity. 
 Instead, the very common technique `RAII (Resource Acquisition Is Initialization) <https://en.cppreference.com/w/cpp/language/raii>`_ 
 should be used. 
 Basically, it just says that resources should be allocated at object instantiation, i.e. within the constructor, and freed by calling the destructor.

.. code:: c++

    class Foo
    {
        public:
            Foo()
            {
                m_ptr = new int[10];
            }
            ~Foo()
            {
                if(m_ptr != nullptr)
                {
                    delete m_ptr;
                    m_ptr = nullptr;
                }
            }
        private:
            int* m_ptr = nullptr;
    }

Btw. this concept is very well applicable for any kind of resources, e.g. streams, locks, sockets and similar.\\ 

Check if you can use const, for example, `const float * const foo()`. 
Avoid using prefix or suffix to indicate pointer types in variable names, i.e. use `my_obj` instead of `myobj_ptr` except in cases where it might make sense to differentiate variables better, for example, `int mynum = 5; int* mynum_ptr = mynum;`

Line Breaks
-----------

Files should be committed with Unix line breaks. When working on Windows, git can be configured to checkout files with Windows line breaks and automatically convert from Windows to Unix line breaks when committing by running the following command:

.. code::
    
    git config --global core.autocrlf true

When working on Linux, it is preferable to configure git to checkout files with Unix line breaks by running the following command:

.. code::

    git config --global core.autocrlf input

For more details on this setting, see `[AirSim] <https://docs.github.com/en/get-started/getting-started-with-git/configuring-git-to-handle-line-endings>`_.

Library namespace
-----------------

Every OpenTwin library (located at ``OpenTwin/Libraries``) should use the ``ot`` namespace for its classes and functions.
By doing so every developer using a OpenTwin library can quickly find the class or function by typing ``ot::`` or going trough the ot namespace in the code documentation.

Warnings and Errors
-------------------

The code compilation should not produce any warnings or any errors.
Always compile your code locally (run at least a build all) and check for warnings and errors.

Includes
--------

Only include what you really need to include.
If a pointer is needed in a header file use a forward declaration instead of an include.
Adding more includes than needed will lead to increased compile times.

.. code:: c++

    #include <MyClass.h> // Wrong, a forward declaration is sufficient.

    class MyClass;

    class foo {
        MyClass* m_obj;
    };

For own and OpenTwin header files use the quotation marks `"` for includes.
For others (e.g. std header) use the angle brackets.

.. code:: c++

    #include "MyHeader.h" // Own header, use "..."
    #include "OTCore/Logger.h" // OpenTwin header, use "..."

    #include <string> // std header, use <...>

Comments
--------

Comment your code in the first place. We use the doxygen style for our comments (see :ref:`How to document the code?<document_the_code>`).