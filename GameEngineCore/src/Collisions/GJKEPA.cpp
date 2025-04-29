#include "GJKEPA.h"
#include "Collisions.h"


namespace Engine {
namespace Collisions {

struct Simplex {
private:
    std::array<glm::vec3, 4> m_points;
    int m_size;

public:
    Simplex()
    : m_size (0)
    {}

    Simplex& operator=(std::initializer_list<glm::vec3> list) 
    {
        m_size = 0;

        for (glm::vec3 point : list)
        m_points[m_size++] = point;

        return *this;
    }

    void push_front(glm::vec3 point) 
    {
        m_points = { point, m_points[0], m_points[1], m_points[2] };
        m_size = std::min(m_size + 1, 4);
    }

    glm::vec3& operator[](int i) { return m_points[i]; }
    size_t size() const { return m_size; }

    auto begin() const { return m_points.begin(); }
    auto end() const { return m_points.end() - (4 - m_size); }
};

bool SameDirection(const glm::vec3& direction, const glm::vec3& ao)
{
    return dot(direction, ao) > 0;
}

bool Line(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];

    glm::vec3 ab = b - a;
    glm::vec3 ao =   - a;

    if (SameDirection(ab, ao)) {
        direction = cross(cross(ab, ao), ab);
    }

    else {
        points = { a };
        direction = ao;
    }

    return false;
}

bool Triangle(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ao =   - a;

    glm::vec3 abc = cross(ab, ac);

    if (SameDirection(cross(abc, ac), ao)) {
        if (SameDirection(ac, ao)) {
            points = { a, c };
            direction = cross(cross(ac, ao), ac);
        }

        else {
            return Line(points = { a, b }, direction);
        }
    }

    else {
        if (SameDirection(cross(ab, abc), ao)) {
            return Line(points = { a, b }, direction);
        }

        else {
            if (SameDirection(abc, ao)) {
                direction = abc;
            }

            else {
                points = { a, c, b };
                direction = -abc;
            }
        }
    }

    return false;
}

bool Tetrahedron(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];
    glm::vec3 d = points[3];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ad = d - a;
    glm::vec3 ao =   - a;

    glm::vec3 abc = cross(ab, ac);
    glm::vec3 acd = cross(ac, ad);
    glm::vec3 adb = cross(ad, ab);

    if (SameDirection(abc, ao)) {
        return Triangle(points = { a, b, c }, direction);
    }

    if (SameDirection(acd, ao)) {
        return Triangle(points = { a, c, d }, direction);
    }

    if (SameDirection(adb, ao)) {
        return Triangle(points = { a, d, b }, direction);
    }

    return true;
}

bool NextSimplex(Simplex& points, glm::vec3& direction)
{
    switch (points.size()) {
        case 2: return Line       (points, direction);
        case 3: return Triangle   (points, direction);
        case 4: return Tetrahedron(points, direction);
    }

    // never should be here
    return false;
}

glm::vec3 Support(const Components::Collider& colliderA, const Components::Collider& colliderB, glm::vec3 direction){
    return colliderA.findFurthestPoint( direction)
    - colliderB.findFurthestPoint(-direction);
};

std::pair<bool, Simplex> GJK(const Components::Collider& colliderA, const Components::Collider& colliderB){
    // Get initial support point in any direction
    glm::vec3 support = Support(colliderA, colliderB, glm::vec3(1, 0, 0));

    // Simplex is an array of points, max count is 4
    Simplex points;
    points.push_front(support);

    // New direction is towards the origin
    glm::vec3 direction = -support;
    while (true) {
        support = Support(colliderA, colliderB, direction);

        if (dot(support, direction) <= 0) {
            return std::pair<bool, Simplex>(false, Simplex()); // no collision
        }

        points.push_front(support);
        if (NextSimplex(points, direction)) { 
            return std::pair<bool, Simplex>(true, points); // no collision
        }
    }
};

