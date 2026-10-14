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
consistently.
11. Push one UV per triangle corner when a valid texture index is present, and set hasTexCoords only if the file actually
supplied usable UV data.
12. Ignore records that are outside the required subse
***************8******************************************************************/

bool parseObj(std::istream& in, ObjMesh& out) {
    // TODO: (objparser): parse v, vn, vt and f lines into the ObjMesh
    // Read the stream a line at a time and dispatch on the first token.
    // f corners are "a/b/c" with b and c each optional; indices are
    // 1-based and may be negative (counting back from the end).
    // Resolve each vt index to a coordinate here and push one per corner,
    // so faces keep their stride of 2 (vertex, normal).

    std::cout << "Starting OBJ parse: " << std::endl;

    (void)in;
    out = ObjMesh();

    // 1. Read the input stream one line at a time.
    std::string line;
    while (std::getline(in, line)) {

        // 2. Skip blank lines and comments.
        if (line.empty() || line[0] == '#') continue; 

        //3. Use a stringstream to read the first token on each line.
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        // 4. For v records, read x, y, and z and add a vertex.
        if (token == "v") {

        // 5. For vn records, read x, y, and z and add a normal.
        } else if (token == "vn") {
            
        // 6. For vt records, read u and v into a local texture-coordinate table. If 3rd value exists ignore it.
        } else if (token == "vt") {
            
        // 7. For f records, collect the face fields and process the first three corners as one triangle.
        } else if (token == "f"){

        // Non-compliant cases
        } else {
            
        }
        std::cout << line << '\n';
    }

    return true;
}
