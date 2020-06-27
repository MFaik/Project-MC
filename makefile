sfmlincludedir = $(CURDIR)/Dependencies/SFML/include/
sfmllibdir = ${CURDIR}/Dependencies/SFML/lib
sfmllibd = -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-audio-s-d -lsfml-network-s-d -lsfml-system-s-d  -lsfml-main-d
sfmllib = -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-network-s -lsfml-system-s -lsfml-main
externallib = -lopengl32 -lwinmm -lgdi32 -lfreetype -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lws2_32 


all:
	g++ UI/*.cpp *.cpp -O3 -s -oGame -DSFML_STATIC -I${sfmlincludedir}  -L${sfmllibdir} $(sfmllib) ${externallib}

debug:
	g++ UI/*.cpp *.cpp -O1 -g -v -Wall -oGame -DSFML_STATIC -I ${sfmlincludedir} -L ${sfmllibdir} $(sfmllibd) ${externallib}

clean:
	del /s *.exe