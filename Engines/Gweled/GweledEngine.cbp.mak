#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall -fPIC
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -fPIC
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = ./GweledEngine.so

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2 -fPIC
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = ./GweledEngine.so

OBJ_DEBUG = $(OBJDIR_DEBUG)/__/__/ImageOperations/findSubImage.o $(OBJDIR_DEBUG)/vision.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/logic.o $(OBJDIR_DEBUG)/__/__/ImageOperations/patchComparison.o $(OBJDIR_DEBUG)/__/__/ImageOperations/imageOps.o $(OBJDIR_DEBUG)/__/__/Codecs/codecs.o $(OBJDIR_DEBUG)/__/__/Codecs/ppmInput.o $(OBJDIR_DEBUG)/__/__/Codecs/pngInput.o $(OBJDIR_DEBUG)/__/__/Codecs/jpgInput.o $(OBJDIR_DEBUG)/__/__/Codecs/jpgExiforient_embed.o $(OBJDIR_DEBUG)/__/__/Codecs/jpgExifexternal.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/ImageOperations/findSubImage.o $(OBJDIR_RELEASE)/vision.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/logic.o $(OBJDIR_RELEASE)/__/__/ImageOperations/patchComparison.o $(OBJDIR_RELEASE)/__/__/ImageOperations/imageOps.o $(OBJDIR_RELEASE)/__/__/Codecs/codecs.o $(OBJDIR_RELEASE)/__/__/Codecs/ppmInput.o $(OBJDIR_RELEASE)/__/__/Codecs/pngInput.o $(OBJDIR_RELEASE)/__/__/Codecs/jpgInput.o $(OBJDIR_RELEASE)/__/__/Codecs/jpgExiforient_embed.o $(OBJDIR_RELEASE)/__/__/Codecs/jpgExifexternal.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_DEBUG)/__/__/ImageOperations || mkdir -p $(OBJDIR_DEBUG)/__/__/ImageOperations
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/__/__/Codecs || mkdir -p $(OBJDIR_DEBUG)/__/__/Codecs

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) -shared $(LIBDIR_DEBUG) $(OBJ_DEBUG)  -o $(OUT_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/__/__/ImageOperations/findSubImage.o: ../../ImageOperations/findSubImage.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../ImageOperations/findSubImage.c -o $(OBJDIR_DEBUG)/__/__/ImageOperations/findSubImage.o

$(OBJDIR_DEBUG)/vision.o: vision.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c vision.c -o $(OBJDIR_DEBUG)/vision.o

$(OBJDIR_DEBUG)/main.o: main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.c -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/logic.o: logic.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c logic.c -o $(OBJDIR_DEBUG)/logic.o

$(OBJDIR_DEBUG)/__/__/ImageOperations/patchComparison.o: ../../ImageOperations/patchComparison.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../ImageOperations/patchComparison.c -o $(OBJDIR_DEBUG)/__/__/ImageOperations/patchComparison.o

$(OBJDIR_DEBUG)/__/__/ImageOperations/imageOps.o: ../../ImageOperations/imageOps.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../ImageOperations/imageOps.c -o $(OBJDIR_DEBUG)/__/__/ImageOperations/imageOps.o

$(OBJDIR_DEBUG)/__/__/Codecs/codecs.o: ../../Codecs/codecs.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/codecs.c -o $(OBJDIR_DEBUG)/__/__/Codecs/codecs.o

$(OBJDIR_DEBUG)/__/__/Codecs/ppmInput.o: ../../Codecs/ppmInput.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/ppmInput.c -o $(OBJDIR_DEBUG)/__/__/Codecs/ppmInput.o

$(OBJDIR_DEBUG)/__/__/Codecs/pngInput.o: ../../Codecs/pngInput.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/pngInput.c -o $(OBJDIR_DEBUG)/__/__/Codecs/pngInput.o

$(OBJDIR_DEBUG)/__/__/Codecs/jpgInput.o: ../../Codecs/jpgInput.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/jpgInput.c -o $(OBJDIR_DEBUG)/__/__/Codecs/jpgInput.o

$(OBJDIR_DEBUG)/__/__/Codecs/jpgExiforient_embed.o: ../../Codecs/jpgExiforient_embed.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/jpgExiforient_embed.c -o $(OBJDIR_DEBUG)/__/__/Codecs/jpgExiforient_embed.o

$(OBJDIR_DEBUG)/__/__/Codecs/jpgExifexternal.o: ../../Codecs/jpgExifexternal.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../Codecs/jpgExifexternal.c -o $(OBJDIR_DEBUG)/__/__/Codecs/jpgExifexternal.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf .
	rm -rf $(OBJDIR_DEBUG)/__/__/ImageOperations
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/__/__/Codecs

before_release: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_RELEASE)/__/__/ImageOperations || mkdir -p $(OBJDIR_RELEASE)/__/__/ImageOperations
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/__/__/Codecs || mkdir -p $(OBJDIR_RELEASE)/__/__/Codecs

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/ImageOperations/findSubImage.o: ../../ImageOperations/findSubImage.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../ImageOperations/findSubImage.c -o $(OBJDIR_RELEASE)/__/__/ImageOperations/findSubImage.o

$(OBJDIR_RELEASE)/vision.o: vision.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c vision.c -o $(OBJDIR_RELEASE)/vision.o

$(OBJDIR_RELEASE)/main.o: main.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.c -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/logic.o: logic.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c logic.c -o $(OBJDIR_RELEASE)/logic.o

$(OBJDIR_RELEASE)/__/__/ImageOperations/patchComparison.o: ../../ImageOperations/patchComparison.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../ImageOperations/patchComparison.c -o $(OBJDIR_RELEASE)/__/__/ImageOperations/patchComparison.o

$(OBJDIR_RELEASE)/__/__/ImageOperations/imageOps.o: ../../ImageOperations/imageOps.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../ImageOperations/imageOps.c -o $(OBJDIR_RELEASE)/__/__/ImageOperations/imageOps.o

$(OBJDIR_RELEASE)/__/__/Codecs/codecs.o: ../../Codecs/codecs.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/codecs.c -o $(OBJDIR_RELEASE)/__/__/Codecs/codecs.o

$(OBJDIR_RELEASE)/__/__/Codecs/ppmInput.o: ../../Codecs/ppmInput.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/ppmInput.c -o $(OBJDIR_RELEASE)/__/__/Codecs/ppmInput.o

$(OBJDIR_RELEASE)/__/__/Codecs/pngInput.o: ../../Codecs/pngInput.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/pngInput.c -o $(OBJDIR_RELEASE)/__/__/Codecs/pngInput.o

$(OBJDIR_RELEASE)/__/__/Codecs/jpgInput.o: ../../Codecs/jpgInput.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/jpgInput.c -o $(OBJDIR_RELEASE)/__/__/Codecs/jpgInput.o

$(OBJDIR_RELEASE)/__/__/Codecs/jpgExiforient_embed.o: ../../Codecs/jpgExiforient_embed.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/jpgExiforient_embed.c -o $(OBJDIR_RELEASE)/__/__/Codecs/jpgExiforient_embed.o

$(OBJDIR_RELEASE)/__/__/Codecs/jpgExifexternal.o: ../../Codecs/jpgExifexternal.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../Codecs/jpgExifexternal.c -o $(OBJDIR_RELEASE)/__/__/Codecs/jpgExifexternal.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf .
	rm -rf $(OBJDIR_RELEASE)/__/__/ImageOperations
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__/__/Codecs

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

