# DC4UE
DC4UE is a Dual Contouring plugin for Unreal Engine 5 based on the [Dual Contouring implementation by Nicholas Gildea](https://github.com/nickgildea/DualContouringSample).

## What is Dual Contouring?
In simple terms, Dual Contouring is a method that generates a mesh representing the tangent plane as the derivative of a Signed Distance Function (SDF). A SDF is a distance function that represents the distance between any two arbitrary points, and returns a value between -ve infinity and +ve infinity. Dual Contouring makes use of this function by generating a face where the value of the SDF at a certain position of space is equal to 0.

## Installation
* Make a clone of the repository
* Place the folder containing the root of the repository into the `Plugins` folder of your Unreal Project.
* Recompile the plugin when prompted after opening Unreal Engine
* Make sure the plugin is enabled in the Plugins section of the project.

## Dependencies
* This plugin requires the [RuntimeMeshComponent](https://github.com/TriAxis-Games/RealtimeMeshComponent) plugin to be installed and enabled in your project.
