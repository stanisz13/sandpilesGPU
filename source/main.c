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

    PingpongBuffer pbuffer;
    configurePingpongBuffer(&contextData, &pbuffer);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pbuffer.fbo[i]);
        glBindTexture(GL_TEXTURE_2D, pbuffer.texture[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                     contextData.windowWidth, contextData.windowHeight,
                     0, GL_RED, GL_FLOAT, i == 0 ? pixels : 0);
    }

    ScreenQuad squad;
    configureScreenQuad(&squad);
    
    unsigned basic = createBasicProgram();
    unsigned step = createStepProgram();

    int old = 0, now = 1;

        
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
        glUseProgram(step);
        glBindFramebuffer(GL_FRAMEBUFFER, pbuffer.fbo[now]);
        glBindTexture(GL_TEXTURE_2D, pbuffer.texture[old]);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);

        //NOTE(Stanisz13):
        //Begin drawing on the 0th framebuffer - screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //NOTE(Stanisz13):
        //Draw on the 0th framebuffer using the data that filled
        //the current fbo (its texture precisely)
        glUseProgram(basic);
        glBindTexture(GL_TEXTURE_2D, pbuffer.texture[now]);
        glBindVertexArray(squad.VAO);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
        
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
