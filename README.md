# What is the HBuffer?
The HBuffer is a buffer class that allows you to do multiple things. 
Point to data or allocate it if needed. 
It was created mainly to replace the c++ std::string because it cant release data.
The HBuffer may be non owning or owning depending on however you tell it to be.
This reduces the need to reallocate data drastically since instead.
Now instead of creating a copy of a string literal we can create a non owning/modifying view of it.
Or if we need to dynamically add characters we can do that with Appending to it which reallocates automatically.
