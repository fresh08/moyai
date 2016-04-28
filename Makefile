MOYAICLISRCS=common.cpp cumino.cpp  lodepng.cpp Prop2D.cpp Prop3D.cpp ColorReplacerShader.cpp Font.cpp FragmentShader.cpp IndexBuffer.cpp Layer.cpp MoyaiClient.cpp TextBox.cpp Prim.cpp Texture.cpp VertexBuffer.cpp Viewport.cpp DrawBatch.cpp Camera.cpp CharGrid.cpp Grid.cpp Mesh.cpp Pad.cpp PerformanceCounter.cpp PrimDrawer.cpp Sound.cpp SoundSystem.cpp VertexFormat.cpp TileDeck.cpp Remote.cpp Keyboard.cpp JPEGCoder.cpp WavePlayer.cpp
UTF8SRC=ConvertUTF.c
UTF8OBJ=ConvertUTF.o
MOYAICLIOBJS=$(MOYAICLISRCS:.cpp=.o) $(UTF8OBJ)
MOYAISVSRCS=common.cpp cumino.cpp  lodepng.cpp Camera.cpp Grid.cpp 
MOYAISVOBJS=$(MOYAISVSRCS:.cpp=.o) $(UTF8OBJ)
DEMO2DSRCS=demo2d.cpp 
DEMO2DOBJS=$(DEMO2DSRCS:.cpp=.o)
DEMO3DSRCS=demo3d.cpp
DEMO3DOBJS=$(DEMO3DSRCS:.cpp=.o)
MIN2DSRCS=min2d.cpp
MIN2DOBJS=$(MIN2DSRCS:.cpp=.o)
VIEWERSRCS=vw.cpp
VIEWEROBJS=$(VIEWERSRCS:.cpp=.o)
DYNCAM2DSRCS=dyncam2d.cpp
DYNCAM2DOBJS=$(DYNCAM2DSRCS:.cpp=.o)
REPLAYERSRCS=rep.cpp
REPLAYEROBJS=$(REPLAYERSRCS:.cpp=.o)

FREETYPE=freetype-2.4.10
FREETYPELIB=$(FREETYPE)/objs/.libs/libfreetype.a  # build product of freetype source

BZ2=bzip2-1.0.6
BZ2LIB=$(BZ2)/libbz2.a # build product of bz2 source

ZLIB=zlib-1.2.7
ZLIBLIB=$(ZLIB)/libz.a

GLFW=glfw-3.1.2
GLFWLIB=$(GLFW)/src/libglfw3.a

FTGLOBJS=vertex-attribute.o vertex-buffer.o vector.o texture-atlas.o texture-font.o
SNAPPYOBJS=snappy/snappy-sinksource.o snappy/snappy-c.o snappy/snappy.o

FTGLLIB=libftgl.a
OUTCLILIB=libmoyaicl.a
OUTSVLIB=libmoyaisv.a
SNAPPYLIB=libsnappy.a
UNTZLIB=untz/libuntz.a

EXTCOMMONLIBS= $(ZLIBLIB) $(BZ2LIB) $(LIBPNGLIB) $(SNAPPYLIB)
EXTCLILIBS = $(EXTCOMMONLIBS) $(FREETYPELIB) $(FTGLLIB) $(GLFWLIB) $(UNTZLIB)
CLILIBFLAGS=-framework Cocoa -framework IOKit -framework OpenGL -framework CoreFoundation -framework CoreVideo -m64  fmod/api/lib/libfmodex.dylib -L/usr/local/lib -luv -ljpeg
CFLAGS=-O0 -I/usr/local/include -I$(FREETYPE)/include -g  -I./freetype-gl -Wall -m64  -I./$(GLFW)/include -DUSE_FMOD
CFLAGS0X=-std=c++0x $(CFLAGS)


DEMO2D=demo2d
DEMO3D=demo3d
MIN2D=min2d
VIEWER=viewer
DYNCAM2D=dyncam2d
REPLAYER=replayer

all : $(DEMO2D) $(DEMO3D) $(MIN2D) $(VIEWER) $(DYNCAM2D) $(REPLAYER)

server : $(OUTSVLIB) $(SNAPPYLIB)

$(REPLAYER) : $(EXTCLILIBS) $(OUTCLILIB) $(REPLAYEROBJS) $(BZ2LIB) $(ZLIBLIB)
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(REPLAYEROBJS) -o $(REPLAYER) $(OUTCLILIB) $(EXTCLILIBS)

$(DYNCAM2D) : $(EXTCLILIBS) $(OUTCLILIB) $(DYNCAM2DOBJS) $(BZ2LIB) $(ZLIBLIB)
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(DYNCAM2DOBJS) -o $(DYNCAM2D) $(OUTCLILIB) $(EXTCLILIBS)

