#include "graphics.h"
#include <math.h>

int isRunning = 1;

int main(int argc, char* argv[])
{
    newLine();
    newLine();
    
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 400;
    contextData.windowHeight = 400;
    contextData.name = "Faith";

    configureOpenGL(&contextData);
    loadFunctionPointers();

    float* pixels = (float*)malloc(contextData.windowHeight * contextData.windowWidth * sizeof(float));
    pixels[contextData.windowHeight/ 2 * contextData.windowWidth + contextData.windowWidth / 2] = 900000;

    PingpongBuffer pbuffer;
    configurePingpongBuffer(&contextData, &pbuffer);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer_FA(GL_FRAMEBUFFER, pbuffer.fbo[i]);
        glBindTexture(GL_TEXTURE_2D, pbuffer.texture[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                     contextData.windowWidth, contextData.windowHeight,
                     0, GL_RED, GL_FLOAT, i == 0 ? pixels : 0);
    }

#define EBO 1
#if EBO == 1
    ScreenQuadWithEBO squad;
    configureScreenQuadWithEBO(&squad);
#else
    ScreenQuad squad;
    configureScreenQuad(&squad);
#endif

    unsigned basic = createBasicProgram();
    unsigned step = createStepProgram();

    int old = 0, now = 1;

    glDisable(GL_DEPTH_TEST);

    glXSwapIntervalMESA_FA(0);
    
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

        //glClearColor(0, 0.5, 1, 1);
        //glClear(GL_COLOR_BUFFER_BIT);

        //NOTE(Stanisz13):
        //Draw on the current fbo using the old texture
        glUseProgram_FA(step);

        for (unsigned i = 0; i < 1; ++i)
        {        
            glBindFramebuffer_FA(GL_FRAMEBUFFER, pbuffer.fbo[now]);
            glBindTexture(GL_TEXTURE_2D, pbuffer.texture[old]);
            
#if EBO == 1
            glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
#else
            glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
            int tmp = old;
            old = now;
            now = tmp;
        }
        //NOTE(Stanisz13):
        //Begin drawing on the 0th framebuffer - screen
        glBindFramebuffer_FA(GL_FRAMEBUFFER, 0);
        //NOTE(Stanisz13):
        //Draw on the 0th framebuffer using the data that filled
        //the current fbo (its texture precisely)
        glUseProgram_FA(basic);
        glBindTexture(GL_TEXTURE_2D, pbuffer.texture[now]);
#if EBO == 1
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
#else
        glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
        
        glXSwapBuffers(contextData.display, contextData.window);
        
        //sleep(1);
    }

    freeContextData(&contextData);
    free(pixels);

#if EBO == 1
    freeScreenQuadWithEBO(&squad);
#else
    freeScreenQuad(&squad);
#endif
    
    return 0;
}
