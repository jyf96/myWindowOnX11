#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <iostream>
#include <sys/time.h>
#include "agent.h"
int MyWindow::Init()
{
    display = XOpenDisplay(getenv("DISPLAY"));
    if (display == NULL)
    {
        printf("XOpenDisplay failed");
        return RET_ERR;
    }
    printf("%s\n", DisplayString(display));

    XSetWindowAttributes xWindowAttributes = {0};
    unsigned long xWindowAttributesMask = CWBackPixel | CWBorderPixel | CWOverrideRedirect;
    xWindowAttributes.background_pixel = 0xFFFFFF;
    xWindowAttributes.border_pixel = 0x0000FF;
    xWindowAttributes.override_redirect = false;
    window = XCreateWindow(display, XDefaultRootWindow(display),
                           0, 0, width, height, 0, CopyFromParent,
                           InputOutput, CopyFromParent, xWindowAttributesMask, &xWindowAttributes);
    XMapWindow(display, window);
    XSelectInput(display, window, KeyPressMask | ButtonPressMask | ExposureMask | FocusChangeMask);
    XGCValues gcValue = {0};
    gcValue.foreground = 0xFFFFFF;
    backgroundGc = XCreateGC(display, window, GCForeground, &gcValue);
    gcValue.foreground = 0x000000;
    foregroundGc = XCreateGC(display, window, GCForeground, &gcValue);
    XSelectInput(display, window, KeyPressMask | ButtonPressMask);
    return RET_OK;
}
MyWindow::~MyWindow()
{
    if (display != nullptr)
    {
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
}
void MyWindow::InternalThreadEntry()
{
    XEvent event;
    while (true)
    {
        XNextEvent(display, &event);
        printf("event.type = %d\n", event.type);
        switch (event.type)
        {

        case KeyPress:
        {
            KeySym keysym;
            char Buff[20] = {0};
            int CharaterNums = 0;
            CharaterNums = XLookupString(&(event.xkey), Buff, 20, &keysym, (XComposeStatus *)nullptr);
            if (CharaterNums != 0)
            {
                for (int i = 0; i < CharaterNums; i++)
                {
                    printf("text[%d]=%x %c\n", i, Buff[i], Buff[i]);
                }

                if (keysym == XK_q)
                {
                    printf("press %c,exit!\n", 'q');
                    return;
                }
            }
            break;
        }
        //鼠标按键
        case ButtonPress:
        {
            printf("ButtonPress :\n");
            unsigned int x = event.xbutton.x;
            unsigned int y = event.xbutton.y;
            int button = event.xbutton.button;
            printf("x = %d,y=%d,button=%d\n", x, y, button);
            switch (button)
            {
            case 1:
            { //鼠标左键
                // XDrawRectangle(display,window,gc,x-10,y-10,20,20);
                // XFillRectangle(display, window, gc, x - 10, y - 10, 20, 20);
                break;
            }
            case 3:
            { //鼠标右键
                // XChangeGC(display, gc, GCForeground, &gcValue);
                // XFillRectangle(display, window, gc, 0, 0, width, height);
                break;
            }
            default:
                break;
            }
            break;
        }

        default:
            break;
        }
    }
}