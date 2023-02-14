#include <FL/Fl.h>
#include <FL/gl.h>

#include "PsmView.h"
#include "PsmMain.h"

#include "ImageOps.h"
#include "ImageLib/FileIO.h"

#ifdef _WIN32
#define GL_BGR  GL_BGR_EXT
#define GL_BGRA GL_BGRA_EXT
#endif

///////////////////////////////////////////////////
// PsmView::draw(): respond to redraw requests
//
void
PsmView::draw()
{
    static bool firsttime = true;
    if (firsttime)
    {
	setupTextures();
	setupLighting();
	firsttime = false;
    }
	
    if (!valid())
    {
	glViewport(0,0,w(),h());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w(),0,h(),-20030,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

//    cout << transX << " " << transY << endl;

    glTranslatef((float)transX, -(float)transY, 0);
    // glRotatef(hAng,0,1,0); glRotatef(vAng,1,0,0);
    glScalef(float(zoom),float(zoom),float(zoom));

    drawContents();

    glPopMatrix();
}
 
//////////////////////////////////////////////////////
// PsmView::handle(): respond to user events
//
int
PsmView::handle(int event)
{
    int *shiftX = 0, *shiftY = 0;
    int button = Fl::event_button();
    switch(button) {
    case 1:	shiftX = &rotX;   shiftY = &rotY;   break;
    case 2:	shiftX = &lightX; shiftY = &lightY; break;
    case 3:	shiftX = &transX; shiftY = &transY; break;
    default:	break;
    }

    switch(event) {
    case FL_PUSH:    // mouse down
	if ((button == 3) || mode == SURFACE)
	{
	    startX = Fl::event_x() - *shiftX;
	    startY = Fl::event_y() - *shiftY;
	}
	return 1;
    case FL_DRAG:    // mouse moved while down
	if ((button == 3) || mode == SURFACE)
	{
	    *shiftX = Fl::event_x() - startX;
	    *shiftY = Fl::event_y() - startY;
	    redraw();
	}
	return 1;
//    case FL_MOVE:    // mouse moved while up
    case FL_RELEASE: // mouse up
	return 1;    // do nothing
    case FL_FOCUS:
    case FL_UNFOCUS:
	return 1;
    case FL_KEYBOARD:
    case FL_SHORTCUT:
	if ((Fl::event_key() == '=') && (Fl::event_ctrl()))
	{
	    zoom *= 2;
	    transX = 2*transX - Fl::event_x();
	    transY = 2*transY + (h() - Fl::event_y());
	}
	else if ((Fl::event_key() == '-') && (Fl::event_ctrl()))
	{
	    zoom /= 2;
	    transX = (transX + Fl::event_x())/2;
	    transY = (transY - (h() - Fl::event_y()))/2;
	}
	else if ((Fl::event_key() == '0'))
	{
	    zoom = 1;
	    transX = transY =0;
	    rotX = rotY = 0;
	    lightX = lightY = 0;
	}
	redraw();
	return 1;
    default:         // pass other events to the base class
	return Fl_Gl_Window::handle(event);
    }
}

////////////////////////////////////////////////////////
// PsmView::drawContents():  Once the camera parameters
//   have been set, draw the mode-dependent contents
//   of the image.
//
void
PsmView::drawContents() const
{
    switch(mode)
    {
    case SOURCES:
    case ALBEDOS:
    case NORMALS:
	drawImage();
	return;
    case SURFACE:
	drawSurface();
	return;
    case NEEDLES:
	drawNeedles();
	return;
    case DEPTHS:
	return;
    default:
	return;
    }

}

//////////////////////////////////////////////////////
// PsmView::drawImage(): draw a 2D image as a textured
//   quad.  Assume the texture has already been bound
//   to textureMap.
//
void
PsmView::drawImage() const
{
//    cout << "drawImage" << endl;

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    glBegin(GL_QUADS);
    glColor3f(1,0,0);
    glTexCoord2f(0,        0        ); glVertex2i(0,       0        );
    glTexCoord2f(subWidth, 0        ); glVertex2i(bufWidth,0        );
    glTexCoord2f(subWidth, subHeight); glVertex2i(bufWidth,bufHeight);
    glTexCoord2f(0,        subHeight); glVertex2i(0,       bufHeight);
    glEnd();
    glFlush();

    glDisable(GL_TEXTURE_2D);
}

///////////////////////////////////////////////////////////
// PsmView::meshVertex(): draw one vertex of a mesh with
//   normals and texture coordinates
//
void
PsmView::meshVertex(int i, int j,const ColorImage * depths,
		    const NormalImage * normals) const
{
    if (normals)
	glNormal3f(normals->Pixel(i,j,0).x,
		   normals->Pixel(i,j,0).y,
		   normals->Pixel(i,j,0).z);
    glTexCoord2f(float(i) / padWidth,
		 float(j) / padHeight);
    glVertex3f((float)i,(float)j,depths->Pixel(i,j,0));
}

///////////////////////////////////////////////////////////
// PsmView::setupLighting(): initialize OpenGL lights
// and materials.
//
void 
PsmView::setupLighting() const
{ 
  glDepthFunc(GL_LEQUAL);

  const GLfloat mat_diffuse[] =  { 1.0, 1.0, 1.0, 1.0 };
  
  glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
  glMaterialfv(GL_BACK,GL_DIFFUSE,mat_diffuse);
  
  glShadeModel(GL_SMOOTH);
}

//////////////////////////////////////////////////////////
// PsmView::drawSurface(): draw a surface as a (possibly
//  textured) mesh.
//
void
PsmView::drawSurface() const
{
    const DepthImage *depths = theApp->getDepths();
    if (!depths) return;

    int width = depths->Shape().width;
    int height = depths->Shape().height;

    const NormalImage *normals = theApp->getShadingNormals();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    GLfloat light_pos0[4] = { sin(lightX/360.0f),
			      -sin(lightY/360.0f),
			      cos(lightX/360.0f), 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION,light_pos0);

    glTranslatef(float(width/2),float(height/2),0);
    glRotatef(float(rotX), 0.f, 1.f, 0.f); glRotatef(float(rotY), 1.f, 0.f, 0.f);
    glTranslatef(float(-width/2),float(-height/2),0);

    if (theUI->useAlbedos->value())
    {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glColor3f(1,1,1);
    }
    else
	glColor3f(.5,.5,.5);

	const ColorImage *mask = theApp->getMask();

    glBegin(GL_QUADS);
    for(int i=0;i<width-1;i++)
	for(int j=0;j<height-1;j++)
	{
		if (mask && !mask->Pixel(i,j,0))
			continue;
	    meshVertex(i,j,depths,normals);
	    meshVertex(i+1,j,depths,normals);
	    meshVertex(i+1,j+1,depths,normals);
	    meshVertex(i,j+1,depths,normals);
	}
    
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////
// PsmView::drawNeedles(): use normal information to
//   draw a needle map.
//
void
PsmView::drawNeedles() const
{
    const int vectorStep = (int)theUI->needleSpacing->value();
    const int vectorSize = (int)(vectorStep * .9);

    glColor3ub(255,255,255);
    glPointSize(2);

    const NormalImage *normals = theApp->getNormals();

    int width = normals->Shape().width;
    int height = normals->Shape().height;

    for(int i=0;i<width;i+=vectorStep)
	for(int j=0;j<height;j+=vectorStep)
	{
	    Vec3f vec = normals->Pixel(i,j,0);
	    if (vec.z != vec.z) continue;   // NaN
	    if ((vec.x == 0) && (vec.y == 0) && (vec.z == 0))
		continue; // empty

	    vec = vec.normalized()*(float)vectorSize;
	    vec.z = 0;

	    glBegin(GL_LINES);
	    glVertex2i(i,j);
	    glVertex2f(i + vec.x, j + vec.y);
	    glEnd();  

	    glBegin(GL_POINTS);
	    glVertex2f(i + vec.x, j + vec.y);
	    glEnd();  
      }
}


//////////////////////////////////////////////////////////
// PsmView::setupTextures():  initialize OpenGL textures
//  and texture parameters
//
void PsmView::setupTextures()
{
    cout << "setup textures" << endl;

    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureMap);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		    GL_NEAREST);
    glDisable(GL_TEXTURE_2D);

}
	