$(MIN2D) : $(EXTCLILIBS) $(OUTCLILIB) $(MIN2DOBJS) $(BZ2LIB) $(ZLIBLIB)
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(MIN2DOBJS) -o $(MIN2D) $(OUTCLILIB) $(EXTCLILIBS)

$(DEMO2D) : $(EXTCLILIBS) $(OUTCLILIB) $(DEMO2DOBJS) $(BZ2LIB) $(ZLIBLIB) 
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(DEMO2DOBJS) -o $(DEMO2D) $(OUTCLILIB) $(EXTCLILIBS)

$(DEMO3D) : $(EXTCLILIBS) $(OUTCLILIB) $(DEMO3DOBJS) $(BZ2LIB) $(ZLIBLIB) 
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(DEMO3DOBJS) -o $(DEMO3D) $(OUTCLILIB) $(EXTCLILIBS)

$(VIEWER) : $(EXTCLILIBS) $(OUTCLILIB) $(VIEWEROBJS) $(BZ2LIB) $(ZLIBLIB)
	g++ $(CFLAGS0X) $(CLILIBFLAGS) $(VIEWEROBJS) -o $(VIEWER) $(OUTCLILIB) $(EXTCLILIBS)

demo2d.o : demo2d.cpp 
	g++ -c demo2d.cpp $(CFLAGS0X)
demo3d.o : demo3d.cpp 
	g++ -c demo3d.cpp $(CFLAGS0X)
min2d.o : min2d.cpp
	g++ -c min2d.cpp $(CFLAGS0X)
vw.o : vw.cpp
	g++ -c vw.cpp $(CFLAGS0X)
dyncam2d.o : dyncam2d.cpp
	g++ -c dyncam2d.cpp $(CFLAGS0X)
rep.o : rep.cpp
	g++ -c rep.cpp $(CFLAGS0X)

$(OUTCLILIB) : $(MOYAICLIOBJS)
	ar cr $(OUTCLILIB) $(MOYAICLIOBJS)
	ranlib $(OUTCLILIB)

$(OUTSVLIB) : $(MOYAISVOBJS)
	ar cr $(OUTSVLIB) $(MOYAISVOBJS)
	ranlib $(OUTSVLIB)

$(FTGLLIB) : $(FTGLOBJS)
	ar cr $(FTGLLIB) $(FTGLOBJS)
	ranlib $(FTGLLIB)

$(SNAPPYLIB) : $(SNAPPYOBJS)
	ar cr $(SNAPPYLIB) $(SNAPPYOBJS)
	ranlib $(SNAPPYLIB)



common.o : common.cpp
	g++ -c common.cpp $(CFLAGS0X)

cumino.o : cumino.cpp
	g++ -c cumino.cpp $(CFLAGS0X)
Prop2D.o : Prop2D.cpp
	g++ -c $(CFLAGS0X) Prop2D.cpp  -o Prop2D.o
Prop3D.o : Prop3D.cpp
	g++ -c $(CFLAGS0X) Prop3D.cpp  -o Prop3D.o
ColorReplacerShader.o : ColorReplacerShader.cpp
	g++ -c $(CFLAGS0X) ColorReplacerShader.cpp -o ColorReplacerShader.o
Font.o : Font.cpp
	g++ -c $(CFLAGS0X) Font.cpp -o Font.o
FragmentShader.o : FragmentShader.cpp
	g++ -c $(CFLAGS0X) FragmentShader.cpp -o FragmentShader.o
IndexBuffer.o : IndexBuffer.cpp
	g++ -c $(CFLAGS0X) IndexBuffer.cpp -o IndexBuffer.o
Layer.o : Layer.cpp
	g++ -c $(CFLAGS0X) Layer.cpp -o Layer.o
MoyaiClient.o : MoyaiClient.cpp
	g++ -c $(CFLAGS0X) MoyaiClient.cpp -o MoyaiClient.o
TextBox.o : TextBox.cpp
	g++ -c $(CFLAGS0X) TextBox.cpp -o TextBox.o
Texture.o : Texture.cpp
	g++ -c $(CFLAGS0X) Texture.cpp -o Texture.o
VertexBuffer.o : VertexBuffer.cpp
	g++ -c $(CFLAGS0X) VertexBuffer.cpp -o VertexBuffer.o
Viewport.o : Viewport.cpp
	g++ -c $(CFLAGS0X) Viewport.cpp -o Viewport.o
Prim.o : Prim.cpp
	g++ -c $(CFLAGS0X) Prim.cpp -o Prim.o
