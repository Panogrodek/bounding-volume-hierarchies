# Bounding-Volume-Hierarchies
*SFML visualization of the bounding volume hierarchies using a dynamic tree*

# SFML Visualization
![Sandbox_zsbLqjqZHm](https://github.com/Panogrodek/bounding-volume-hierarchies/assets/97810330/5203a82c-64de-43e8-98b0-5f0fb3b7f07f)

# How to install
step 1: clone this repository into a folder using:  <br>
git clone https://github.com/Panogrodek/bounding-volume-hierarchies/  <br>
or alternatively download the repository zip file

step 2: use the build.bat file found in bounding-volume-hierarchies folder

step 3: open the visual studio BVH.sln project and compile

# Controls and Instructions
Holding the "A" key on your keyboard switches between using a brute force collision detection algorithm,
and using BV-tree to find collisions

In CircleObject.cpp in CircleManager Render function you can uncomment the m_dynamicTree draw function, to see all of the internal nodes aabb boxes <br>
In Application.cpp you can manage how many circles are shown on the screen, manage their radiuses, starting positions, and their vectors <br>

If the circle is red, it doesn't intersect with any other circle