std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
    const std::vector<glm::vec3>& polytope,
    const std::vector<size_t>& faces)
{
    std::vector<glm::vec4> normals;
    size_t minTriangle = 0;
    float  minDistance = FLT_MAX;

    for (size_t i = 0; i < faces.size(); i += 3) {
        glm::vec3 a = polytope[faces[i    ]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];

        glm::vec3 normal = glm::normalize(cross(b - a, c - a));
        float distance = dot(normal, a);

        if (distance < 0) {
            normal   *= -1;
            distance *= -1;
        }

        normals.emplace_back(normal, distance);

        if (distance < minDistance) {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }

    return { normals, minTriangle };
}

void AddIfUniqueEdge(
    std::vector<std::pair<size_t, size_t>>& edges,
    const std::vector<size_t>& faces,
    size_t a,
    size_t b)
{
    auto reverse = std::find(
                             edges.begin(),
                             edges.end(),
                             std::make_pair(faces[b], faces[a])
                             );

    if (reverse != edges.end()) {
        edges.erase(reverse);
    }

    else {
        edges.emplace_back(faces[a], faces[b]);
    }
}


ContactManifold EPA(const Components::Collider* colliderA, const Components::Collider* colliderB){
    auto result = GJK(*colliderA, *colliderB);
    if (!result.first) {
        return ContactManifold();
    }
    return EPA(result.second, *colliderA, *colliderB);
};

ContactManifold EPA(Simplex& simplex, const Components::Collider& colliderA, const Components::Collider& colliderB){
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
    std::vector<size_t> faces = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    // list: vec4(normal, distance), index: min distance
    auto [normals, minFace] = GetFaceNormals(polytope, faces);

    glm::vec3  minNormal;
    float minDistance = FLT_MAX;

    while (minDistance == FLT_MAX) {
        minNormal   = normals[minFace];
        minDistance = normals[minFace].w;

        glm::vec3 support = Support(colliderA, colliderB, minNormal);
        float sDistance = dot(minNormal, support);

        if (abs(sDistance - minDistance) <= 0.001f) {
            continue;
        }

        minDistance = FLT_MAX;
        std::vector<std::pair<size_t, size_t>> uniqueEdges;

        for (size_t i = 0; i < normals.size(); i++) {

            //if (SameDirection(normals[i], support)) { size_t f = i * 3 .........
            if (glm::dot(glm::vec3(normals[i]), support) < glm::dot(glm::vec3(normals[i]), polytope[faces[i*3]])){
                continue;
            }
            size_t f = i * 3;

            AddIfUniqueEdge(uniqueEdges, faces, f,     f + 1);
            AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
            AddIfUniqueEdge(uniqueEdges, faces, f + 2, f    );

            faces[f + 2] = faces.back(); faces.pop_back();
            faces[f + 1] = faces.back(); faces.pop_back();
            faces[f    ] = faces.back(); faces.pop_back();

            normals[i] = normals.back(); // pop-erase
            normals.pop_back();

            i--;
        }

        std::vector<size_t> newFaces;
        for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
            newFaces.push_back(edgeIndex1);
            newFaces.push_back(edgeIndex2);
            newFaces.push_back(polytope.size());
        }

        polytope.push_back(support);

        auto [newNormals, newMinFace] = GetFaceNormals(polytope, newFaces);
        float oldMinDistance = FLT_MAX;
        for (size_t i = 0; i < normals.size(); i++) {
            if (normals[i].w < oldMinDistance) {
                oldMinDistance = normals[i].w;
                minFace = i;
            }
        }

        if (newNormals[newMinFace].w < oldMinDistance) {
            minFace = newMinFace + normals.size();
        }

        faces  .insert(faces  .end(), newFaces  .begin(), newFaces  .end());
        normals.insert(normals.end(), newNormals.begin(), newNormals.end());
    }

    ContactManifold manifold;
    manifold.pointCount = 1;  //  Start with a single contact point
    manifold.normal = glm::normalize(glm::vec3(minNormal));
    manifold.points[0].Penetration = minDistance;
    
    // Calculate a point on the separating plane.  We can average points on the
    // face.
    glm::vec3 contactPosition(0.0f);
    for (int i = 0; i < 3; ++i) {
        contactPosition += polytope[faces[minFace * 3 + i]];
    }
    contactPosition /= 3.0f;
    manifold.points[0].Position = contactPosition;
    
    return manifold;
}


}
}