DrawBatch.o : DrawBatch.cpp
	g++ -c $(CFLAGS0X) DrawBatch.cpp -o DrawBatch.o
Camera.o : Camera.cpp
	g++ -c $(CFLAGS0X) Camera.cpp -o Camera.o
CharGrid.o : CharGrid.cpp
	g++ -c $(CFLAGS0X) CharGrid.cpp -o CharGrid.o
Grid.o : Grid.cpp
	g++ -c $(CFLAGS0X) Grid.cpp -o Grid.o
Mesh.o : Mesh.cpp
	g++ -c $(CFLAGS0X) Mesh.cpp -o Mesh.o
Pad.o : Pad.cpp
	g++ -c $(CFLAGS0X) Pad.cpp -o Pad.o
PerformanceCounter.o : PerformanceCounter.cpp
	g++ -c $(CFLAGS0X) PerformanceCounter.cpp -o PerformanceCounter.o
PrimDrawer.o : PrimDrawer.cpp
	g++ -c $(CFLAGS0X) PrimDrawer.cpp -o PrimDrawer.o
Sound.o : Sound.cpp
	g++ -c $(CFLAGS0X) Sound.cpp -o Sound.o
SoundSystem.o : SoundSystem.cpp
	g++ -c $(CFLAGS0X) SoundSystem.cpp -o SoundSystem.o
VertexFormat.o : VertexFormat.cpp
	g++ -c $(CFLAGS0X) VertexFormat.cpp -o VertexFormat.o
TileDeck.o : TileDeck.cpp
	g++ -c $(CFLAGS0X) TileDeck.cpp -o TileDeck.o
Remote.o : Remote.cpp
	g++ -c $(CFLAGS0X) Remote.cpp -o Remote.o
ConvertUTF.o : ConvertUTF.c
	g++ -c $(CFLAGS0X) ConvertUTF.c -o ConvertUTF.o
Keyboard.o : Keyboard.cpp
	g++ -c $(CFLAGS0X) Keyboard.cpp -o Keyboard.o
JPEGCoder.o : JPEGCoder.cpp
	g++ -c $(CFLAGS0X) JPEGCoder.cpp -o JPEGCoder.o
WavePlayer.o : WavePlayer.cpp
	g++ -c $(CFLAGS0X) WavePlayer.cpp -o WavePlayer.o

# freetype-gl
texture-atlas.o :
	g++ -c freetype-gl/texture-atlas.c $(CFLAGS)
texture-font.o :
	g++ -c freetype-gl/texture-font.c $(CFLAGS)
vector.o :
	g++ -c freetype-gl/vector.c $(CFLAGS)
vertex-buffer.o :
	g++ -c freetype-gl/vertex-buffer.c $(CFLAGS)
vertex-attribute.o :
	g++ -c freetype-gl/vertex-attribute.c $(CFLAGS)


snappy-sinksource.o:
	g++ -c snappy/snappy-sinksource.cc $(CFLAGS)
snappy-c.o:
	g++ -c snappy/snappy-c.cc $(CFLAGS)
snappy.o:
	g++ -c snappy/snappy.cc $(CFLAGS)

$(FREETYPELIB):
	rm -rf $(FREETYPE)
	tar jxf $(FREETYPE).tar.bz2
	cd $(FREETYPE); ./configure; make

$(BZ2LIB):
	rm -rf $(BZ2)
	tar zxf $(BZ2).tar.gz
	cd $(BZ2); make

$(ZLIBLIB):
	rm -rf $(ZLIB)
	tar zxf $(ZLIB).tar.gz
	cd $(ZLIB); ./configure; make

$(LIBPNGLIB):
	rm -rf $(LIBPNG)
	tar zxf $(LIBPNG).tar.gz
	cd $(LIBPNG); ./configure; make


$(GLFWLIB):
	cd $(GLFW); cmake .; make

$(UNTZLIB):
	make -C untz

clean:
	make -C $(GLFW) clean
	make -C untz clean
	rm -rf $(FREETYPE) $(BZ2) $(ZLIB) $(LIBPNG) 
	rm -f deps.make $(VIEWER) $(DEMO2D) $(MIN2D) $(DEMO3D) $(DYNCAM2D) $(REPLAYER) $(OUTCLILIB) $(OUTSVLIB) *.o *.a */*.o

depend: $(GLFWLIB) $(UNTZLIB)
	$(CC) $(CFLAGS) -MM $(TESTSRCS) $(MOYAICLISRCS) $(DEMO2DSRCS) $(DEMO3DSRCS) $(MIN2DSRCS) $(DYNCAM2DSRCS) $(REPLAYERSRCS) $(VIEWERSRCS) > deps.make


-include deps.make
