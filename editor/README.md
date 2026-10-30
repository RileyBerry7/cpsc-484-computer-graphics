
## Assignment 3

---

| **Question**                          | **Response**                                    |
| :---                                  | :---                                            |
| **Student Name**                      | Riley Berry                                     |
| **Compile/Run Instructions**          | "./editor" or "make clean && make && ./editor"  |
| **OS**                                | Linux (Windows 11 - WSL)                        |
| **Collaborators**                     | None                                            |
| **Optional Functionalities**          | None                                            |
| **Known Issues**                      | None                                            | 
| **Additional Functionalties**         | None                                            |

---

### **References/Sources**                
- LearnOpenGL (Joey de Vries)
- https://learnopengl.com/

---

### **Why moveKey() returns a new index?:**

This is because the renderer needs to know the correct key to track after the move. From the renderer's perspective, moveKey() is a black box that destroys the key at the passed in index, so the only way for the renderer to have any idea of where the new key ends up, i.e. where in the sorted vector it ends up, is with the returned new index.

---

### **Explantation of gimbal lock:**

Gimbal lock is basically when any of our axis of rotations line up, causing us to lose some degree of rotational freedom. Euler angles represent rotation around 3 axes with pitch, yaw and roll. At certain angles the roll axis will line up with the pitch or yaw axis, causing gimbal lock.

---

### **Animation guide:**

Animated Objects: plane, sphere, skull.
Movement: The plane takes off then does a loopty loop, flattens out then does a barrel roll. The skull moves torwards the camera and jump scares it. The sphere(earth) rotates slowly in place.
Animated Transform: Skull, Plane
Animated Rotation: Sphere, Plane
Animated Scale: Skull
Easing: The plane uses linear, ease-in and ease-in-out on multiple keys.

---

### **Feedback:**
This was fun but the animation panel was a headache to use, and I am disappointed that I couldn't get my animation to look they way I wanted it to.
