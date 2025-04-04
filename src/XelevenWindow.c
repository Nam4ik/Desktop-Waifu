//Compile to .so or .dll

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>

void GetScreen(){
    Display *display = XOpenDisplay(NULL); 
    if (!display) {
        perror("Unable to connect XEleven window server.");
    }

    Window root = DefaultRootWindow(display);
    XWindowAttributes attrs;
}


void GetWindow(Display *display, Window root, XWindowAttributes attrs){
  Window window;
  Window* children;
  //unsigned int nchildren;
  XGetWindowAttributes(display, root, &attrs);
  int x,y, width, height;

  if (!display){
    perror("Unable to connect XEleven server");
  }

  //XQueryTree(display, client_window, &root, &parent, &children, &nchildren);
  
  if (children) XFree(children);
  XGetWindowAttributes(display, window, &attrs);

  Atom actual_type;
  int actual_format;
  unsigned long bytes_after;
  unsigned char *data = NULL;
  Atom frame_extents = XInternAtom(display, "_NET_FRAME_EXTENTS", False);

  if (XGetWindowProperty(display, window, frame_extents, 
        0, 4, 
        False, 
        AnyPropertyType,              
        &actual_type, 
        &actual_format, 
        &bytes_after, 
        &bytes_after, 
        &data) == Success) {
    
    if (data && actual_format == 32 && bytes_after == 0) {
        long *extents = (long*)data;
        int left = extents[0];
        int right = extents[1];
        int top = extents[2];
        int bottom = extents[3];

      
        x -= left;
        y -= top;
        width += left + right;
        height += top + bottom;
    }
    XFree(data);
}

}

void GetAllWindows(Display *display, Window root){
    Window parent, *children;
    unsigned int nchildren;

while(1){
    if (XQueryTree(display, root, &root, &parent, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            GetScreen();
            GetWindow(display, children[i], attrs);
            GetAllWindows(display, children[i]);
        }
        XFree(children);
    }
 }
}
