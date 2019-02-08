From the c++ standard on types (page 77):

“For any object (other than a base-class subobject) of trivially copyable type T,
whether or not the object holds a valid value of type T, the underlying bytes
making up the object can be copied into an array of char, unsigned char, or std::byte.
If the content of that array is copied back into the object, the object shall
subsequently hold its original value.”

This is a cool thing. You can convert anything to an array of chars/unsigned chars/bytes
and if you copy if back into the object it will hold its original value. This could be
useful for some kind of compression or conversion. As long as two things know the
coding/decoding function, they can get back the original value.

Basically I'm imagining some kind of communication scheme in which the sender and receiver
decide on some kind of encoding idea, apply it to the unsigned char array that stores the
vector of whatevers you're sending, and they can encode/decode safely.

The standard's example:

const size_t N = sizeof(T)
char buf[N];
T obj; // obj initialized to its original value
std::memcpy(buf, &obj, N); // between these two calls to
                           // std::memcpy, obj might be modified

std::memcpy(&obj, buf, N); // at this point, each subobject of
                           // obj of scalar type holds its original value


sizeof: https://en.cppreference.com/w/cpp/language/sizeof