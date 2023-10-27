#include <vector>
#include <cmath>
#include "raylib.h"

// ==========================
// ====== BASE STRUCTS ======
// ==========================
struct Triangle
{
    Vector3 points[3];
};

struct basicMesh
{
    std::vector<Triangle> tries;
};

struct mat4x4{
    float m[4][4] = {0.0f};
};

// ==========================
// ====== MATRIX MATHS ======
// ==========================
mat4x4 getProjectionMatrix(float fFov, float fAspectRatio) 
{
    // Base Variables for the matrix
    float fNear = 0.1f; // Distance between user head and screen
    float fFar = 1000.0f; // Furthest point player can see

    float fFovRad = 1.0f / std::tan(fFov * 0.5f / 180.0f * PI); // Conversion from degrees to radians

    mat4x4 matProj;
    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    return matProj;
}

void MultiplayMatrixVector(Vector3 &input, Vector3 &output, mat4x4 &m)
{
    output.x = input.x * m.m[0][0] + input.y * m.m[1][0] + input.z * m.m[2][0] + m.m[3][0];
    output.y = input.x * m.m[0][1] + input.y * m.m[1][1] + input.z * m.m[2][1] + m.m[3][1];
    output.z = input.x * m.m[0][2] + input.y * m.m[1][2] + input.z * m.m[2][2] + m.m[3][2];
    float w  = input.x * m.m[0][3] + input.y * m.m[1][3] + input.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        output.x /= w; output.y /= w; output.z /= w;
    }
    
}

Triangle ApplyTriangleRotation(Triangle &tri, Vector3 rotation)
{
    mat4x4 matRotZ, matRotX, matRotY;
    Triangle triRotatedZ, triRotatedZX, triRotatedZXY;

    // == Set Matrixes ==
    // Z Rotation
    matRotZ.m[0][0] = cosf(rotation.z);
    matRotZ.m[0][1] = sinf(rotation.z);
    matRotZ.m[1][0] = -sinf(rotation.z);
    matRotZ.m[1][1] = cosf(rotation.z);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // X Rotation
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(rotation.x);
    matRotX.m[1][2] = sinf(rotation.x);
    matRotX.m[2][1] = -sinf(rotation.x);
    matRotX.m[2][2] = cosf(rotation.x);
    matRotX.m[3][3] = 1;

    // Y Rotation
    matRotY.m[0][0] = cosf(rotation.y);
    matRotY.m[0][2] = sinf(rotation.y);
    matRotY.m[1][1] = 1;
    matRotY.m[2][0] = -sinf(rotation.y);
    matRotY.m[2][2] = cosf(rotation.y);
    matRotZ.m[3][3] = 1;

    // == Apply Rotation ==
    //Z Rotation
    MultiplayMatrixVector(tri.points[0],triRotatedZ.points[0],matRotZ);
    MultiplayMatrixVector(tri.points[1],triRotatedZ.points[1],matRotZ);
    MultiplayMatrixVector(tri.points[2],triRotatedZ.points[2],matRotZ);    

    // X Rotation
    MultiplayMatrixVector(triRotatedZ.points[0],triRotatedZX.points[0],matRotX);
    MultiplayMatrixVector(triRotatedZ.points[1],triRotatedZX.points[1],matRotX);
    MultiplayMatrixVector(triRotatedZ.points[2],triRotatedZX.points[2],matRotX);

    //Y Rotation
    MultiplayMatrixVector(triRotatedZX.points[0],triRotatedZXY.points[0],matRotY);
    MultiplayMatrixVector(triRotatedZX.points[1],triRotatedZXY.points[1],matRotY);
    MultiplayMatrixVector(triRotatedZX.points[2],triRotatedZXY.points[2],matRotY);

    return triRotatedZXY;
}

// ============================
// ====== TRIANGLE MATHS ======
// ============================

//Project each point in the triangle into 2D space with projection matrix
Triangle projectTriangleTo3D(Triangle tri, mat4x4 matProj)
{
    Triangle triProjected;
    MultiplayMatrixVector(tri.points[0],triProjected.points[0],matProj);
    MultiplayMatrixVector(tri.points[1],triProjected.points[1],matProj);
    MultiplayMatrixVector(tri.points[2],triProjected.points[2],matProj);
    return triProjected;
}