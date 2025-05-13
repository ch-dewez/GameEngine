#include "GJKEPA.h"
#include "Collisions.h"
#include "glm/geometric.hpp"
#include <MacTypes.h>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <tuple>
#include <utility>

namespace Engine {
namespace Collisions {

struct Simplex {
private:
  std::array<glm::vec3, 4> m_points;
  int m_size;

public:
  Simplex() : m_size(0) {}

  Simplex &operator=(std::initializer_list<glm::vec3> list) {
    m_size = 0;

    for (glm::vec3 point : list)
      m_points[m_size++] = point;

    return *this;
  }

  void push_front(glm::vec3 point) {
    m_points = {point, m_points[0], m_points[1], m_points[2]};
    m_size = std::min(m_size + 1, 4);
  }

  glm::vec3 &operator[](int i) { return m_points[i]; }
  size_t size() const { return m_size; }

  auto begin() const { return m_points.begin(); }
  auto end() const { return m_points.end() - (4 - m_size); }
};

bool SameDirection(const glm::vec3 &direction, const glm::vec3 &ao) {
  return dot(direction, ao) > 0;
}

bool Line(Simplex &points, glm::vec3 &direction) {
  glm::vec3 a = points[0];
  glm::vec3 b = points[1];

  glm::vec3 ab = b - a;
  glm::vec3 ao = -a;

  if (SameDirection(ab, ao)) {
    direction = cross(cross(ab, ao), ab);
  }

  else {
    points = {a};
    direction = ao;
  }

  return false;
}

bool Triangle(Simplex &points, glm::vec3 &direction) {
  glm::vec3 a = points[0];
  glm::vec3 b = points[1];
  glm::vec3 c = points[2];

  glm::vec3 ab = b - a;
  glm::vec3 ac = c - a;
  glm::vec3 ao = -a;

  glm::vec3 abc = cross(ab, ac);

  if (SameDirection(cross(abc, ac), ao)) {
    if (SameDirection(ac, ao)) {
      points = {a, c};
      direction = cross(cross(ac, ao), ac);
    }

    else {
      return Line(points = {a, b}, direction);
    }
  }

  else {
    if (SameDirection(cross(ab, abc), ao)) {
      return Line(points = {a, b}, direction);
    }

    else {
      if (SameDirection(abc, ao)) {
        direction = abc;
      }

      else {
        points = {a, c, b};
        direction = -abc;
      }
    }
  }

  return false;
}

bool Tetrahedron(Simplex &points, glm::vec3 &direction) {
  glm::vec3 a = points[0];
  glm::vec3 b = points[1];
  glm::vec3 c = points[2];
  glm::vec3 d = points[3];

  glm::vec3 ab = b - a;
  glm::vec3 ac = c - a;
  glm::vec3 ad = d - a;
  glm::vec3 ao = -a;

  glm::vec3 abc = cross(ab, ac);
  glm::vec3 acd = cross(ac, ad);
  glm::vec3 adb = cross(ad, ab);

  if (SameDirection(abc, ao)) {
    return Triangle(points = {a, b, c}, direction);
  }

  if (SameDirection(acd, ao)) {
    return Triangle(points = {a, c, d}, direction);
  }

  if (SameDirection(adb, ao)) {
    return Triangle(points = {a, d, b}, direction);
  }

  return true;
}

bool NextSimplex(Simplex &points, glm::vec3 &direction) {
  switch (points.size()) {
  case 2:
    return Line(points, direction);
  case 3:
    return Triangle(points, direction);
  case 4:
    return Tetrahedron(points, direction);
  }

  // never should be here
  return false;
}

glm::vec3 Support(const Components::Collider &colliderA,
                  const Components::Collider &colliderB, glm::vec3 direction) {
  return colliderA.findFurthestPoint(direction) -
         colliderB.findFurthestPoint(-direction);
};

std::pair<bool, Simplex> GJK(const Components::Collider &colliderA,
                             const Components::Collider &colliderB) {
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

std::pair<std::vector<glm::vec4>, size_t>
GetFaceNormals(const std::vector<glm::vec3> &polytope,
               const std::vector<size_t> &faces) {
  std::vector<glm::vec4> normals;
  size_t minTriangle = 0;
  float minDistance = FLT_MAX;

  for (size_t i = 0; i < faces.size(); i += 3) {
    glm::vec3 a = polytope[faces[i]];
    glm::vec3 b = polytope[faces[i + 1]];
    glm::vec3 c = polytope[faces[i + 2]];

    glm::vec3 normal = glm::normalize(cross(b - a, c - a));
    float distance = dot(normal, a);

    if (distance < 0) {
      normal *= -1;
      distance *= -1;
    }

    normals.emplace_back(normal, distance);

    if (distance < minDistance) {
      minTriangle = i / 3;
      minDistance = distance;
    }
  }

  return {normals, minTriangle};
}

void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>> &edges,
                     const std::vector<size_t> &faces, size_t a, size_t b) {
  auto reverse =
      std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));

