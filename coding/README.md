
# DataStore Lab

Topics Covered
- Policy-based Class Design
- Advanced Templates / Template Metaprograaming
- Multiple Inheritance

## Overview

All of the programs you've written up to this point have been designed
to solve some very specific problem. The specification details exactly
what the program should do given all possible inputs. All you have had
to do is come up with the best way to implement that functionality without
error (or at least handling errors gracefully). This works well for
learning about concepts and how to write programs revolving around some
computer science concept. What it doesn't do is provide the kind of
flexibility that allows other programmers to take code you've written and
extend it for their own purposes. In developing an application that is
going to be used by any number of developers for whatever purpose they
bring to the table, we want to provide a framework that will bend and stretch
to their needs. Oftentimes, they will write their own classes that they want
to fit into the framework we've provided.

A broad category of programs that fit this description is an API. An API (application programming interface) is a specification of some set of functions, data structures, classes, etc that provide some functionality. Hopefully from an operating systems course, you're familiar with the POSIX API which provides an interface in the form of system calls for applications that we write to utilize functionality the operating system manages.

APIs such as POSIX strive to accomplish the very important task of allowing us to use code and functionality that we didn't write, but they serve another very useful promise. They separate the functionality of code from its usage. Anyone could re-write the POSIX functions - as long as they take the same input, give the same output, and have the same side effects, then the libraries would be viewed as interchangeable. With this well defined interface, operating system developers can make changes to the underlying OS as long as they obey what the POSIX API promises. At the same time, application developers can make changes to their applications using the POSIX methods without worrying that the system calls will change or worse that their code will change the behavior of system calls.

Your first task
The first goal for this lab is to design a very simple API resembling some kind of data storage unit. We'll call the class DataStore, because it's very general. All DataStore can do is be read from and written to by any application/computer/whatever. DataStore doesn't even understand different types - it can only read/write chars (from/into strings, by extension). You are to write a class that provides the following public functionality:

```
class DataStore {
public:
    // p is the persistence of data - when data is written
    // to DataStore, it stays there for p seconds, after which
    // read will not include that expired data
  DataStore(int p);

    // s is a character array (string). DataStore records each
    // character in s and retains it for as many seconds as is
    // passed into the constructor
  void write(string s);

    // s is a reference to some character array (string). DataStore
    // fills that character array with all data (chars) that
    // it currently has. If there are n chars in the DataStore,
    // then when this function ends, s[0] through s[n-1] contains
    // those characters and s.size() == n
  void read(string& s);
};
```

You can, of course, add any private member variables and functions that you see fit. If you need to add a destructor you may, although I'd recomment utilizing STL containers to provide any functionality you'd like to get from dynamic memory.

The persistence of data is worth clarifying. When data is written to DataStore, the DataStore doesn't keep track of it forever. There are many reasons to provide evict data from a DataStore. Maybe the DataStore models a cache and so when it fills up, the cache has to make space. Maybe it models a bus that only holds data instantaneously and only listeners that read constantly receive that data. To keep our model very general , we'll say data persists some number of seconds before being cleaned up. Your constructor should take in one integer parameter specifying exactly how many seconds data written to the DataStore lasts. This program is still designed to be entirely single threaded - we'll get to multi-threaded programs in a future lab. Because of this, the DataStore cannot continuously monitor data it holds to remove expired data as soon as it expires. Instead, your DataStore class must ensure that when a read happens, no expired data is read.

A user of this class can make a couple of assumptions that you must guarentee:
1) DataStore is limitless, as in someone who knows about the DataStore can write infinite characters to it without error (if they fill your entire virtual address space, we'll let the underlying OS take care of things)
2) DataStore has no privacy, as in when someone wants some data, they get all of the data in DataStore, not just the data at some address - they not only can assume this, but they must deal with this slight inconvenience (more on that later)
3) DataStore's data cannot be altered - only added to. All an application aware of the DataStore can do is write (store more data) or read (get all data in DataStore), I cannot modify the data in the DataStore
Your second task
Application
Now that we have a simple way of storing data in a general sense, let's write some applications that share such data, that can read from and write to this line. Before we get into the specifics, your Application class must support the following functionality:

