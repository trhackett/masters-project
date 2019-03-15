
# Memory Lab
## Topics Covered
- Memory Management in C++
- PImpl, template, and polymorphism
- Function Pointers and Lambda Functions
- Rvalue References and Move Semantics

One of the biggest and most nuanced challenges a C++ developer faces is managing allocated memory in their programs. Even the most beginner of programmers knows that every call to new must have an accompanying call to delete or else their code will leak memory. A program with heavy memory requirements that runs for any kind of duration (think browsers, games, text editors, any persisting program on your machine) needs to be very careful about the memory it allocates so that it doesn't blow up the machine's RAM and prevent other programs from starting up and running efficiently.

This lab is an exercise in careful programming. The goal is two-fold: 1. we want to develop a pointer class that handles memory allocation (calls to new) and de-allocation (calls to delete) and 2. we want to write a memory-intensive program that replaces all raw pointers (\*) with our pointer class so that we don't have to worry about leaking memory.

Note that C++ already did this for us. The C++11 standard created the unique_ptr and shared_ptr interfaces that make certain memory guarantees. Our goal here is not to replace those interfaces nor is it to come up with a better system of managing memory. Our goal is to create an interface that acts exactly like a raw pointer does with the additional functionality that it calls delete for us when the memory is no longer being used.


## Part1
###LeaklessPtr<T, Impl>>

Our leak-proof pointer class template is called `LeaklessPtr` and it defines two templates: `T`, the type object this pointer points to, and SelfishPtrImpl, a class template for a polymorphic implementation of the LeaklessPtr class template. The Leakless employs the pImpl idiom: it stores a pointer to some other class that handles the major functionality of the pointer itself. This class provides the following interface:


```
template class Impl>
class LeaklessPtr {
public:
  using AllocFunc = T* (*) ();
  using FreeFunc = void (*) (T*);
  // the main constructor utilized by this class: it
  // takes in a pointer to some allocation function
  // (that calls new) and some de-allocation function
  // (that calls delete). The constructor has its
  // implementation allocate a new object of type T
  // and maintains the pointer to it
  LeaklessPtr(AllocFunc aFunc, FreeFunc fFunc);

  // the copy constructor and assignment operator are defined
  // for LeaklessPtr, which if you know about unique_ptr, this
  // may come as a shock. Since you can copy raw pointers, we'll
  // say you can also copy LeaklessPtrs. More implementation
  // details below
  LeaklessPtr(LeaklessPtr& other);
  LeaklessPtr& operator=(LeaklessPtr& rhs);

  // the move constructor moves the resources pointer to by
  // mover's implementation to me - at the end of this function
  // mover == nullptr should return true and *this == nullptr
  // should return false. DON'T LEAK MEMORY
  LeaklessPtr(LeaklessPtr&& mover);

  // this function frees the memory we allocated, if there is any
  ~LeaklessPtr();

  // everything a normal raw pointer provides - defer the work to
  // your implementation so we can change it easily
  T& operator*() const;
  T* operator->() const;
  T& operator[](int offset) const;
  bool operator==(const T* other) const;
  bool operator!=(const T* other) const;
  bool operator==(const LeaklessPtr& other) const;
  bool operator!=(const LeaklessPtr& other) const;
  T* operator+(int offset) const;
  T* operator-(int offset) const;

private:
  // your implementation pointer here
};
```

You may be wondering who we take in a template Impl if we are going to store a pointer to the implementation. Well, we want to combine C++'s polymorphic promises with what pImpl can provide. Specifically, you should define some abstract base implementation class that provides the functionality that every raw pointer provides - leaving it to you to decide exactly what that includes. Meanwhile, Impl should be some class that derives from this abstract base classes and provides additional functionality. You'll find that to be very little for the first implementation, SelfishPtrImpl.

### Implementation specifics

#### SelfishPtrImpl<T>

The `SelfishPtrImpl` implementation of LeaklessPtr is just that - selfish. It demands, requires in fact, that it is the one and only things pointing to the allocated object at any given point in time. It's constructor should allocate the memory and point to it. It demands that it be the only one to call the free function on that object that it allocated. Because we are providing copy functionality, we'll say that SelfishPtrImpls can steal from one another, acting much like the move constructor. Outside of you implementation classes (a base class is ok), you must never explicitly call new or delete. You must pass in to the constructor some lambda function that makes the calls for you and have the implementation make the calls.

#### AltrusticPtrImpl<T>

