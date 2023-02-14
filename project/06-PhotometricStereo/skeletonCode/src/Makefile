CPPFILES = PsmMain.cpp PsmUI.cpp PsmApp.cpp LightDirs.cpp Normals.cpp \
	Albedos.cpp Depths.cpp ImageOps.cpp \
	ImageLib/Image.cpp ImageLib/FileIO.cpp ImageLib/RefCntMem.cpp \
	PsmView.cpp

ifeq ($(OSTYPE),darwin)
  LIBS = -framework vecLib
else
  LIBS = -llapack -lblas -lf2c
endif

LPATH = -L.
IPATH = -I./ImageLib
CPP = g++
LD = g++

OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
CPPFLAGS = -D__UNIX__ $(IPATH)
LDFLAGS = $(LPATH) $(LIBS)

CPPFLAGS += -g -DDEBUG

CPPFLAGS += `fltk-config --cxxflags`
LDFLAGS += `fltk-config --ldflags --use-gl`

psm: $(OFILES)
	$(LD) $^ $(LDFLAGS) -o $@
	fltk-config --post psm

.PHONY: clean
clean:
	-rm $(OFILES) psm