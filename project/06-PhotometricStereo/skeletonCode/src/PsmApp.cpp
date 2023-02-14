#include <iostream>
#include <fstream>
#include <assert.h>

#include "PsmMain.h"
#include "PsmApp.h"
#include "binary.h"

#include "LightDirs.h"
#include "Normals.h"
#include "Albedos.h"
#include "Depths.h"

#include "ImageOps.h"
#include "ImageLib/FileIO.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// PsmApp Destructor: deletes all internal data structures
//
PsmApp::~PsmApp()
{
    // delete all source images
    for (unsigned int i = 0; i<sources.size(); i++)
    {
	if (sources[i]) delete sources[i];
	sources[i] = 0;
    }
    sources.clear();

    // clear the light directions
    lightDirs.clear();

    // delete normals, albedos, and depths
    if (normals) delete normals; normals = 0;
    if (albedos) delete albedos; albedos = 0;
    if (depths)  delete depths;  depths  = 0;
}


//////////////////////////////////////////////////////////////////////////
// PsmApp loadSources: loads images specified in text file
//
//   Note: argument sourceFile is not the name of an image file!  It's the
//         name of a text file in which the first line is a number of
//         images, and each subsequent line is an image filename.
//
bool
PsmApp::loadSources(const char *sourceFile)
{
    ifstream fp(sourceFile);

    int numImages;
    fp >> numImages;

    if (numImages <= 0 || numImages > 100)
    {
	cerr << "ERROR: bad source image file " << sourceFile << endl;
	return false;
    }

    // clear the old sources
    if (sources.size())
    {
	for (int i = 0; i < numImages; i++)
	{
	    delete sources[i];
	    sources[i] = 0;
	}
	sources.clear();
    }
    // clear the mask
    if (mask)
    {
	delete mask;
	mask = 0;
    }

    string imageFilename;
    for (int i = 0; i < numImages; i++)
    {
	fp >> imageFilename;
	if (!fileExists(imageFilename.c_str()))
	{
	    cerr << "ERROR: couldn't load image file " << imageFilename << endl;
	    return false;
	}
	if (ColorImage *image = getFloatImage(imageFilename.c_str()))
	    sources.push_back(image);
	else
	{
	    cerr << "ERROR: couldn't load image file " << imageFilename << endl;
	    return false;
	}
    }

    cerr << "Successfully loaded " << numImages << " image files." << endl;

    string maskFilename;
    fp >> maskFilename;

    if ((maskFilename != imageFilename) &&
	(maskFilename != "none") &&
	(maskFilename != ""))
    {
	if (!fileExists(imageFilename.c_str()))
	{
	    cerr << "ERROR: couldn't load mask file " << maskFilename << endl;
	    return false;
	}
	if (!(mask = getFloatImage(maskFilename.c_str())))
	{
	    cerr << "ERROR: couldn't load mask file " << maskFilename << endl;
	    return false;
	}
    }
	
    theUI->tabs->value(theUI->sourcesTab);
    theUI->sourcesTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::loadLightDirs: loads light directions specified in text file
//
//    lightDirsFile is an ASCII text file in which the first line is a
//    number of lighting directions, and each subsequent line is a triple
//    of floats.
//
bool
PsmApp::loadLightDirs(const char *lightDirsFile)
{
    ifstream fp(lightDirsFile);

    int numLightDirs;
    fp >> numLightDirs;

    if (numLightDirs < 0 || numLightDirs > 100)
    {
	cerr << "ERROR: bad lighting directions file " << lightDirsFile << endl;
	return false;
    }

    lightDirs.resize(numLightDirs);
    for (int i = 0; i < numLightDirs; i++)
    {
	Vec3f &lightDir = lightDirs[i];
	fp >> lightDir.x >> lightDir.y >> lightDir.z;
	if ((lightDir.length() < .5) || (lightDir.length() > 1.5))
	{
	    cerr << "WARNING: lighting direction " << lightDir;
	    cerr << " is not normalized." << endl;
	}
    }

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::loadNormals: loads normals specified as a binary data file
//
//   normalsFile is a binary data file with extension "dat".  The binary
//   data format is simply
//     int width;
//     int height;
//     float x,y,z;
//     float x,y,z;
//     float x,y,z;
//     ...
//
bool
PsmApp::loadNormals(const char *normalsFile)
{
    int width, height;
    FILE *fp = fopen(normalsFile,"rb");
    if (!fp)
    {
	cerr << "ERROR: Couldn't open binary normals file ";
	cerr << normalsFile << " for reading." << endl;
	return false;
    }
    
    if (!readBinary("width", &width, fp)) return false;
    if (!readBinary("height", &height, fp)) return false;
    
    if (width < 0 || width > 4096)
    {
	cerr << "Binary normals file has bad width " << width << endl;
	return false;
    }
    
    if (height < 0 || height > 4096)
    {
	cerr << "Binary normals file has bad width " << height << endl;
	return false;
    }
    
    if (normals)
    {
	delete normals;
	normals = 0;
    }

    normals = new NormalImage(width,height,1);
    for (int y = 0; y < height; y++)
	for (int x = 0; x < width; x++)
	{
	    Vec3f &n = normals->Pixel(x,y,0);
	    if (!readBinary("normal x",&n.x,fp)) return false;
	    if (!readBinary("normal y",&n.y,fp)) return false;
	    if (!readBinary("normal z",&n.z,fp)) return false;
	}
    
    fclose(fp);

    theUI->tabs->value(theUI->normalsTab);
    theUI->normalsTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::loadAlbedos: load albedo map from image file
//
bool
PsmApp::loadAlbedos (const char *albedosFile)
{
    if (albedos)
    {
	delete albedos;
	albedos = 0;
    }

    if (!(albedos = getFloatImage(albedosFile)))
    {
	cerr << "ERROR: couldn't load albedos from image file ";
	cerr << albedosFile << endl;
	return false;
    }

    theUI->tabs->value(theUI->albedoTab);
    theUI->albedoTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::loadDepths: load depth map from binary data file
//
//   depthsFile is a binary data file with extension "dat".  The binary data
//   format is simply
//     int width;
//     int height;
//     float z;
//     float z;
//     float z;
//     ...
//
bool
PsmApp::loadDepths(const char *depthsFile)
{
    int width, height;
    FILE *fp = fopen(depthsFile,"rb");
    if (!fp)
    {
	cerr << "ERROR: Couldn't open binary depths file ";
	cerr << depthsFile << " for reading." << endl;
	return false;
    }

    if (!readBinary("width", &width, fp)) return false;
    if (!readBinary("height", &height, fp)) return false;
    
    if (width < 0 || width > 4096)
    {
	cerr << "Binary depths file has bad width " << width << endl;
	return false;
    }
    
    if (height < 0 || height > 4096)
    {
	cerr << "Binary depths file has bad width " << height << endl;
	return false;
    }

    if (depths)
    {
	delete depths;
	depths = 0;
    }

    depths = new DepthImage(width,height,1);
    for (int y = 0; y < height; y++)
	for (int x = 0; x < width; x++)
	{
	    float &z = depths->Pixel(x,y,0);
	    if (!readBinary("depth",&z,fp)) return false;
	}

    fclose(fp);

    computeShadingNormals();

    theUI->tabs->value(theUI->surfaceTab);
    theUI->surfaceTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::saveLightDirs:  save lighting directions to ascii file
//
//    lightDirsFile is an ASCII text file in which the first line is a number
//    of lighting directions, and each subsequent line is a triple of floats.
//
bool
PsmApp::saveLightDirs(const char *lightDirsFile) const
{
    ofstream fp(lightDirsFile);  // TODO error conditions?

    fp << (unsigned int) lightDirs.size() << endl;

    for (unsigned int i = 0; i < lightDirs.size(); i++)
    {
	fp << lightDirs[i].x << " ";
	fp << lightDirs[i].y << " ";
	fp << lightDirs[i].z << endl;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::saveNormals: saves normals into a binary data file
//
//
//   normalsFile is a binary data file with extension "dat".  The binary data
//   format is simply
//     int width;
//     int height;
//     float x,y,z;
//     float x,y,z;
//     float x,y,z;
//     ...
//
bool
PsmApp::saveNormals(const char *normalsFile) const
{

    if (!normals)
    {
	cerr << "ERROR: No normals have been computed;";
	cerr << " cannot save to file " << normalsFile << endl;
	return false;
    }

    FILE *fp = fopen(normalsFile,"wb");
    if (!fp)
    {
	cerr << "ERROR: Couldn't open binary normals file ";
	cerr << normalsFile << " for writing." << endl;
	return false;
    }

    int width = normals->Shape().width;
    int height = normals->Shape().height;
    if (!writeBinary("width", &width, fp)) return false;
    if (!writeBinary("height", &height, fp)) return false;
    
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    Vec3f &n = normals->Pixel(x,y,0);
	    if (!writeBinary("normal x",&n.x,fp)) return false;
	    if (!writeBinary("normal y",&n.y,fp)) return false;
	    if (!writeBinary("normal z",&n.z,fp)) return false;
	}
    fclose(fp);

#ifdef DEBUG
    ColorImage nc(width,height,3);
    encodeNormalsAsColors(normals,&nc);
    saveFloatImage(&nc,"/tmp/normals.tga");
#endif // DEBUG

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::saveAlbedos: save albedo map to image file
//
bool
PsmApp::saveAlbedos (const char *albedosFile) const
{
    if (!albedos)
    {
	cerr << "ERROR: no albedos computed;";
	cerr << " cannot save to file " << albedosFile << endl;
	return false;
    }

    saveFloatImage(albedos,albedosFile);
	return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::saveDepths: save depth map to binary data file
//
//   depthsFile is a binary data file with extension "dat".  The binary data
//   format is simply
//     int width;
//     int height;
//     float z;
//     float z;
//     float z;
//     ...
//
bool
PsmApp::saveDepths (const char *depthsFile) const
{
    if (!depths)
    {
	cerr << "ERROR: no depths computed;";
	cerr << " cannot save to file " << depthsFile << endl;
	return false;
    }

    FILE *fp = fopen(depthsFile,"wb");
    if (!fp)
    {
	cerr << "ERROR: Couldn't open binary depths file ";
	cerr << depthsFile << " for writing." << endl;
	return false;
    }

    int width = depths->Shape().width;
    int height = depths->Shape().height;
    if (!writeBinary("width", &width, fp)) return false;
    if (!writeBinary("height", &height, fp)) return false;

    for (int y = 0; y < depths->Shape().height; y++)
	for (int x = 0; x < depths->Shape().width; x++)
	{
	    float &z = depths->Pixel(x,y,0);
	    if (!writeBinary("depth",&z,fp)) return false;
	}

    fclose(fp);

#ifdef DEBUG
    ColorImage dc(width,height,3);
    encodeDepthsAsColors(depths,&dc);
    saveFloatImage(&dc,"/tmp/depths.tga");
#endif    

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::quit:  Check for unsaved files, then quit
//
void
PsmApp::quit(int status)
{
    // TODO check for unsaved files
    exit(status);
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::computeLightDirs:  Use images of chrome spheres to determine
//   lighting directions.  Return true if successful.
//
bool
PsmApp::computeLightDirs()
{
    if(!canComputeLightDirs())
	return false;

	lightDirs.clear();
    for (unsigned int i = 0; i < sources.size(); i++)
	lightDirs.push_back(computeOneLightDir(sources[i],mask));

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::computeNormals:  Use photometric stereo to determine per-pixel
//   normals.  Return true if successful.
//
bool
PsmApp::computeNormals()
{
    if (!canComputeNormals())
	return false;

    if (normals && !normals->Shape().SameIgnoringNBands(sources[0]->Shape()))
    {
	delete normals;
	normals = 0;
    }

    int width = sources[0]->Shape().width;
    int height = sources[0]->Shape().height;
    if (!normals)
	normals = new NormalImage(width,height,1);

    normals->ClearPixels();

    vector<float> values;
    values.resize(sources.size());
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    if (mask && !mask->Pixel(x,y,0))
		continue;

	    for (unsigned int i = 0; i<sources.size(); i++)
		values[i] = sources[i]->Pixel(x,y,1);
	    normals->Pixel(x,y,0) = computeOneNormal(values,lightDirs);
	}

    theUI->tabs->value(theUI->normalsTab);
    theUI->normalsTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::computeAlbedos:  Use known normals to compute color albedos.
//   Return true if successful.
//
bool
PsmApp::computeAlbedos()
{
    if(!canComputeAlbedos())
	return false;

    if (albedos &&
	!albedos->Shape().SameIgnoringNBands(normals->Shape()))
    {
	delete albedos;
	albedos = 0;
    }

    int width = normals->Shape().width;
    int height = normals->Shape().height;

    if (!albedos)
	albedos = new ColorImage(width,height,3);

    albedos->ClearPixels();

    vector<float> values;
    values.resize(sources.size());
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    if (mask && !mask->Pixel(x,y,0))
		continue;
	    for (int c = 0; c<3; c++)
	    {
		for (unsigned int i = 0; i<sources.size(); i++)
		    values[i] = sources[i]->Pixel(x,y,c);
		albedos->Pixel(x,y,c) =
		    computeOneAlbedo(values,lightDirs,normals->Pixel(x,y,0));
	    }
	}

    theUI->tabs->value(theUI->albedoTab);
    theUI->albedoTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::computeDepths:  Use known normals to compute depths.
//    Return true if successful.
//
bool
PsmApp::computeDepths()
{
    if(!canComputeDepths())
	return false;

    // make sure shape is compatible
    if (depths && (depths->Shape() != normals->Shape()))
    {
	delete depths;
	depths = 0;
    }

    int width = normals->Shape().width;
    int height = normals->Shape().height;

    if (!depths)
	depths = new DepthImage(width,height,1);

    depths->ClearPixels();

    ::computeDepths(depths,normals,mask);

    computeShadingNormals();

    theUI->tabs->value(theUI->surfaceTab);
    theUI->surfaceTab->do_callback();

    updateUI();

    return true;
}

///////////////////////////////////////////////////////////////////////////
// PsmApp::computeShadingNormals:  given newly loaded or computed
// depths, compute the "shading normals" for this depth buffer, so
// that surface renderings reflect the normals of an integrable surface
//
bool
PsmApp::computeShadingNormals()
{
    assert(depths);

    // check recomputed normals buffer, reallocate if necessary,
    // and recompute
    if (shadingNormals && (shadingNormals->Shape() != depths->Shape()))
    {
	delete shadingNormals;
	shadingNormals = 0;
    }

    int width = depths->Shape().width;
    int height = depths->Shape().height;

    if (!shadingNormals)
	shadingNormals = new NormalImage(width,height,1);

    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    Vec3f &n = shadingNormals->Pixel(x,y,0);
	    if (mask && !mask->Pixel(x,y,0))
		n = Vec3f(0,0,1);
	    else
	    {
		Vec3f dx = Vec3f(1,0,depths->Pixel(x+1,y,0) - depths->Pixel(x,y,0));
		Vec3f dy = Vec3f(0,1,depths->Pixel(x,y+1,0) - depths->Pixel(x,y,0));
		n = dx.cross(dy).normalized();
	    }
	}

    return true;
}

    
///////////////////////////////////////////////////////////////////////////
// PsmApp::updateUI:  Update the UI based on the application state
//  (This really should be in the UI class but it's unwieldy to code in
//   fluid so I just moved it into this codependent class instead.)
//
void
PsmApp::updateUI() const
{
    if ( hasSources()   )
    {
	theUI->imageNum->maximum( sources.size()-1 );
	theUI->imageNum->redraw();

	theUI->sourcesTab ->activate();
	theUI->sourcesTab ->redraw  ();
    }

    if ( hasMask() )
	theUI->showMask->activate();
    else
    {
	theUI->showMask->value(0);
	theUI->showMask->deactivate();
    }

    if ( hasLightDirs() )
    {
	theUI->saveLights ->activate();
    }

    if ( hasNormals()   )
    {
	theUI->saveNormals->activate();
	theUI->normalsTab ->activate();
	theUI->normalsTab ->redraw  ();
    }

    if ( hasAlbedos()   )
    {
	theUI->saveAlbedos->activate();
	theUI->albedoTab  ->activate();
	theUI->albedoTab  ->redraw  ();
    }

    if ( hasDepths()    )
    {
	theUI->saveDepths ->activate();
	theUI->surfaceTab ->activate();
	theUI->surfaceTab ->redraw  ();
	// TODO saveVRML
    }

    if (canComputeLightDirs()) theUI->solveLights ->activate();
    if (canComputeNormals  ()) theUI->solveNormals->activate();
    if (canComputeAlbedos  ()) theUI->solveAlbedos->activate();
    if (canComputeDepths   ()) theUI->solveDepths ->activate();

    theUI->view->updateBuffer();

//    Fl::flush();
    theUI->mainWindow->redraw();
}
