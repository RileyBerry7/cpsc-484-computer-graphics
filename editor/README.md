
## Assignment 5

---

| **Question**                          | **Response**                                    |
| :---                                  | :---                                            |
| **Student Name**                      | Riley Berry                                     |
| **Compile/Run Instructions**          | "./editor" or "make clean && make && ./editor"  |
| **OS**                                | Linux (Windows 11 - WSL)                        |
| **Collaborators**                     | None                                            |
| **Known Issues**                      | None                                            | 
| **Additional Functionalties**         | None                                            |

---

### **References/Sources**                
- LearnOpenGL (Joey de Vries)
- https://learnopengl.com/
- The Math of Computer Graphics - TEXTURES and SAMPLERS
- https://www.youtube.com/watch?v=DuQDx0ZIxa8

---

### **An explanation of why cube() returns one UV coordinate per triangle corner rather than one per vertex. Explain your answer in terms of the Cube's faces and shared corners:**

 A cube has 8 corners, and each corner is shared by 3 perpendicular faces that need to map to different parts of the texture. If we only stored one UV per vertex, all three faces meeting at that corner would be forced to use the exact same texture coordinate, causing heavy stretching. Returning one UV per triangle corner lets the same 3D vertex have different UV coordinates depending on which face is being drawn.

---

### **A description of what the Teapot's wrap seam looks like before it is repaired, and why the repair cannot be represented using one UV coordinate per vertex:**

Before fixing it, the wrap seam looks like a thin vertical strip down the teapot where the entire texture is squished and backwards. This happens because the triangles at the end of the wrap interpolate from u ≈ 0.99 back down to u = 0.0 instead of going forward to 1.0. You can't fix this with one UV per vertex because the vertices on that seam line need to act as u = 0.0 for the triangles starting the wrap and u = 1.0 for the triangles finishing it, which requires per-corner UVs.

---

### **An explanation of why nearest-neighbor sampling may be difficult to notice on a photograph such as 2k_earth_daymap.jpg but obvious on checker.png. Explain why the tests sample between texel centers:**

2k_earth_daymap.jpg is very high-resolution with smooth, organic color transitions, so individual texels are usually smaller than screen pixels and hard to notice unless you zoom way in. checker.png is low-resolution with sharp, high-contrast black and white edges, which makes nearest-neighbor's blocky "staircase" aliasing obvious. The tests sample between texel centers because sampling right on a texel center gives the exact same color for both nearest-neighbor and bilinear filtering—sampling in between forces bilinear to blend neighbors while nearest-neighbor snaps to one, proving the filter actually works.

---

### **Feedback:**
None.
