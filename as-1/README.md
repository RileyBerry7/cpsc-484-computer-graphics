## Assignment 1
---
| :--- | :--- |
| **Student Name**                      | Riley Berry                  |
| **OS**                                | Linux (Windows 11 - WSL)     |
| **Collaborators**                     | None                         |
| **References/Sources**                | LearnOpenGL (Joey de Vries)  |
<br>
**Known Issues**
Odd cube rotation. So, my implementation has the the arrow keys rotate the cube. The functionality itself works fine. However I noticed that the axis on which my cube was being rotated was always dependant on the cube's current model rotation. This makes the rotation controls feel inconsistent when the cube gets rotated a lot. I wasn't able to fix this issue. My best guess is that I have to offset the rotation axis vector by the current rotation of the cube somehow, I assume I need to store a seperate mat4 that stores the total current rotation of the cube, rather than just rotate the actual model matrix every frame.
<br>
**Feedback:**
I enjoyed this assignment. The entire thing took me ~12 hours. It was a really good refresher on how OpenGL actually loads mesh data. I think many OpenGL tutorials have you abstract the early GL objects so its easy to lose appreaciation for the interface OpenGL provides. I think the hardest part of the assigment was starting it, the provided main.cpp was pretty dense, very comment-heavy. I always try to keep my code as readable as possible, so I ended up deleting a lot of the comments. That is not to say they weren't helpful, they provided a lot of context and information which I appreaciate. If I could help other students withh this assignment I would likely reccomend them to put as much of their code into seperate source/header fiels as possible, just because I think less code lets you take a step back and gives you the oppurtunity to think about what were actually doing with the OpenGL interface and how were interacting with the rasterization pipeline. In conclusion, I thought this assignment was fun to work on.
