#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <strstream>
#include "raylib.h"

// ==========================
// ====== BASE STRUCTS ======
// ==========================
struct Triangle
{
    Vector3 points[3];
    Color col = WHITE;
};

struct basicMesh
{
    std::vector<Triangle> tries;

    bool LoadFromObjectFile(std::string filename)
    {
        std::ifstream f(filename);
        if (!f.is_open()) return false;

        //local cache of verts
        std::vector<Vector3> verts;

        while (!f.eof())
        {
            char line[128];
            f.getline(line,128);

            std::strstream s;
            s << line;

            char junk;

            if (line[0] == 'v')
            {
                Vector3 v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if (line[0] == 'f')
            {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tries.push_back({verts[f[0]-1],verts[f[1]-1],verts[f[2]-1]});  // Construct triangle and shove it in mesh.
            }
        }
        
        
        return true;
    }


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

mat4x4 getRotationMatrix_X(float rotation)
{
    mat4x4 matRotX;

    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(rotation);
    matRotX.m[1][2] = sinf(rotation);
    matRotX.m[2][1] = -sinf(rotation);
    matRotX.m[2][2] = cosf(rotation);
    matRotX.m[3][3] = 1;

    return matRotX;
}

mat4x4 getRotationMatrix_Y(float rotation)
{
    mat4x4 matRotY;

    matRotY.m[0][0] = cosf(rotation);
    matRotY.m[0][2] = sinf(rotation);
    matRotY.m[1][1] = 1;
    matRotY.m[2][0] = -sinf(rotation);
    matRotY.m[2][2] = cosf(rotation);
    matRotY.m[3][3] = 1;

    return matRotY;
}

mat4x4 getRotationMatrix_Z(float rotation)
{
    mat4x4 matRotZ;

    matRotZ.m[0][0] = cosf(rotation);
    matRotZ.m[0][1] = sinf(rotation);
    matRotZ.m[1][0] = -sinf(rotation);
    matRotZ.m[1][1] = cosf(rotation);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    return matRotZ;
}

// Multiply a Vector 3 by a 4x4 Matrix.
void MultiplayMatrixVector(Vector3 &input, Vector3 &output, mat4x4 &m)
{
    output.x = input.x * m.m[0][0] + input.y * m.m[1][0] + input.z * m.m[2][0] + m.m[3][0];
    output.y = input.x * m.m[0][1] + input.y * m.m[1][1] + input.z * m.m[2][1] + m.m[3][1];
    output.z = input.x * m.m[0][2] + input.y * m.m[1][2] + input.z * m.m[2][2] + m.m[3][2];
    float w  = input.x * m.m[0][3] + input.y * m.m[1][3] + input.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f) // Normalise back into 3 values.
    {
        output.x /= w; output.y /= w; output.z /= w;
    }
}


// ============================
// ====== TRIANGLE MATHS ======
// ============================

//Project each point in the triangle into 2D space with projection matrix
//TODO: Turn this into a generic matrix multiplication function, as this step works on many other things (such as rotation)
Triangle projectTriangleTo3D(Triangle tri, mat4x4 matProj)
{
    Triangle triProjected;
    MultiplayMatrixVector(tri.points[0],triProjected.points[0],matProj);
    MultiplayMatrixVector(tri.points[1],triProjected.points[1],matProj);
    MultiplayMatrixVector(tri.points[2],triProjected.points[2],matProj);
    return triProjected;
}

Triangle ApplyTriangleRotation(Triangle &tri, Vector3 rotation)
{
    mat4x4 matRotZ, matRotX, matRotY;
    Triangle triRotatedZ, triRotatedZX, triRotatedZXY;

    // == Set Matrixes ==
    matRotZ = getRotationMatrix_Z(rotation.z);
    matRotX = getRotationMatrix_X(rotation.x);
    matRotY = getRotationMatrix_Y(rotation.y);


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

// ======================
// ====== LIGHTING ======
// ======================

Color getColourFromLuminance(float lum)
{
    int brightness = (int)(16.0f*lum);
    Color col;

    //TODO: Find a better way to derive this
    switch (brightness)
    {
    case 0: col = {15,15,15,255}; break;
    case 1: col = {30,30,30,255}; break;
    case 2: col = {45,45,45,255}; break;
    case 3: col = {60,60,60,255}; break;
    case 4: col = {75,75,75,255}; break;
    case 5: col = {90,90,90,255}; break;
    case 6: col = {105,105,105,255}; break;
    case 7: col = {120,120,120,255}; break;
    case 8: col = {135,135,135,255}; break;
    case 9: col = {150,150,150,255}; break;
    case 10: col = {165,165,165,255}; break;
    case 11: col = {180,180,180,255}; break;
    case 12: col = {195,195,195,255}; break;
    case 13: col = {210,210,210,255}; break;
    case 14: col = {225,225,225,255}; break;
    case 15: col = {240,240,240,255}; break;
    default: col = BLACK; break;
    }

    return col;
}


// ==========================
// ====== MODEL LOADER ======
// ==========================