The`AltruisticPtrImpl` is much kinder than the `SelfishPtrImpl`, it is always willing to share the object that it points to. It wouldn't steal resources - but it is willing to give its resources away when asked to. To be more technical, just like the SelfishPtrImpl, it's constructor must take in an allocation function and a freeing function - it allocates (or has some base class allocate) an object of type T. To copy, it shares the object it points to, letting the left-hand side of the call point to its object. When asked to move, it gives away the pointer to another AltruisticPtrImpl. Of course, it would never leak memory, so you must come up with a scheme for knowing whether or not you are the last referece to the object.

## Part 2 Simple Road Simulator

Now we're going to write an application that heavily relies on LeaklessPtrs to see some of the benefit. Specifically, we're going to write a simple simulator of a road network with cars flowing through it. No one likes to sit in traffic and everyone wants to take the fastest route from point A to point B. Unfortunately if everyone took the same route, there would be so much congestion no one would ever make it their destination (see the 405 mere blocks from campus for an illustration). We'd like to figure out the best way to get a number of cars to their destination in on average the fastest time possible. We already know that A* search which gives us complexity exponential to the depth of the solution (even better with a reasonable heuristic) and allows us to specify which factors effect the routing decision using different heuristic functions.

In order to explore which heurstics give us the best performance, we need to some reproducible setting in which we can test out some heuristics. So we'd like a simulator that can specify the density of cars (rate at which they enter the simulation), their movement (how many can leave an intersection), a route computation algorithm, and a heuristic for that algorithm. That way, by varying these parameters, we can evaluate how well each heurstic performs in different settings.

To simulate a network of roads and vehicles moving through it, we'll make the reduction that the network is a two-dimensional grid of intersections. It takes one "iteration" of the simulation for each car to move from one intersection to another. Each intersection can hold an infinite number of cars.

The specification for your simulator is intentionally simple and vague. You should use your now exceptional C++ skills to design the simulator. A few key requirements:

a. it must not store or use ANY raw pointers.

b. it must leak no memory.

c. your Simulation class must be constructed with the following integers:

- numRows,numCols: number of rows/columns in the network ((numRows*numCols) is the number of intersections)
- startRow,startCol: row/column each car enters on
- endRow,endCol: row/column each car exits on
- carRate: insert this many new cars each iteration interations
- numIterations: number of iterations the simulation goes through - carMovement: number of cars that can leave each intersection per iteration

d. You Simulation class should support the provided driver program that runs it: simulate.cpp

e. At the end of the simulation, it should print out the average amount of time (in ms) it took each car to successfully get from startRow,startCol to endRow,endCol. Any cars that didn't make it don't get factored into the statistic. Be sure you factor out the time it takes to compute the route, we can assume each car has an impossibly fast processor so can perform any route computation immediately. It's very rudimentary.

f. You must use Professor Smallberg's Timer class (timer.h) so we have consistent results between students.

Each iteration, at most carMovement cars per intersection can move to an adjactent intersection. Cars can only move in one of four directions: Up, Right, Down, or Left. To compute the optimal move, each car must use A* search each iteration. You can write your own and vary which heuristic function they use to compute the h value of each intersection. Heuristic functions take the following form:

```
// returns a double, takes in coordinates of an intersection
// and a const reference to the simulation
double heuristic(int row, int col, const Simulation& sim);
```

An interation of the simulation consists of the following:

- potentially inserting a new car
- moving carMovement cars per intersection (in a direction the car computes)
- removing the car from the simulation if it's reached the end
Once the simulation is over it should return the average time it took the cars to get from startRow,startCol to endRow,endCol. Remember to factor out the time it takes to compute the route or else you find some heuristics perform arbitrarily better than some other smarter heuristic.

This simulation is admittedly rudimentary. Once you've got it working and it leaks no memory, here are fun directions you can take it in:

- heuristics: which ones can you think of that get cars there the fastest?
- traffic model: it's unlikely that intersections can actually support an infinite number of cars. It could be interesting to block off full intersections. 
- accident simulation: what happens if there is an accident at an intersection and none of the cars can move? How would they factor that into their route.
- actual roads: instead of this 2D grid of intersections, can you confure up a way for the simulator to look like actual roads with distances and times to get from one to another. We make the strong assumption here that it takes the same amount of time to get from one intersection to an adjacent on, which is not true in real life. It would be interesting to make a more accurate model.


# Instructions for running solution

```
# compile and run simple test driver main function for LeaklessPtrs
make main
./main

# run memory test on main function
make mem-test

# compile and run simulation solution
# feel free to change the heuristic function in simulate.cpp
make sim
./sim

# run memory test on simulation
make sim-mem
```