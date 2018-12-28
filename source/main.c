#include "graphics.h"
#include <math.h>

int isRunning = 1;

int main(int argc, char* argv[])
{
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 1600;
    contextData.windowHeight = 900;
    contextData.name = "Faith";

    configureOpenGL(&contextData);
    loadFunctionPointers();

    float* pixels = (float*)malloc(contextData.windowHeight * contextData.windowWidth * sizeof(float));
    pixels[contextData.windowHeight/ 2 * contextData.windowWidth + contextData.windowWidth / 2] = 10000;
    
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                     contextData.windowWidth, contextData.windowHeight,
                     0, GL_RED, GL_FLOAT, i == 0 ? pixels : 0);
            
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }        
    
    int old = 0, now = 1;

    unsigned VBO, VAO;

    float screenQuadVerts[] =
        {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
        };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    unsigned basic = createBasicProgram();
    unsigned step = createStepProgram();
    
    while(1)
    {        
        XEvent event;
        
        while (XPending(contextData.display))
        {
            XNextEvent(contextData.display, &event);
            switch (event.type)
            {
                case ClientMessage:
                    if (event.xclient.data.l[0] == contextData.deleteMessage)
                        isRunning = 0;
                    break;
            }
        }

        if (isRunning == 0)
        {
            break;
        }

        glClearColor(0, 0.5, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(step);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[now]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[old]);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glUseProgram(basic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[now]);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        int tmp = old;
        old = now;
        now = tmp;
        
        glXSwapBuffers(contextData.display, contextData.window);
        
        //sleep(1);
    }

    freeContextData(&contextData);
    free(pixels);
    return 0;
}
