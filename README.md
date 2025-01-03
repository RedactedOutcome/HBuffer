# What is the HBuffer?
The HBuffer just a simple class that has a few things.
A pointer to a place in memory, a size, a capacity, a ownership flag, and a change/modify flag.
This class is useful in the way to do things such as treat it as a string while also having complete access to if the buffer controls the data and should manage it or not. We can efficiently have the data be its own managed piece of memory one instant or just a view to another the next.
