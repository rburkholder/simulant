#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Cue.o \
	${OBJECTDIR}/CueList.o \
	${OBJECTDIR}/DecodeH264.o \
	${OBJECTDIR}/DndDropTarget.o \
	${OBJECTDIR}/DndSourceButton.o \
	${OBJECTDIR}/DndTargetButton.o \
	${OBJECTDIR}/appProjection.o \
	${OBJECTDIR}/canvasOpenGL.o \
	${OBJECTDIR}/eventImage.o \
	${OBJECTDIR}/frameMain.o \
	${OBJECTDIR}/frameProjection.o \
	${OBJECTDIR}/panelSurfaceSources.o \
	${OBJECTDIR}/tex1.o \
	${OBJECTDIR}/tex2.o \
	${OBJECTDIR}/tut1.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 `/usr/local/bin/wx-config --cxxflags` 
CXXFLAGS=-m64 `/usr/local/bin/wx-config --cxxflags` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lavdevice -lavcodec -lavresample -lavfilter -lavformat -lavutil -lswscale -lz -lpthread -lpng -lboost_system -lboost_thread -lboost_chrono -lGL -lwx_gtk3u_gl-3.1 -lGLU -lSOIL

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/Cue.o: Cue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Cue.o Cue.cpp

${OBJECTDIR}/CueList.o: CueList.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CueList.o CueList.cpp

${OBJECTDIR}/DecodeH264.o: DecodeH264.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DecodeH264.o DecodeH264.cpp

${OBJECTDIR}/DndDropTarget.o: DndDropTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndDropTarget.o DndDropTarget.cpp

${OBJECTDIR}/DndSourceButton.o: DndSourceButton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndSourceButton.o DndSourceButton.cpp

${OBJECTDIR}/DndTargetButton.o: DndTargetButton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndTargetButton.o DndTargetButton.cpp

${OBJECTDIR}/appProjection.o: appProjection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/appProjection.o appProjection.cpp

${OBJECTDIR}/canvasOpenGL.o: canvasOpenGL.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/canvasOpenGL.o canvasOpenGL.cpp

${OBJECTDIR}/eventImage.o: eventImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/eventImage.o eventImage.cpp

${OBJECTDIR}/frameMain.o: frameMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/frameMain.o frameMain.cpp

${OBJECTDIR}/frameProjection.o: frameProjection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/frameProjection.o frameProjection.cpp

${OBJECTDIR}/panelSurfaceSources.o: panelSurfaceSources.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/panelSurfaceSources.o panelSurfaceSources.cpp

${OBJECTDIR}/tex1.o: tex1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tex1.o tex1.cpp

${OBJECTDIR}/tex2.o: tex2.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tex2.o tex2.cpp

${OBJECTDIR}/tut1.o: tut1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tut1.o tut1.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
