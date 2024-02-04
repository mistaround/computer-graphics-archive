#include "mesh.h"
#include <random>
#include <unordered_set>
#include <QDebug>

glm::vec3 Mesh::getRandomColor() {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
//    float r = distribution(generator);
//    float g = distribution(generator);
//    float b = distribution(generator);
//    return glm::vec3(r, g, b);
    float g = distribution(generator);
    return glm::vec3(g);
}

Mesh::Mesh(OpenGLContext* context) :
    Drawable(context),
    faces(std::vector<uPtr<Face>>()),
    edges(std::vector<uPtr<HalfEdge>>()),
    verts(std::vector<uPtr<Vertex>>())
{}

void Mesh::create() {
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> normals;
    std::vector<glm::ivec2> joints;
    std::vector<glm::vec2> weights;
    std::vector<GLuint> indices;

    int idx = 0;

    // Write create() so that it organizes VBO data on a per-face basis
    for (auto& face : faces) {

        HalfEdge* edge = face->edge;
        int edgeCount = 0;

        // Set up pos, color, normal for every vertex on each edge
        do {
            glm::vec3 a = edge->next->vert->pos - edge->vert->pos;
            glm::vec3 b = edge->next->next->vert->pos - edge->next->vert->pos;
            glm::vec3 normal = glm::normalize(glm::cross(a, b));

            positions.push_back(glm::vec4(edge->vert->pos, 1));
            colors.push_back(glm::vec4(face->color, 1));
            normals.push_back(glm::vec4(normal, 0));

            if (!edge->vert->joints.empty()) {
                std::vector<std::pair<int, float>> joint_weight = edge->vert->joints;
                joints.push_back(glm::ivec2(joint_weight[0].first, joint_weight[1].first));
                weights.push_back(glm::vec2(joint_weight[0].second, joint_weight[1].second));
            }

            edge = edge->next;
            edgeCount ++;
        } while (edge != face->edge);

        // Set up indices of this face
        for (int i = 0; i < edgeCount - 2; i ++) {
            indices.push_back(idx);
            indices.push_back(idx + i + 1);
            indices.push_back(idx + i + 2);
        }

        idx += edgeCount;
    }

    // Buffer position data
    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    // Buffer color data
    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    // Buffer normal data
    generateNor();
    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);

    // Buffer joints data
    generateJoints();
    bindJoints();
    mp_context->glBufferData(GL_ARRAY_BUFFER, joints.size() * sizeof(glm::ivec2), joints.data(), GL_STATIC_DRAW);

    // Buffer weights data
    generateWeights();
    bindWeights();
    mp_context->glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec2), weights.data(), GL_STATIC_DRAW);

    // Buffer indice data
    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    count = indices.size();
}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}

void Mesh::addVertex(HalfEdge *edge) {
    // Step 1: Create the new vertex V3
    HalfEdge* HE1 = edge;
    HalfEdge* HE2 = edge->sym;
    Vertex* V1 = HE1->vert;
    Vertex* V2 = HE2->vert;
    uPtr<Vertex> V3 = mkU<Vertex>();
    V3->pos = (V1->pos + V2->pos) / 2.f;

    // Step 2: Create the two new half-edges HE1B and HE2B needed to surround V3
    uPtr<HalfEdge> HE1B = mkU<HalfEdge>();
    uPtr<HalfEdge> HE2B = mkU<HalfEdge>();
    HE1B->vert = V1;
    HE2B->vert = V2;
    HE1B->face = HE1->face;
    HE2B->face = HE2->face;
    V1->edge = HE1B.get();
    V2->edge = HE2B.get();
    V3->edge = HE1;

    // Step 3: Adjust the sym, next, and vert pointers of HE1, HE2, HE1B, and HE2B
    HE1B->next = HE1->next;
    HE2B->next = HE2->next;
    HE1->next = HE1B.get();
    HE2->next = HE2B.get();
    HE1->vert = V3.get();
    HE2->vert = V3.get();
    HE1B->sym = HE2;
    HE2B->sym = HE1;
    HE1->sym = HE2B.get();
    HE2->sym = HE1B.get();

    verts.push_back(std::move(V3));
    edges.push_back(std::move(HE1B));
    edges.push_back(std::move(HE2B));
}