```
template< /*stuff here*/ >
class Application : /*stuff here*/
{
public:
    // every application has a 3-character address
    // and knows about the DataStore that is shared
    // between all Applications
  Application(string addr, DataStore& ds);

    // when an Application wants others to be able to
    // discover it, it sends out a heartbeat, writing
    // its address to the DataStore. Returns the message
    // ("HTBT"+m_addres) that was written.
  string heartbeat() const;

    // Applications can connect to other Applications,
    // discoverable via their address on the DataStore.
    // An Application can only connect with others that
    // are currently on the DataStore, so any that no
    // longer have a heartbeat should be removed. 
    // Returns the number of connections that were added.
  int connect();

    // when data is produced, the Application records
    // it, storing it to its local memory. Returns true
    // as long as the data was succefully stored, else
    // false.
  bool record(DataType data);

    // Applications can broadcast all data that they've
    // recorded on the DataStore's line for other Applications
    // to read. Returns the number of data points written.
  int broadcast() const;

    // I can also read the data messages that are present
    // on the DataStore at the time this is called, recording
    // it as long as it was broadcasted by someone other that
    // myself. Returns the number of messages read.
  int readMessages();

private:
    // add any private functionality and members you want
};
```

You're to employ policy-based class design techniques in creating this class to handle a majority of the functionality. There are three policies specific to our Application class. A Protocol policy, which determines how to format messages written to the DataStore as well as how to read messages read from the DataStore. An Encoding policy, which encodes and decodes data written to and read from the DataStore. Finally, a Storage policy, which takes care of managing the data for each application.

## Protocol

The Protocol policy needs to take care of preparing messages for writing to the DataStore and extracting information from messages read from the DataStore. So before an Application writes something to the DataStore, it asks the Protocol for format the message. Similarly, when an Application reads from the DataStore, it passes the raw data to the Protocol and asks for either the address if it's connecting and data if its reading messages. The only Protocol policy we'll write is called SimpleProtocol and supports the following functionality:

```
template< /*stuff here*/ >
class SimpleProtocol : /*stuff here*/ {
public:
    // WRITE - when the Application is going to write,
    // I have to add the appropriate header so that
    // I can read the information later. Returns the
    // message ready for sending on the DataStore
  string prepareHeartbeat(string addr) const;
  string prepareData(string data, string addr) const;

    // READ - given some raw data, I need to be able to
    // pull out the address of a heartbeat or data of a
    // data message. Returns true if there is an address/data
    // left in the rawdata, setting startIdx to the character
    // after the last character processed, addr to the sender
    // of the data or heartbeat, and data to the data in the
    // message.
  bool getNextConnection(int& startIdx, const string& rawData, string& addr) const;
  bool getNextData(int& startIdx, const string& rawData, string& data, string& addr) const;

private:
    // we need a series of types of headers that the Protocol
    // can use to tell what information is present in a string
    // raw data: a series of characters
  enum MsgType { HEARTBEAT, DATA, KEY };
};
```

Let's say an application (address "BOE") does a heartbeat and writes the following data (strings) to DataStore: "kylie", "kim", "khloe". If the header string is "HTBT" for a heartbeat message and "DATA" for a data message, DataStore may look something like this:

  `"HTBTBOEDATABOEkylieDATABOEkimDATABOEkhloe"`
  
Then, if another application (address "MOO") does a heartbeat, DataStore will look like this:
  `"HTBTBOEDATABOEkylieDATABOEkimDATABOEkhloeHTBTMOO"`
  