  if (reverse != edges.end()) {
    edges.erase(reverse);
  }

  else {
    edges.emplace_back(faces[a], faces[b]);
  }
}

ContactManifold EPA(Simplex &simplex, const Components::Collider &colliderA,
                    const Components::Collider &colliderB);

ContactManifold EPA(const Components::Collider *colliderA,
                    const Components::Collider *colliderB) {
  auto result = GJK(*colliderA, *colliderB);
  if (!result.first) {
    return ContactManifold();
  }
  return EPA(result.second, *colliderA, *colliderB);
};

ContactManifold
generateContactManifoldAfterEPA(const Components::Collider &colliderA,
                                const Components::Collider &colliderB,
                                glm::vec3 normal, float penetration);

ContactManifold EPA(Simplex &simplex, const Components::Collider &colliderA,
                    const Components::Collider &colliderB) {
  std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
  std::vector<size_t> faces = {0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2};

  // list: vec4(normal, distance), index: min distance
  auto [normals, minFace] = GetFaceNormals(polytope, faces);

  glm::vec3 minNormal;
  float minDistance = FLT_MAX;

  while (minDistance == FLT_MAX) {
    minNormal = normals[minFace];
    minDistance = normals[minFace].w;

    glm::vec3 support = Support(colliderA, colliderB, minNormal);
    float sDistance = dot(minNormal, support);

    if (abs(sDistance - minDistance) <= 0.001f) {
      continue;
    }

    minDistance = FLT_MAX;
    std::vector<std::pair<size_t, size_t>> uniqueEdges;

    for (size_t i = 0; i < normals.size(); i++) {

      // if (SameDirection(normals[i], support)) { size_t f = i * 3 .........
      if (glm::dot(glm::vec3(normals[i]), support) <
          glm::dot(glm::vec3(normals[i]), polytope[faces[i * 3]])) {
        continue;
      }
      size_t f = i * 3;

      AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
      AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
      AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

      faces[f + 2] = faces.back();
      faces.pop_back();
      faces[f + 1] = faces.back();
      faces.pop_back();
      faces[f] = faces.back();
      faces.pop_back();

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

    faces.insert(faces.end(), newFaces.begin(), newFaces.end());
    normals.insert(normals.end(), newNormals.begin(), newNormals.end());
  }

  return generateContactManifoldAfterEPA(
      colliderA, colliderB, minNormal * -1.0f,
      minDistance); // I have no idea why -1 but idc it works
}

Components::Face &
findClosestFaceToCollisions(Components::Polyhedron &polyhedron,
                            glm::vec3 normal) {
  float minDot = std::numeric_limits<float>::max();
  Components::Face *bestFace;

  for (int i = 0; i < polyhedron.faces.size(); ++i) {
    Components::Face &face = polyhedron.faces[i];
    // Ensure face normals are pre-calculated and point outwards from P_inc
    float dot = glm::dot(face.normal, normal);

    if (dot < minDot) {
      minDot = dot;
      bestFace = &face;
    }
  }

  // incidentFaceIndex now holds the index of the face on P_inc
  // whose normal is most anti-parallel to the collision normal.
    auto& result = *bestFace;
  return result;
};

// clips the line segment points v1, v2
// if they are past o along n
std::vector<glm::vec3> clip(glm::vec3 v1, glm::vec3 v2, glm::vec3 n, float o) {
  std::vector<glm::vec3> cp;

  float d1 = glm::dot(n, v1) - o;
  float d2 = glm::dot(n, v2) - o;
  // if either point is past o along n
  // then we can keep the point
  if (d1 >= 0.0)
    cp.push_back(v1);
  if (d2 >= 0.0)
    cp.push_back(v2);
  // finally we need to check if they
  // are on opposing sides so that we can
  // compute the correct point
  if (d1 * d2 < 0.0) {
    // if they are on different sides of the
    // offset, d1 and d2 will be a (+) * (-)
    // and will yield a (-) and therefore be
    // less than zero
    // get the vector for the edge we are clipping
    glm::vec3 e = v2 - v1;
    // compute the location along e
    double u = d1 / (d1 - d2);
    e *= u;
    e += v1;
    // add the point
    cp.push_back(e);
  }
  return cp;
}

struct Plane {
  glm::vec3 normal; // Plane normal (should be unit length)
  float distance;   // Distance from origin along the normal

  // Constructor from normal and a point on the plane
  Plane(const glm::vec3 &n, const glm::vec3 &pointOnPlane)
      : normal(glm::normalize(n)), distance(glm::dot(normal, pointOnPlane)) {}

  // Constructor from normal and distance
  Plane(const glm::vec3 &n, float d) : normal(glm::normalize(n)), distance(d) {}

  // Default constructor
  Plane() : normal(0.0f), distance(0.0f) {}

  static Plane calculateFacePlane(const std::vector<uint32_t> &faceIndices,
                                  const std::vector<glm::vec3> &polyVertices) {
    // Ensure we have enough vertices to define a plane
    if (faceIndices.size() < 3) {
      throw std::runtime_error(
          "Face has fewer than 3 vertices, cannot calculate plane.");
      // Or return an invalid plane: return Plane(glm::vec3(0.0f), 0.0f);
    }

    // Get the positions of the first three vertices
    const glm::vec3 &v0 = polyVertices[faceIndices[0]];
    const glm::vec3 &v1 = polyVertices[faceIndices[1]];
    const glm::vec3 &v2 = polyVertices[faceIndices[2]];

    // Calculate edge vectors
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    // Calculate the raw normal using the cross product (assumes CCW winding)
    glm::vec3 rawNormal = glm::cross(edge1, edge2);

    // Check for degenerate cases (collinear vertices -> zero normal)
    float normalLengthSq = glm::dot(rawNormal, rawNormal);
    constexpr float epsilonSq = 1e-12f; // Tolerance for zero length squared

    if (normalLengthSq < epsilonSq) {
      // Vertices are collinear or coincident.
      // We might try other vertices if available, but often indicates bad
      // geometry.
      throw std::runtime_error(
          "Face vertices are collinear, cannot calculate plane normal.");
      // Or return an invalid plane
    }

    // Normalize the normal vector
    glm::vec3 normal =
        rawNormal /
        glm::sqrt(normalLengthSq); // Manual normalize to use length^2

    // Calculate the distance 'd' using the first vertex and the normalized
    // normal
    float distance = glm::dot(normal, v0);

    // Return the calculated plane
    return Plane(normal, distance);
  }
  // Calculate signed distance from a point to the plane
  // Positive distance means the point is on the side the normal points to
  float signedDistance(const glm::vec3 &point) const {
    return glm::dot(normal, point) - distance;
  }
};

// Helper to intersect a line segment with a plane
std::optional<glm::vec3> intersectEdgePlane(const glm::vec3 &v_start,
                                            const glm::vec3 &v_end,
                                            const Plane &plane) {
  glm::vec3 line_dir = v_end - v_start;
  float denom = glm::dot(plane.normal, line_dir);

  // Check if edge is parallel to the plane (within tolerance)
  constexpr float epsilon = 1e-6f;
  if (std::abs(denom) < epsilon) {
    return std::nullopt;
  }

  // Calculate intersection parameter t
  // t = (plane.distance - dot(plane.normal, v_start)) / denom;
  // Using signedDistance simplifies this:
  float t = -plane.signedDistance(v_start) / denom;

  // Although Sutherland-Hodgman works even if t is outside [0, 1],
  // for robustness, especially with floating point, sometimes a check is added.
  // However, the core algorithm doesn't strictly need it if inside/outside
  // checks are correct.
  if (t >= -epsilon && t <= 1.0f + epsilon) {
    return v_start + line_dir * t;
  } else {
    return std::nullopt; // Intersection point outside the segment
  }
}

// Clips a polygon (represented by ordered vertices) against a single plane
std::vector<glm::vec3>
clipPolygonAgainstPlane(const std::vector<glm::vec3> &polygonVertices,
                        const Plane &clippingPlane) {
  std::vector<glm::vec3> outputVertices;
  if (polygonVertices.empty()) {
    return outputVertices;
  }

  constexpr float epsilon =
      1e-6f; // Tolerance for checking if point is on plane

  for (size_t i = 0; i < polygonVertices.size(); ++i) {
    const glm::vec3 &currentVertex = polygonVertices[i];
    const glm::vec3 &nextVertex =
        polygonVertices[(i + 1) % polygonVertices.size()]; // Wrap around

    float dist_current = clippingPlane.signedDistance(currentVertex);
    float dist_next = clippingPlane.signedDistance(nextVertex);

    bool currentInside = dist_current <= epsilon;
    bool nextInside = dist_next <= epsilon;

    // Case 1: Both vertices are inside -> Keep the next vertex
    if (currentInside && nextInside) {
      outputVertices.push_back(nextVertex);
    }
    // Case 2: Current is inside, Next is outside -> Keep intersection
    else if (currentInside && !nextInside) {
      std::optional<glm::vec3> intersection =
          intersectEdgePlane(currentVertex, nextVertex, clippingPlane);
      if (intersection) {
        outputVertices.push_back(*intersection);
      }
      // else: Edge parallel, next point is outside, do nothing
    }
    // Case 3: Current is outside, Next is inside -> Keep intersection and next
    // vertex
    else if (!currentInside && nextInside) {
      std::optional<glm::vec3> intersection =
          intersectEdgePlane(currentVertex, nextVertex, clippingPlane);
      if (intersection) {
        outputVertices.push_back(*intersection);
      }
      // else: Edge parallel, next point is inside? Should ideally not happen
      // if start is truly outside unless segment lies on plane.
      outputVertices.push_back(nextVertex);
    }
    // Case 4: Both vertices are outside -> Keep nothing
    else {
      // No vertices added
    }
  }

  // Remove duplicate consecutive vertices (can happen at corners)
  if (outputVertices.size() > 1) {
    auto last = std::unique(outputVertices.begin(), outputVertices.end(),
                            [epsilon](const glm::vec3 &a, const glm::vec3 &b) {
                              return glm::distance(a, b) < epsilon;
                            });
    outputVertices.erase(last, outputVertices.end());

    // Check if first and last points are the same after unique
    if (outputVertices.size() > 1 &&
        glm::distance(outputVertices.front(), outputVertices.back()) <
            epsilon) {
      outputVertices.pop_back(); // Remove redundant last point
    }
  }

  return outputVertices;
}

Tangent calculateTangent(glm::vec3 contactNormal) {
  contactNormal = glm::normalize(contactNormal); // Ensure it's a unit vector

  glm::vec3 tangent1;
  glm::vec3 tangent2;

  // Choose a helper vector not collinear with the normal
  glm::vec3 helper = glm::vec3(0.0f, 1.0f, 0.0f);
  if (abs(glm::dot(contactNormal, helper)) > 0.999f) { // If normal is too close to Y-axis
    helper = glm::vec3(1.0f, 0.0f, 0.0f); // Use X-axis instead
  }

  tangent1 = glm::cross(contactNormal, helper);
  tangent1 = glm::normalize(tangent1);

  tangent2 = glm::cross(contactNormal, tangent1);
  tangent2 = glm::normalize(tangent2);
  // tangent2 should already be normalized if contactNormal and tangent1 are
  // but normalize for robustness if needed: tangent2.normalize();

  Tangent tangent;
  tangent.vec1 = tangent1;
  tangent.vec2 = tangent2;
  return tangent;
}

// based on
// https://dyn4j.org/2011/11/contact-points-using-clipping/
ContactManifold
generateContactManifoldAfterEPA(const Components::Collider &colliderA,
                                const Components::Collider &colliderB,
                                glm::vec3 normal, float penetration) {
    if ((colliderA.m_entity->name == "second entity" || colliderB.m_entity->name == "second entity") && colliderA.m_entity->name != "plane" && colliderB.m_entity->name != "plane"){
        
    }
  Components::Polyhedron polyhedronA;
  Components::Polyhedron polyhedronB;
  ContactManifold manifold;
  manifold.normal = normal;
  manifold.tangent = calculateTangent(normal);
  manifold.penetration = penetration;

  if (colliderA.type == Components::ColliderType::Capsule ||
      colliderB.type == Components::ColliderType::Capsule) {
    return ContactManifold();
  } else if (colliderA.type == Components::ColliderType::Sphere ||
             colliderB.type == Components::ColliderType::Sphere) {
    return ContactManifold();
  } else {
    polyhedronA = colliderA.getPolyhedron();
    polyhedronB = colliderB.getPolyhedron();
  }

  Components::Face &face1 = findClosestFaceToCollisions(polyhedronA, normal);
  Components::Face &face2 = findClosestFaceToCollisions(polyhedronB, -normal);

  const bool face1IsMoreAligned = glm::abs(glm::dot(face1.normal, normal)) >=
                                  glm::abs(glm::dot(face2.normal, normal));

  Components::Face &ref = face1IsMoreAligned ? face1 : face2;
  Components::Face &inc = face1IsMoreAligned ? face2 : face1;
  Components::Polyhedron refPolyhedron =
      face1IsMoreAligned ? std::move(polyhedronA) : std::move(polyhedronB);
  Components::Polyhedron incPolyhedron =
      face1IsMoreAligned ? std::move(polyhedronB) : std::move(polyhedronA);

  std::vector<glm::vec3> incidentVertices =
      inc.getVertices(incPolyhedron.vertices);

  // Clip the incident face polygon against the side planes of the reference
  // face
  std::vector<glm::vec3> clippedVertices = incidentVertices;
  std::vector<glm::vec3> refVertices = ref.getVertices(refPolyhedron.vertices);
  int numRefVerts = refVertices.size();

  for (int i = 0; i < numRefVerts; ++i) {
    const glm::vec3 &v1 = refVertices[i];
    const glm::vec3 &v2 = refVertices[(i + 1) % numRefVerts]; // Next vertex

    glm::vec3 edgeDirection = glm::normalize(v2 - v1);

    // Calculate the normal of the clipping plane for this edge
    // It should be perpendicular to the edge and the reference face normal,
    // pointing *inwards* across the reference face polygon.
    glm::vec3 planeNormal =
        glm::normalize(glm::cross(edgeDirection, ref.normal));

    // Create the clipping plane passing through the edge vertex v1
    Plane clippingPlane(planeNormal, v1);

    // Clip the current polygon against this plane
    clippedVertices = clipPolygonAgainstPlane(clippedVertices, clippingPlane);

    // If clipping resulted in no vertices, there's no contact patch
    if (clippedVertices.empty()) {
      return manifold; // Early exit
    }
  }

  Plane referencePlane =
      Plane::calculateFacePlane(ref.vertexIndices, refPolyhedron.vertices);

  constexpr float distanceTolerance = 1e-4f; // Tolerance for penetration check

  for (const glm::vec3 &point : clippedVertices) {
    float dist = referencePlane.signedDistance(point);

    // If the point is behind or very close to the reference face plane
    if (dist <= distanceTolerance) {
      // Optional: Project point onto the reference plane for cleaner contacts
      // glm::vec3 contactPointOnPlane = point - dist * referencePlane.normal;
      // manifold.points.push_back(contactPointOnPlane);

      ContactPoint contactPoint;
      contactPoint.position = point;
      // contactPoint.penetration = -dist/;
      // manifold.penetration = std::max(manifold.penetration, -dist); // get
      // positive value
      manifold.points.push_back(contactPoint);
    }
  }

  return manifold;
};

} // namespace Collisions
} // namespace Engine