void Mesh::triangulate(Face *face) {
    HalfEdge *HE_0 = face->edge;

    int numVerts = 0;
    do {
        HE_0 = HE_0->next;
        numVerts ++;
    } while (HE_0 != face->edge);

    Face *FACE1 = face;
    for (int i = 0; i < numVerts - 3; i ++) {
        // Step 1: Create two new half-edges HE_A and HE_B
        uPtr<HalfEdge> HE_A = mkU<HalfEdge>();
        uPtr<HalfEdge> HE_B = mkU<HalfEdge>();
        HE_A->vert = HE_0->vert;
        HE_B->vert = HE_0->next->next->vert;
        HE_A->sym = HE_B.get();
        HE_B->sym = HE_A.get();

        // Step 2: Create a second face FACE2
        uPtr<Face> FACE2 = mkU<Face>();
        FACE2->color = getRandomColor();
        HE_A->face = FACE2.get();
        HE_0->next->face = FACE2.get();
        HE_0->next->next->face = FACE2.get();
        HE_B->face = FACE1;
        FACE2->edge = HE_A.get();

        // Step 3: Fix up the next pointers for our half-edges
        HE_B->next = HE_0->next->next->next;
        HE_0->next->next->next = HE_A.get();
        HE_A->next = HE_0->next;
        HE_0->next = HE_B.get();

        faces.push_back(std::move(FACE2));
        edges.push_back(std::move(HE_A));
        edges.push_back(std::move(HE_B));
    }

}

void Mesh::subdivision() {
    // Initial mesh
    std::unordered_map<int, Vertex*> faceToCentroid;
    int originNumVerts = verts.size();
    int originNumEdges = edges.size();
    int originNumFaces = faces.size();

    // For each Face, compute its centroid
    for (int i = 0; i < originNumFaces; i ++) {
        Face *face = this->faces[i].get();
        addCentroid(face, faceToCentroid);
    }

    // Compute the smoothed midpoint of each edge in the mesh
    std::unordered_set<int> processed;
    for (int i = 0; i < originNumEdges; i ++) {
        HalfEdge *edge = edges[i].get();
        // Should only process for a pair of HE once
        if (processed.find(edge->id) == processed.end()) {
            processed.insert(edge->id);
            processed.insert(edge->sym->id);
            addMidpoint(edge, faceToCentroid);
        }
    }

    // Smooth the original vertices
    for (int i = 0; i < originNumVerts; i ++) {
        Vertex *vert = verts[i].get();
        smoothVertex(vert, faceToCentroid);
    }

    // For each original face, split that face into N quadrangle faces
    for (int i = 0; i < originNumFaces; i ++) {
        std::vector<HalfEdge*> originEdges;
        std::vector<HalfEdge*> newEdges;

        Face *face = faces[i].get();
        HalfEdge *edge = face->edge->next;
        do {
            originEdges.push_back(edge);
            edge = edge->next;
            newEdges.push_back(edge);
            edge = edge->next;
        } while (edge != face->edge->next);

        quadrangulate(face, faceToCentroid[face->id], originEdges, newEdges);
    }
}

void Mesh::addCentroid(Face *face, std::unordered_map<int, Vertex*> &map) {
    // Centroid = average of all vertices
    uPtr<Vertex> centroid = mkU<Vertex>();
    HalfEdge *edge = face->edge;

    int numEdges = 0;
    do {
        centroid->pos += edge->vert->pos;
        edge = edge->next;
        numEdges++;
    } while (edge != face->edge);

    centroid->pos = centroid->pos / (float)numEdges;
    map.insert({face->id, centroid.get()});
    verts.push_back(std::move(centroid));
}

void Mesh::addMidpoint(HalfEdge* edge, std::unordered_map<int, Vertex*> &map) {
    // Step 1: Create the new vertex
    HalfEdge* HE1 = edge;
    HalfEdge* HE2 = edge->sym;
    Vertex* V1 = HE1->vert;
    Vertex* V2 = HE2->vert;
    Face *F1 = HE1->face;
    Face *F2 = HE2->face;
    Vertex *C1 = map[F1->id];
    Vertex *C2 = map[F2->id];

    uPtr<Vertex> midPoint = mkU<Vertex>();
    midPoint->edge = HE1;
    midPoint->pos = F2 != nullptr ?
                    (V1->pos + V2->pos + C1->pos + C2->pos) / 4.f :
                    (V1->pos + V2->pos + C1->pos) / 3.f;

    // Step 2: Create the two new half-edges HE1B and HE2B needed to surround V3
    uPtr<HalfEdge> HE1B = mkU<HalfEdge>();
    uPtr<HalfEdge> HE2B = mkU<HalfEdge>();
    HE1B->vert = V1;
    HE2B->vert = V2;
    HE1B->face = HE1->face;
    HE2B->face = HE2->face;
    V1->edge = HE1B.get();
    V2->edge = HE2B.get();

    // Step 3: Adjust the sym, next, and vert pointers of HE1, HE2, HE1B, and HE2B
    HE1B->next = HE1->next;
    HE2B->next = HE2->next;
    HE1->next = HE1B.get();
    HE2->next = HE2B.get();
    HE1->vert = midPoint.get();
    HE2->vert = midPoint.get();
    HE1B->sym = HE2;
    HE2B->sym = HE1;
    HE1->sym = HE2B.get();
    HE2->sym = HE1B.get();

    verts.push_back(std::move(midPoint));
    edges.push_back(std::move(HE1B));
    edges.push_back(std::move(HE2B));
}

