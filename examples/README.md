### These are some example folders.
##
NOTE: This should be examined to see how the development process should go and the files should <ins>**not**</ins> be mindlessly copy-pasted into a project!

With the CMakeLists.txt in the examples folder it is necessary to have an ```assets``` folder in the root of your project, since it needs to be copied for the RESOURCE_PATH macro to work. If you don't want that, the just comment or delete the line:

```c
file(COPY "assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
```
