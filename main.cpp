#include "engine.h"
#include <iostream>
#include <algorithm>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    //Test cube - will replace later
    basicMesh meshCube;
    // meshCube.tries = 
    // {

    //     // SOUTH FACE
	// 	{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
	// 	{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	// 	// EAST FACE                                             
	// 	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
	// 	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

	// 	// NORTH FACE                                                   
	// 	{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
	// 	{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

	// 	// WEST FACE                                                   
	// 	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
	// 	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

	// 	// TOP FACE                                                 
	// 	{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
	// 	{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

	// 	// BOTTOM FACE                                                  
	// 	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
	// 	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
    
    // };

    meshCube.LoadFromObjectFile("models/burger.obj");


    // Create Screen
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "DIY 3D Engine");
    //DisableCursor(); // For the FPS controls that will come later
    
    float fAspectRatio = (float)GetScreenHeight() / (float)GetScreenWidth();
    mat4x4 matProj = getProjectionMatrix(90.0f,fAspectRatio);
    float fTheta = 0.0f;

    Vector3 camera = {0,0,0}; //Super Basic Camera, will be replaced


    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------


        fTheta += 1 * GetFrameTime();
        Vector3 rotator = {fTheta,fTheta * 0.5f,0.0f};

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            std::vector<Triangle> trianglesToRaster;

            //Get Triangles We Want To Draw
            for (auto tri : meshCube.tries)
            {
                Triangle triRotated, triTranslated, triProjected;

                triRotated = ApplyTriangleRotation(tri,rotator);

                //Translate triangle into view
                triTranslated = triRotated;
                triTranslated.points[0].z = triRotated.points[0].z + 5.0f;
                triTranslated.points[1].z = triRotated.points[1].z + 5.0f;
                triTranslated.points[2].z = triRotated.points[2].z + 5.0f;
                
                // Calculate Normals
                Vector3 normal, line1, line2;

                line1.x = triTranslated.points[1].x - triTranslated.points[0].x;
                line1.y = triTranslated.points[1].y - triTranslated.points[0].y;
                line1.z = triTranslated.points[1].z - triTranslated.points[0].z;

                line2.x = triTranslated.points[2].x - triTranslated.points[0].x;
                line2.y = triTranslated.points[2].y - triTranslated.points[0].y;
                line2.z = triTranslated.points[2].z - triTranslated.points[0].z;

                normal.x = line1.y * line2.z - line1.z * line2.y;
                normal.y = line1.z * line2.x - line1.x * line2.z;
                normal.z = line1.x * line2.y - line1.y * line2.x;

                float normalLength = std::sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
                normal.x /= normalLength; normal.y /= normalLength; normal.z /= normalLength;

                // Normal Culling
                if(normal.x * (triTranslated.points[0].x - camera.x) +
                   normal.y * (triTranslated.points[0].y - camera.y) +
                   normal.z * (triTranslated.points[0].z - camera.z) < 0.0f) // Accounting for where the camera is positioned, and can actually see. And as a result where the triangle is in 3d space
                {
                    // Hyper basic lighting, should be improved later
                    Vector3 lightDir = {-1.0f,0.0f,-1.0f}; 

                    // For this simple lighting, the more lit a triangle is the more the normal is aligned with the light direction.
                    //Normalise light normal
                    float lightNormal = std::sqrt(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
                    lightDir.x /= lightNormal; lightDir.y /= lightNormal; lightDir.z /= lightNormal;            

                    float lightDP = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;
           

                    // Projection from 3D -> 2D Screen Space
                    triProjected = projectTriangleTo3D(triTranslated,matProj);
                    
                    //Scale into view
                    triProjected.points[0].x += 1.0f; triProjected.points[0].y += 1.0f;
                    triProjected.points[1].x += 1.0f; triProjected.points[1].y += 1.0f;
                    triProjected.points[2].x += 1.0f; triProjected.points[2].y += 1.0f;

                    triProjected.points[0].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[0].y *= 0.5 * (float)GetScreenHeight();
                    triProjected.points[1].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[1].y *= 0.5 * (float)GetScreenHeight();
                    triProjected.points[2].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[2].y *= 0.5 * (float)GetScreenHeight();

                    //Store all the good triangles
                    triProjected.col = getColourFromLuminance(lightDP);
                    trianglesToRaster.push_back(triProjected);


                }
            }

            //Sort Valid Triangles - Painter's Algorithm
            std::sort(trianglesToRaster.begin(),trianglesToRaster.end(),[](Triangle &t1,Triangle &t2)
            {
                float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
                float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
                return z1 > z2;
            });

            //Draw Triangles
            for (auto &triProjected : trianglesToRaster){
                {
                    DrawTriangle({triProjected.points[0].x,triProjected.points[0].y},
                                {triProjected.points[1].x,triProjected.points[1].y},
                                {triProjected.points[2].x,triProjected.points[2].y},
                                 triProjected.col);

                }
            }
            

            DrawFPS(20,20);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }



    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    return 0;
}