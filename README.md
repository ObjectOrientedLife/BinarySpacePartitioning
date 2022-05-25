# Binary Space Partitioning Tree(BSP Tree)

This project demonstrates an implementation of a simple BSP tree. The Binary space partitioning(BSP) technique divides a scene recursively with a plane being the divider. By traversing nodes(polygons) stored in the BSP tree, the polygons can be rendered in the correct order in terms of depth. For further explanation about the BSP tree, please refer to the Wikipedia document [^2].

## Features
- BSP tree
- A BSP version and non-BSP version
- Scene viewer
- Objects with diverse properties
- Three lights with different setups

## Environment and Prerequisites
> Note: You can run this only on Linux. If you're running Windows, using WSL(Windows Subsystem for Linux) is recommended.
> Note: The BSP tree may consume much memory. More than 8GB of RAM should be secured.

g++ and OpenGL must be installed in advance. Just enter the following commands for g++ and glm respectively.
```
sudo apt-get install g++
sudo apt-get install freeglut3-dev
```

## How to run
There are two directories included; a scene without the BSP tree and one with the BSP tree. You can run both of them and compare what the effect of the binary space partitioning is. In either directory, you can commonly use the following commands.

You can compile and run the scene by entering the command below.
```
make
make run_viewer
```

Once the window is opened, it takes up to one minute to build a BSP tree, so don't be afraid that the process has been aborted.

## How to use
- Click the left mouse button and drag it to rotate the view.
- Click the middle mouse button and drag to dolly in/out.
- Click the right mouse button and drag it to move the view.
- Pressing the keyboard z key turns the scene into the zoom mode and dragging now controls zoom in/out. You can return to the ordinary mode by pressing the z again
- Pressing the keyboard s key turns the scene into the selection mode. You can now select a new rotation pivot object. Clicking an empty space cancels the selection mode. 

## Implementation
`objImporter.h` and `objImporter.cpp` implements a .obj file importer. The importer reads a .obj file in a given path, then returns the composing polygons as a vector of faces. Be noticed that it can only parse triangulated .obj files.

The built-in depth test offered by OpenGL was disabled since transluscent objects can't be rendered correctly with it. Instead, those objects are drawn properly while traversing the BSP tree. The BSP tree is built once when the program starts. The following procedure describes how to build a BSP tree.
1. Store the information of the entire faces into a vector, namely `faceVec`.
2. Choose the `faceVec[0]` as the 'partitioner' node.
3. Find every intersection between the partitioner and other faces. If necessary, slice the partitioned polygons into multiple triangles. This algorithm is based on the codes in [^1].
4. Classify the polygons into the ones in front of the partitioner and the ones behind it. Each class again becomes into the left subtree and the right subtree. 
5. Repeat this process recursively until no one polygon slices one another.

After building the BSP tree, it is traversed in every frame a scene is rendered. The traversal is done according to the steps below.
1. If the polygon of the current node is facing toward the camera, render the rear subtree first, then this node, and finally the frontal subtree.
2. Otherwise if the polygon of the current node is facing the opposite of the camera, render the frontal subtree first, then this node, and finally the rear subtree.
3. Repeat 1. and 2. recursively for each node.

## Results
You can check out the effect of the BSP tree by yourself by comparing the scenes as consequences of the BSP version and the non-BSP version.
- Non-BSP version  
![Non-BSP](https://user-images.githubusercontent.com/36808324/170297214-fe8fd9e2-aa95-40f0-b043-ded53d5a9f6a.png)

- BSP version  
![BSP](https://user-images.githubusercontent.com/36808324/170297251-18c2cfc0-254b-413c-a843-1001bb8c2de4.png)


In the former version, the glass cube hides the objects behind it, unlike in reality. In the latter version, the objects behind the cube can be seen in the same way as they behave in the real world.

## References
[^1]: https://stackoverflow.com/questions/3142469/determining-the-intersection-of-a-triangle-and-a-plane
[^2]: https://en.wikipedia.org/wiki/Binary_space_partitioning