One Application call to getNextConnection(idx, rawdata, addr) with idx initialized to 0 will return true, set idx to 7 and addr to "BOE". Another call will the same arguments will return true, set idx to rawdata.size() and addr to "MOO". Meanwhile, one Application call to getNextData(idx, rawdata, data, addr) with idx initialized to 0 will return true, set idx to 16, addr to BOE, data to "kylie". It returns true two more times (setting idx, data, and address accordingly) and then returns false.
With that being said, there are multiple ways to implement the Protocol. It is up to your discretion as to exactly what the message format is.

## Encoding

The above description of the Protocol does no encoding of messages, anyone anywhere with access to the DataStore can read the messages. We'd like some kind of capability to add protection here. To do so, the Application can also specify some Encoding Policy that the Protocol will use to encode the data before sending and decode data that is read. We'll write a HuffmanEncoder that supports the following functionality:

```
class HuffmanEncoder {
public:
    // take some data to be sent, perform Huffman Encoding,
    // and return a string of bits representing the encoding.
    // We also need the tree resulting from the encoding
    // process so that Applications receiving the data can
    // decode. This function must allocate memory for the
    // tree and copy the tree into this character array so
    // that it can be shared and other HuffmanEncoders can
    // decode given some data and a tree.
  string encode(string data, char* tree) const;

    // takes in some encoded data in the form of bits and
    // a pointer to a tree in the form of a character
    // array. It uses the tree to decode the bitstring
    // and returns the resulting string.
  string decode(string bitstring, char* tree) const;
};
```

A potentially convulted part of developing this encoder is how the tree is going to be communicated between Applications. Keep in mind the following fact from the C++ standard:

For any object (other than a base-class subobject) of trivially copyable type T, whether or not the object holds a valid value of type T, the underlying bytes making up the object can be copied into an array of char, unsigned char, or std::byte. If the content of that array is copied back into the object, the object shall subsequently hold its original value. As always, be wary of leaking memory. You may find it helpful to first write a SimpleEncoder class that does not encoding (just returns the passed in string), up to you.

## Storage

Finally, your Application needs to have a Storage policy. It stores the type of data that your Application manages and gives the application a way to access that data. Our Storage (unsurprisingly rather simple) implements the following functionality:

```
template
class SimpleStorage {
public:
    // have the Storage policy remember some piece of data,
    // returns the index of the stored data, or -1 if storing
    // was unsuccessful
  int      store(DataType data);

    // return the amount of data stored
  int      size() const;

    // get a piece of data by index
  DataType get(int idx) const;

private:
  // anything you need
};
```

There are many ways for an Application to remember where something is stored. Here, we'll just use integer indices. We can conveniently access all stored data by looping over the indices from 0 to size()-1.

In this scenario, we have the application managing it's own connections, which in practice we're unlikely to do. It'd be better - it would generalize the class structure even better, if we were to provide a ConnectionManager policy that takes care of setting up connections over the DataStore, storing connections that it has, and connecting with other Applications. But we won't go into that in this lab, you've done enough.

## Your final task

Now that you have a working, extensible Application class, let's write a real life application that uses it. You're going to implement a simple Airport application. When this airport is constructed, you need to pass in a vector of airport routes that the Airport stores. A route is a series of airport codes (all length 3) such as "LAXDENDTWSANJFK". Airports can alertAirports(), which posts their data, including their own address to the DataStore. They can also gatherData(), in which they read all routes on the DataStore and add it to their collection of routes. The interface is as such:

```
class Airport :
  public Application<SimpleProtocol,HuffmanEncoding,Route,SimpleStorage>
{
public:

    // takes in a list of routes to store - all originate at this
    // address.
  Airport(string addr, DataStore& ds, const vector<Route>& routes);

    // alertAirports() not only makes you discoverable by other airports
    // but writes your routes to the DataStore so that others can learn
    // your routes.
  void alertAirports() const;

    // gatherData() reads in data from airports currently broadcasting
    // on the DataStore. It stores an routes on the DataStore along
    // with our own.
  void gatherData();
};
```
## Instructions to run solution;

```
# mostly concerned with compiler errors
make test
./test
```