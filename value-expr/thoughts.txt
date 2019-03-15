c++ standard:

c++ expression: "a sequence of operators and their operands,
that specifies a computation.". May produce a result and may
have side effects.

"The object representation of an object of type T is the sequence of N unsigned char
objects taken up by the object of type T, where N equals sizeof(T)."

"The value representation of an object is the set of bits that hold the value of type T.
For trivially copyable types, the value representation is a set of bits in the object
representation that determines a value, which is one discrete element of an
implementation-defined set of values."

Value Categories:

— An lvalue (so called, historically, because lvalues could appear on the left-hand side
of an assignment expression) designates a function or an object. [ Example: If E is an
expression of pointer type, then (asterisk)E is an lvalue expression referring to the object or
function to which E points. As another example, the result of calling a function whose
return type is an lvalue reference is an lvalue. —end example ]
— An xvalue (an “eXpiring” value) also refers to an object, usually near the end of its
lifetime (so that its resources may be moved, for example). An xvalue is the result of
certain kinds of expressions involving rvalue references (8.3.2). [ Example: The result
of calling a function whose return type is an rvalue reference is an xvalue. —end example ]
— A glvalue (“generalized” lvalue) is an lvalue or an xvalue.
— An rvalue (so called, historically, because rvalues could appear on the right-hand
side of an assignment expressions) is an xvalue, a temporary object (12.2) or subobject
thereof, or a value that is not associated with an object.
— A prvalue (“pure” rvalue) is an rvalue that is not an xvalue. [ Example: The result
of calling a function whose return type is not a reference is a prvalue. The value of a
literal such as 12, 7.3e5, or true is also a prvalue. —end example ]
Every expression belongs to exactly one of the fundamental classifications in this taxonomy:
lvalue, xvalue, or prvalue.

lvalue typically identifies some location in memory
 - not just a register
 - content can be modified
rvalue is everything else