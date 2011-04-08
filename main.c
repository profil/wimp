/* WIMP - WIndow Manager Profil. Thanks to scp1!
 * A very simple window manager for the X Window System.
 *
 * Copyright (C) 2010 profil
 * http://libs.se/wm/
 *
 * See README for help
 *
 * LICENSE?!
 * Use it, share it. Don't be evil.
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MODKEY Mod4Mask 	/* Mod4Mask == windows key */
#define BORDERCOLOR #367fc5	/* Color of borders */
#define MOVE 30			/* Pixels to move each step */


Display *dpy;
Window root;
XEvent ev;
XColor color;
static int screen, sw, sh;
int running, i=0;

int init();
void quit();
void grabkeys();
void keypress(XEvent *e);
void expose(XEvent *e);
void focus();
void move(int direction);
void term();
void maximize();

void grabkeys() {

	/* For now, TODO: add shortcuts. */
	printf("wimp: grabbing keys\n");
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Up")), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Down")), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Left")), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Right")), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("t")), MODKEY, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("q")), MODKEY, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("c")), MODKEY, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("o")), MODKEY, root, True, GrabModeAsync, GrabModeAsync);
}

void keypress(XEvent *e) {
	XKeyEvent xke = e->xkey;
	KeySym keysym = XKeycodeToKeysym(dpy, xke.keycode, 0);

	printf("wimp: got keypress: %s\n",XKeysymToString(keysym));

	/*	TODO:
		As stated earlier, this is just for the time being. */
	if(xke.state == MODKEY) {
		if(keysym == XStringToKeysym("t")) {
			term();
		}
		else if(keysym == XStringToKeysym("q")) {
			printf("wimp: mod+q pressed: quitting!\n");
			quit();
		}
		else if(keysym == XStringToKeysym("c")) {
			focus();
		}
		else if(keysym == XStringToKeysym("o")) {
			maximize();
		}
	}
	else if(xke.state == (MODKEY|ShiftMask)) {
		if(keysym == XStringToKeysym("Up")) {
			move(1);
		}
		else if(keysym == XStringToKeysym("Right")) {
			move(2);
		}
		else if(keysym == XStringToKeysym("Down")) {
			move(3);
		}
		else if(keysym == XStringToKeysym("Left")) {
			move(4);
		}
	}
}

void term() {
if(fork() == 0) {
		if(fork() == 0) {
			if(dpy) {
				close(ConnectionNumber(dpy));
			}

			setsid();
			execl("/usr/bin/urxvt", "urxvt", 0);

		}
		exit(0);
	}
	focus();
}


void focus() {
	Window *children, root_ret, par_ret;
	unsigned int n=0;

	XQueryTree(dpy, root, &root_ret, &par_ret, &children, &n);
	printf("wimp: children: %u\ti: %d\n", n, i);

	if(n < 1) {
		return;
		XFree(children);
	}
	else if(i+1 >= n) {
		i=0;
		XRaiseWindow(dpy, children[i]);
		XSetWindowBorder(dpy, children[i], color.pixel);
		XSetInputFocus(dpy, children[i] , RevertToParent, CurrentTime);
		XFree(children);
	}
	else {
		i++;
		XRaiseWindow(dpy, children[i]);
		XSetWindowBorder(dpy, children[i], color.pixel);
		XSetInputFocus(dpy, children[i] , RevertToParent, CurrentTime);
		XFree(children);
	}

}


/* 1==up, 2==right, 3==down, 4==left; */
void move(int direction) {
	Window focuswin;
	Window root_return;
	int revert, x , y;
	unsigned int wr, hr, bwr, dr;
	if(!XGetInputFocus(dpy, &focuswin, &revert)) {
		printf("wimp: error: ops, no window got focus\n");
		return;
	}

	XGetGeometry(dpy, focuswin, &root_return, &x, &y, &wr, &hr, &bwr, &dr);

	if(direction == 1) {
		XMoveWindow(dpy, focuswin, x, y-MOVE);
	}
	else if(direction == 2) {
		XMoveWindow(dpy, focuswin, x+MOVE, y);
	}
	else if(direction == 3) {
		XMoveWindow(dpy, focuswin, x, y+MOVE);
	}
	else if(direction == 4) {
		XMoveWindow(dpy, focuswin, x-MOVE, y);
	}
}


void maximize() {
	Window focuswin;
	int revert;
	if(!XGetInputFocus(dpy, &focuswin, &revert)) {
		printf("wimp: error: ops, no window got focus\n");
		return;
	}

	if(!XMoveResizeWindow(dpy, focuswin, 0, 0, sw, sh)) {
		printf("wimp: error: maximizing window\n");
		return;
	}
}


int init() {
	screen = DefaultScreen(dpy);
	root = DefaultRootWindow(dpy);
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);
	if(!XAllocNamedColor(dpy, DefaultColormap(dpy, screen), "#367fc5",&color, &color)) {
		printf("wimp: error: allocating color\n");
		return 1;
	}
	printf("%lu\n",color.pixel);
	running = 1;
	return 0;
}

void quit() {
	running = 0;
	printf("wimp: quit\n");
}


int main(int argc, char *argv[]) {

	if(!(dpy = XOpenDisplay(NULL))) {
		printf("wimp: error: Can't open display!\n");
		return 1;
	}
	if(init()) {
		printf("fail\n");
		return 1;
	}
	grabkeys();


	/* This is the main loop! */
	printf("wimp: Starting main event loop.\n");
	while(running) {
		XNextEvent(dpy, &ev);
		switch(ev.type) {
			case KeyPress:
				keypress(&ev);
			break;
		}
	}
	XCloseDisplay(dpy);
	return 0;
}
