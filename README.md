
# masters-project

This is the root directory for labs designed for a speculative advanced C++
course. The course would address the following topics, preparing students
for writing advanced C++ code:

- Memory management
- Strong typing, templates and auto
- Exception Handling
- The C++ Concurrency API
- Lambda Expressions / functors & operator overloading / functional programming in C++
- Smart Pointers
- Move semantics and Rvalue References
- Template Metaprogramming

The first lab addresses ways to manage allocated data in C++. It is designed to
be a precursor to the semantics of C++11's unique_ptr and shared_ptr. The
[specification](pointers/README.md) provides details of what
exactly is asked of students. The code in [this directory](pointers/)
is provided as a solution for the spec.

The second lab is more of a problem set (no code-writing necessary) and addresses
rvalue references and value expressions. The [spec](value-expr/README.md)
serves as a write-up on these topics with pointers to additional resources and provides
the questions. Solutions are found in [this text file](value-expr/solutions.txt).

The third lab digs into template metaprogramming and writing classes that are easily
extensible. Programmers should be able to take the interface and use it for their
own purposes. Students need to write a number of interacting classes, the base-most
[DataStore](coding/DataStore.h) and [Application](coding/Application.h) classes
providing a generic, template interface that developers can plug their own
implementations into for their desired functionality. The specification is located
[here](coding/README.md) with the solution proposed solution code based in
[this](coding/) directory.
