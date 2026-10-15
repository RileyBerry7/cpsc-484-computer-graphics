// Wavefront OBJ parsing -- see include/ObjParser.h for why this is its own
// translation unit. Student work; everything around it is given.

#include "ObjParser.h"

#include <cstdlib>
#include <sstream>
#include <iostream>

namespace {

// Pulls the three fields out of one "a/b/c" corner. Any of b and c may be
// absent: "a", "a/b", "a//c" and "a/b/c" are all legal OBJ.
//
// Indices are returned already converted to zero-based, or -1 when the field
// was not present. OBJ counts from 1, and OBJ also allows NEGATIVE indices
// meaning "counting back from the end of the list so far", which is why the
// caller passes the current counts in.
void parseCorner(const std::string& field,
                 size_t vCount, size_t vtCount, size_t vnCount,
                 int& vi, int& ti, int& ni) {
    vi = ti = ni = -1;

    const size_t s1 = field.find('/');
    const size_t s2 = (s1 == std::string::npos) ? std::string::npos
                                                : field.find('/', s1 + 1);

    const std::string a = field.substr(0, s1);
    const std::string b = (s1 == std::string::npos) ? std::string()
                        : field.substr(s1 + 1,
                              (s2 == std::string::npos) ? std::string::npos
                                                        : s2 - s1 - 1);
    const std::string c = (s2 == std::string::npos) ? std::string()
                                                    : field.substr(s2 + 1);

    struct Resolve {
        static int one(const std::string& text, size_t count) {
            if (text.empty()) return -1;
            const long raw = std::atol(text.c_str());
            if (raw > 0)  return static_cast<int>(raw - 1);
            if (raw < 0)  return static_cast<int>(static_cast<long>(count) + raw);
            return -1;    // index 0 is not legal in OBJ
        }
    };

    vi = Resolve::one(a, vCount);
    ti = Resolve::one(b, vtCount);
    ni = Resolve::one(c, vnCount);
}

} // namespace


/******************************************************************************
 * 5.3 PARSING STRATEGY
1. Read the input stream one line at a time.
2. Skip blank lines and comments.
3. Use a stringstream to read the first token on each line.
4. For v records, read x, y, and z and add a vertex.
5. For vn records, read x, y, and z and add a normal.
6. For vt records, read u and v into a local texture-coordinate table. If a third value appears on the line, ignore it.
7. For f records, collect the face fields and process the first three corners as one triangle.
8. Use parseCorner() to resolve vertex, texture, and normal indices, including optional and negative indices.
9. Reject a face if any required vertex index is invalid.
10 If a normal index is missing or invalid, fall back to the vertex index so the rest of the project can still handle the mesh
11. Push one UV per triangle corner when a valid texture index is present, and set hasTexCoords only if the file actually
supplied usable UV data.
12. Ignore records that are outside the required subset
***************8******************************************************************/

bool parseObj(std::istream& in, ObjMesh& out) {

    // TODO: (objparser): parse v, vn, vt and f lines into the ObjMesh
    // Read the stream a line at a time and dispatch on the first token.
    // f corners are "a/b/c" with b and c each optional; indices are
    // 1-based and may be negative (counting back from the end).
    // Resolve each vt index to a coordinate here and push one per corner,
    // so faces keep their stride of 2 (vertex, normal).

    std::cout << "Starting OBJ parse: " << std::endl;

    std::string line;
    (void)in;
    out = ObjMesh();
    int lineIndex = 0;

    // 1. Read the input stream one line at a time.
    // Line Loop
    while (std::getline(in, line)) {
        lineIndex++;

        // 2. Skip blank lines and comments.
        if (line.empty() || line[0] == '#') continue; 

        //3. Use a stringstream to read the first token on each line.
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        // 4. For v records, read x, y, and z and add a vertex.
        if (token == "v") {
        
            float x, y, z;
            iss >> x >> y >> z;
            out.vertices.push_back(glm::vec3(x, y, z));

        // 5. For vn records, read x, y, and z and add a normal.
        } else if (token == "vn") {
            
            float x, y, z;
            iss >> x >> y >> z;
            out.normals.push_back(glm::vec3(x, y, z));

        // 6. For vt records, read u and v into a local texture-coordinate table. If 3rd value exists ignore it.
        } else if (token == "vt") {

            float u, v;
            iss >> u >> v;
            out.texCoords.push_back(glm::vec2(u, v));
            
        // 7. For f records, collect the face fields and process the first three corners as one triangle.
        } else if (token == "f") {
    
            // Corner Loop
            std::vector<int> face;
            for (int i = 0; i < 3; i++) {

                int vi, ti, ni;
                std::string cornerString;

                size_t vCount  = out.vertices.size();
                size_t vtCount = out.texCoords.size();
                size_t vnCount = out.normals.size();

                iss >> cornerString; // Read corner
                
                // 8. Use parseCorner() to resolve vertex, texture, and normal indices, including optional and negative indices
                parseCorner(cornerString, vCount, vtCount, vnCount, vi, ti, ni);

                vi--;
                ti--;
                ni--;

                // 9. Reject a face if any required vertex index is invalid.
                if (vi < 0 || static_cast<size_t>(vi) >= vCount) {

                    std::cerr << "Line " << lineIndex << " - Invalid vi, face discarded: " << line << std::endl;
                    face.clear();
                    break; // Early exit implies invalid face
                }
                face.push_back(vi); // Push corner-associated vertex index

                // 11. Push one UV per triangle corner when a valid texture index is present, and set hasTexCoords.
                if (ti >= 0 && static_cast<size_t>(ti) < vtCount) {
                    out.hasTexCoords = true;
                    //face.push_back(ti); // Push corner-associated texture index

                // Fallback for invalid ti when one was expected
                } else if (out.hasTexCoords) {
                    //face.push_back(ti); // TODO: temporary
                    std::cerr << "Line " << lineIndex << " - Expected ti: " << line << std::endl;
                }

                // 10 If a normal index is invalid fall back to the vi so the project can still handle the mesh
                if (ni < 0 || static_cast<size_t>(ni) >= vnCount)
                    ni = vi;

                face.push_back(ni); // Push corner-associated normal index

            } // END - Corner Loop

            // 9.1 Discard face if invalid
            if (!face.empty())
                out.faces.push_back(face);

        // Non-compliant lines
        } else {
            // 12. Ignore records that are outside the required subset
            std::cerr << "Line " << lineIndex << " - Unrecognized: " << line << std::endl;
        }
    
    } // END - Line Loop

    std::cerr << "Parse Completed. " << std::endl;
    std::cerr << "Parse Stats: " << std::endl;
    std::cerr << "Vertices: " << out.vertices.size() << std::endl;
    std::cerr << "Normals: " << out.normals.size() << std::endl;
    std::cerr << "TexCoords: " << out.texCoords.size() << std::endl;
    std::cerr << "Faces: " << out.faces.size() << std::endl;

    if (out.faces.empty())
        return false;
    return true;
}
//______________________________________________________________________________________________
