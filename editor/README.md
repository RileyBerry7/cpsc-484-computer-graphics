
## Assignment 3

---

| **Question**                          | **Response**                                    |
| :---                                  | :---                                            |
| **Student Name**                      | Riley Berry                                     |
| **Compile/Run Instructions**          | "./editor" or "make clean && make && ./editor"  |
| **OS**                                | Linux (Windows 11 - WSL)                        |
| **Collaborators**                     | None                                            |
| **Optional Functionalities**          | None                                            |
| **Known Issues**                      | None                             

---

### **References/Sources**                
- LearnOpenGL (Joey de Vries)
- https://learnopengl.com/

---

### **Which OBJ files download and import:**
The OBJ free OBJ files that I downloaded were the Knight-helm, ISD(Imperial Star Destroyer), and there was another OBJ for a character named Phos but they were not used in the final screenshot. Did I run into any issues? Yes, many. For a while my imported OBJs were looking mostly fine but with a few stray spaghetti vertices that I couldnt fix for the life of me. Eventually I realized its because I was converting the 1-based indexs to 0-based twice, I did not realize that the resolve function did that for me, but after I fixed that the spaghetti vertices went away and my spheres no longer had holes at their poles.

Some other issues are that for one, a lot of OBJ files use Z+ as up, I assume because it is whats used in Blender, either way models I download seem to all be flipped such that they are facing straight down, this is of course because OpenGL uses Y+ as up, I have not attempted to fix or account for this. A second issue is one to do with scaling. There were many imported OBJ models that I simply couldn't find for like 30 minutes because they were huge models such that their faces were too far from the camera to even render. The Knight-helm I had to scale down by 100% I think and the ISD by over 1000%. I made no attempt to fix the scaling issue as it was out of scope for this assignment.

---

### **Additional Functionality:**
Yes, there were two main additional functionalities I added. First, I was able to get the texture mapping working by properly saving the uv face indices and by pushing filler indices when needed. I also added a small function for sphere that allows it to calculate proper uv coordinates for its mesh. All that together allowed me to apply textures to most of my models which is visible in my final screenshot. The second additional functionality was a simple implementation of a Triangle-fanning algorithm into my existing Face/Corner loop in the OBJ parser function. This was an attempt to try to fix my spaghetti vertices issue from earlier, it of course did not help at all and to this point im not sure if it actually works or does anything different as my meshes look the same as before.

---

### **Feedback:**
I enjoyed this assignment. I completed it over the course of 3 days. The first day I spend a few hours getting the camera controls to work, the second day I spent around 6-8 hours getting the OBJ parser working, at least 2 hours were dedicated to fixing my spaghetti vertices issue caused by resolving the indices twice. On the third day I spend 2 hours playing around with the editor setting up a cool scene and also writing the README. So the entire assignment took me around 12 hours to complete, this large amount of time was mostly due to me not using AI to write any of the code, I'm sure a student could finish this assignment is under an hour using AI, OBJ parsing has an abundant amount of resources online dedicated to it. I have written an OBJ parser before and it was a lot easier this time around, yet it still took me a whole day to get it functional. During my time writing the OBJ parser I stuck as faithful as possible to the strategy outline in the assignment document. I only used google to check the proper c++ syntax of a line when I was unsure if it was legal, and I only used AI at the end to help me identify bugs in my code and more specifically the cause of the spaghetti vertices issue, it wasnt very helpful and I did not use it to write any code. 

I thought this assignment was more difficult than assignment 2, mainly because OBJ parsers take a second to understand especially in a project envioronment I didnt create. I will say that I skipped over 90% of the assignment document, it was just a lot. I actually finished the code for the camera controls when I was still reading the overview section, so I really didn't use any of the information for that part of the assignment, but for a student less aquainted with spherical coordinate sytems I can fully understand how useful that information is. I really just wanted to stop reading and start coding, because that is the fun part, trying to solve a problem with your own knowledge. My experience was different with the OBJ parser section which I read many times over especially when I encountered bugs. The Parsing Strategy outline was extremely helpful. I did find some of the error cases and fallbacks to be a little confusing though. I also was confused why each shape has a single bool for hasTexCoords when an OBJ file could have some face lines that provide the UV indices and some that don't. 

There was also commenting in the parser header file that confused me, it said something like for face a/b/c only b is optional then the project document itself said that b and c were both optional. That header file also implies that the nested vector in our face vector should contain corner indices when in actuality it should contain face indices, this confusion stumped my for a while. I also had a hard ti e getting the import dialog library to work on WSL because I didn't have a certain dependency installed. Getting everything to work how I wanted it to took a while, but it was mostly enjoyable and a good learning experience, Thank you.

Riley Berry
