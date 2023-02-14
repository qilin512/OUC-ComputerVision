#ifndef _PSM_APP_H_
#define _PSM_APP_H_

#include <vector>

#include "ImageLib/Image.h"
#include "ImageTypes.h"

class PsmApp {
public:
    // Constructor: initializes data members

    PsmApp() : mask(0), normals(0), albedos(0), depths(0),
	shadingNormals(0) {}

    // Destructor: Nuke it all

    ~PsmApp();

    // Load methods: load the corresponding data from files
    //   return true if successful

    bool loadSources   (const char *sourceFile   );
    bool loadLightDirs (const char *lightDirsFile);
    bool loadNormals   (const char *normalsFile  );
    bool loadAlbedos   (const char *albedosFile  );
    bool loadDepths    (const char *depthsFile   );

    // Save methods: save the corresponding data from files
    //   return true if successful

    bool saveLightDirs (const char *lightDirsFile) const;
    bool saveNormals   (const char *normalsFile  ) const;
    bool saveAlbedos   (const char *albedosFile  ) const;
    bool saveDepths    (const char *depthsFile   ) const;

    // Compute methods: compute the data from existing data
    //   return true if successful

    bool computeLightDirs      ();
    bool computeNormals        ();
    bool computeAlbedos        ();
    bool computeDepths         ();
    bool computeShadingNormals ();

    // Existence checkers: verify if data is present

    bool hasSources   () const { return (sources.size() > 0);          }
    bool hasMask      () const { return (mask    != 0);                }
    bool hasLightDirs () const { return (lightDirs.size() > 0);        }
    bool hasNormals   () const { return (normals != 0);                }
    bool hasAlbedos   () const { return (albedos != 0);                }
    bool hasDepths    () const { return (depths  != 0);                }

    // Const accessors for internal data

    const ColorImage  *getSource (unsigned int i) const
	{ if ((i>=0) && (i<sources.size())) return sources[i]; else return 0; }
    const ColorImage  *getMask           () const { return mask; }
    const NormalImage *getNormals        () const { return normals;   }
    const ColorImage  *getAlbedos        () const { return albedos;   }
    const DepthImage  *getDepths         () const { return depths;    }
    const NormalImage *getShadingNormals () const { return shadingNormals; }

    // Capability checkers: verify if next computation is possible

    bool canComputeLightDirs () const { return hasSources() & hasMask();      }
    bool canComputeNormals   () const { return hasSources() && hasLightDirs()
					    && (sources.size() ==
						lightDirs.size());            }
    bool canComputeAlbedos   () const { return hasSources() && hasNormals();  }
    bool canComputeDepths    () const { return hasNormals();                  }

    // Update the user interface based on current state

    void updateUI () const;

    // Quit program: check to see if anything needs to be saved

    void quit(int);

private:
    std::vector<ColorImage *> sources;
    ColorImage *mask;
    std::vector<Vec3f> lightDirs;
    NormalImage *normals;
    NormalImage *shadingNormals;  // recomputed normals, used for shading
    ColorImage *albedos;
    DepthImage *depths;
};

#endif // _PSM_APP_H_