void Mesh::smoothVertex(Vertex *v, std::unordered_map<int, Vertex*> &map) {
    /*
        v' = (n-2)v/n + sum(e)/n^2 + sum(f)/n^2
        v is the vertex’s original position
        sum(e) is the sum of all adjacent midpoints
        sum(f) is the sum of the centroids of all faces incident to v
    */
    float n = 0.f;
    glm::vec3 e = glm::vec3(0.f);
    glm::vec3 f = glm::vec3(0.f);

    HalfEdge *edge = v->edge;
    do {
        e += edge->sym->vert->pos;
        f += map[edge->face->id]->pos;
        edge = edge->next->sym;
        n += 1.f;
    } while (edge != v->edge);

    v->pos = (n - 2) * v->pos / n +
             e / (n * n) +
             f / (n * n);
}

void Mesh::quadrangulate(Face *face, Vertex *centroid, std::vector<HalfEdge*> &originEdges, std::vector<HalfEdge*> &newEdges) {
    int numVerts = originEdges.size();
    std::vector<uPtr<HalfEdge>> toCentroidEdges;
    std::vector<uPtr<HalfEdge>> toMidpointEdges;

    for (int i = 0; i < numVerts; i ++) {
        /*
            *--------->M--origin-->*
            |          |           |
            |          |          new
            |          |           |
            *----------C<----------*
            |          |           |

            new -> toCentroid
            toCentroid -> toMidpoint
            toMidpoint ->origin
        */

        HalfEdge *originEdge = originEdges[i];
        HalfEdge *newEdge = newEdges[i];
        uPtr<HalfEdge> toCentroidEdge = mkU<HalfEdge>();
        uPtr<HalfEdge> toMidpointEdge = mkU<HalfEdge>();

//        newEdge->vert = centroid;
//        toCentroidEdge->vert = originEdge->sym->next->vert;
//        toMidpointEdge->vert = originEdge->sym->vert;

        toCentroidEdge->vert = centroid;
        toMidpointEdge->vert = originEdge->sym->vert;

        newEdge->next = toCentroidEdge.get();
        toCentroidEdge->next = toMidpointEdge.get();
        toMidpointEdge->next = originEdge;

        if (i == 0) {
            // Modify the origin face
            centroid->edge = toCentroidEdge.get();
            face->edge = toMidpointEdge.get();
            toCentroidEdge->face = face;
            toMidpointEdge->face = face;
        } else {
            // Create new face
            uPtr<Face> newFace = mkU<Face>();
            newFace->edge = toCentroidEdge.get();
            newFace->color = getRandomColor();
            toCentroidEdge->face = newFace.get();
            toMidpointEdge->face = newFace.get();
            originEdge->face = newFace.get();
            newEdge->face = newFace.get();
            faces.push_back(std::move(newFace));
        }

        toCentroidEdges.push_back(std::move(toCentroidEdge));
        toMidpointEdges.push_back(std::move(toMidpointEdge));
    }

    /*
            *---new--->M--origin-->*
            |          |           |
         origin       toM         new
            |          |    toC    |
            M----------C<----------M
            |   toC    |    toM    |
           new      toM|toC      origin
            |          |           |
            *--origin--M----new----*

    */

    for (int i = 0; i < numVerts; i++) {
        toCentroidEdges[i]->sym = toMidpointEdges[(i + 1) % numVerts].get();
        toMidpointEdges[(i + 1) % numVerts]->sym = toCentroidEdges[i].get();
        edges.push_back(std::move(toCentroidEdges[i]));
        edges.push_back(std::move(toMidpointEdges[(i + 1) % numVerts]));
    }
}

