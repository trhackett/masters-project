From the c++ standard on types (page 77):

“For any object (other than a base-class subobject) of trivially copyable type T,
whether or not the object holds a valid value of type T, the underlying bytes
making up the object can be copied into an array of char, unsigned char, or std::byte.
If the content of that array is copied back into the object, the object shall
subsequently hold its original value.”

This is a cool thing. You can convert anything to an array of chars/unsigned
chars/bytes and if you copy if back into the object it will hold its original
value. This could be useful for some kind of compression or conversion. As
long as two things know the coding/decoding function, they can get back the original value.

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

Ok project inspiration has struck. So the context is this: there is a network
of computers (maybe servers, maybe just multiple users connected) and they'd
like to share data. But they want to minimize the amount of data they're sending
over the network, so they're going to use some kind of encoding scheme. On top
of that, they can't just send data willy-nilly to anyone. They need some sort of
protocol that establishes when you can send data and when you should expect to
receive data. Furthermore, the computer itself shouldn't be the one taking care
of managing it's storage - it should delegate that responsibility out to someone
else.

These computers need something to interface with, not directly to one another,
so I'll define a Network that they register with when they are created.

Networked computers will read from and write to
the network. The network will buffer the data for
some time - during which it could be read by the
proper recipient. If it sits there for too long,
the network will delete it. We're making the assumption
here that the network is fully connected and
instantaneous. Might be ridiculous, but simplifies
things.

Mmmmm you know what. The computer should specify an encoding scheme. Rather
the protocol should have an encoding scheme. So that needs to be a templated
class in which you can specify a protocol. 