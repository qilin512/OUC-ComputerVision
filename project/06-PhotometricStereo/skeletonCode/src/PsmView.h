#ifndef _PSM_VIEW_H_
#define _PSM_VIEW_H_

#include <FL/Fl_Gl_Window.h>
#include <FL/gl.h>

#include "ImageTypes.h"

class PsmView : public Fl_Gl_Window {

public:

    typedef enum {
	SOURCES = 0,
	NORMALS,
	NEEDLES,
	ALBEDOS,
	DEPTHS,
	SURFACE,
	LIT_SURFACE
    } PsmMode;

    PsmView(int X, int Y, int W, int H, const char *L = 0)
	:  mode(SOURCES),
	transX(0), transY(0),
	rotX(0), rotY(0),
	lightX(100), lightY(-100),
	zoom(1),
	bufWidth(0), bufHeight(0),
	subWidth(1), subHeight(1),
	Fl_Gl_Window(X, Y, W, H, L) {}

    void updateBuffer();
    void setMode(PsmMode newMode) {mode = newMode; updateBuffer();}

    void saveSnapshot (const char *filename) const;

protected:

    void draw();
    int handle(int);

    void setupTextures();
    void setupLighting() const;

    void drawContents() const;
    void drawImage() const;
    void drawSurface() const;
    void drawNeedles() const;
    void meshVertex(int i, int j, const ColorImage *depths,
		    const NormalImage *normals) const;

    const ColorImage *whichImage() const;

private:
    int startX, startY;
    int transX, transY;
    int rotX, rotY;
    int lightX, lightY;
    float zoom;
    PsmMode mode;

    GLuint textureMap;
    int bufWidth, bufHeight;
    int padWidth, padHeight;
    float subWidth, subHeight;

    mutable ColorImage normalColors;
};

#endif // _PSM_VIEW_H_
