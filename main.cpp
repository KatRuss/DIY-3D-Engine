#include "engine.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    //Test cube - will replace later
    basicMesh meshCube;
    meshCube.tries = 
    {

        // SOUTH FACE
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST FACE                                             
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH FACE                                                   
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST FACE                                                   
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP FACE                                                 
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM FACE                                                  
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
    
    };


    // Create Screen
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "DIY 3D Engine");
    //DisableCursor(); // For the FPS controls that will come later
    
    float fAspectRatio = (float)GetScreenHeight() / (float)GetScreenWidth();
    mat4x4 matProj = getProjectionMatrix(90.0f,fAspectRatio);
    float fTheta = 0.0f;

    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------


        fTheta += 1 * GetFrameTime();
        Vector3 rotator = {fTheta,0.0f,fTheta * 0.5f};

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            //Draw Triangles
            for (auto tri : meshCube.tries)
            {
                Triangle triRotated, triTranslated, triProjected;

                triRotated = ApplyTriangleRotation(tri,rotator);

                //Translate triangle into view
                triTranslated = triRotated;
                triTranslated.points[0].z = triRotated.points[0].z + 3.0f;
                triTranslated.points[1].z = triRotated.points[1].z + 3.0f;
                triTranslated.points[2].z = triRotated.points[2].z + 3.0f;
                
                // Projection
                triProjected = projectTriangleTo3D(triTranslated,matProj);

                //Scale into view
                triProjected.points[0].x += 1.0f; triProjected.points[0].y += 1.0f;
                triProjected.points[1].x += 1.0f; triProjected.points[1].y += 1.0f;
                triProjected.points[2].x += 1.0f; triProjected.points[2].y += 1.0f;

                triProjected.points[0].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[0].y *= 0.5 * (float)GetScreenHeight();
                triProjected.points[1].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[1].y *= 0.5 * (float)GetScreenHeight();
                triProjected.points[2].x *= 0.5 * (float)GetScreenWidth(); triProjected.points[2].y *= 0.5 * (float)GetScreenHeight();

                DrawTriangleLines({triProjected.points[0].x,triProjected.points[0].y},
                            {triProjected.points[1].x,triProjected.points[1].y},
                            {triProjected.points[2].x,triProjected.points[2].y},
                            RAYWHITE);
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