// round an integer up to the next power of 2
inline int roundup(int n)
{
  int val = 1;
  while (val < n) val <<= 1;
  return val;
}

/////////////////////////////////////////////////////////////////
// PsmView::whichImage(): determine, based on mode, which image
//   to display next.  Should be called only when the buffer is
//   being updated.
//
const ColorImage *
PsmView::whichImage() const
{

    switch(mode)
    {
    case SOURCES:
	if (theUI->showMask->value())
	    return theApp->getMask();
	else if (theApp->hasSources())
	    return theApp->getSource((int)theUI->imageNum->value());
	else
	    return 0;
    case NORMALS:
    {
	const NormalImage *normals = theApp->getNormals();
	if (!normals) return 0;
	CShape sh = normals->Shape();
	sh.nBands = 3;
	normalColors.ReAllocate(sh,true);
	encodeNormalsAsColors(theApp->getNormals(), &normalColors);
	return &normalColors;
    }
    case ALBEDOS:
    case SURFACE:
	return theApp->getAlbedos();
    default:
	return 0;
    }
}
    
//////////////////////////////////////////////////////////
// PsmView::updateBuffer():  The user has chosen an event
//   which may change the image or texture being displayed.
//   Replace the OpenGL texture map with the new contents.
//
void
PsmView::updateBuffer()
{
    const ColorImage *img = whichImage();

    int nBands = 3;
    if (img)
    {
	bufWidth = img->Shape().width;
	bufHeight = img->Shape().height;
	nBands = img->Shape().nBands;
    }

    if ((nBands < 3) || (nBands > 4))
    {
	cout << "ERROR: can only handle 3 or 4 channels" << endl;
	return;
    }
    
    padWidth = roundup(bufWidth);
    padHeight = roundup(bufHeight);
    
    subWidth = float(bufWidth)/padWidth;
    subHeight = float(bufHeight)/padHeight;
    
    float *padImg = new GLfloat[padWidth*padHeight*nBands];
    //bzero(padImg,padWidth*padHeight*nBands*sizeof(GLfloat));
	memset(padImg,0,padWidth*padHeight*nBands*sizeof(GLfloat));
    glEnable(GL_TEXTURE_2D);
    if (!img)
	glTexImage2D(GL_TEXTURE_2D, 0, 3, padWidth, padHeight, 0, GL_BGR,
		     GL_FLOAT, padImg);
    else if (nBands == 4)
    {
	glTexImage2D(GL_TEXTURE_2D, 0, 4, padWidth, padHeight, 0, GL_BGRA,
		     GL_FLOAT, padImg);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bufWidth, bufHeight,
			GL_BGRA, GL_FLOAT, img->PixelAddress(0,0,0));
    } else {
	glTexImage2D(GL_TEXTURE_2D, 0, 3, padWidth, padHeight, 0, GL_BGR,
		     GL_FLOAT, padImg);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bufWidth, bufHeight,
			GL_BGR, GL_FLOAT, img->PixelAddress(0,0,0));
    }

    delete padImg;
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    
//    cout << "updateBuffer: (" << bufWidth << ", " << bufHeight << ") ";
//    cout << "(" << padWidth << ", " << padHeight << ")" << endl;

    glDisable(GL_TEXTURE_2D);
    
    redraw();
//    shiftX = (w()-img->Shape().width)/2;
//    shiftY = -(h()-img->Shape().height)/2;

}

////////////////////////////////////////////////////////
// PsmView::saveSnapshot(): save the contents of the
//   current view to a TGA file.
//
void
PsmView::saveSnapshot(const char *filename) const
{
    CByteImage temp(w(),h(),3);
    glReadPixels(0,0,w(),h(),
		 GL_BGR,GL_UNSIGNED_BYTE,
		 temp.PixelAddress(0,0,0));
    WriteFile(temp,filename);
}
