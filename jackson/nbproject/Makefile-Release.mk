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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AppProjection.o \
	${OBJECTDIR}/AspectRatio.o \
	${OBJECTDIR}/CanvasBase.o \
	${OBJECTDIR}/CanvasOpenGL.o \
	${OBJECTDIR}/Cue.o \
	${OBJECTDIR}/CueList.o \
	${OBJECTDIR}/DecodeH264.o \
	${OBJECTDIR}/DndDropTarget.o \
	${OBJECTDIR}/DndSourceButton.o \
	${OBJECTDIR}/DndTargetButton.o \
	${OBJECTDIR}/EventGenerateFrame.o \
	${OBJECTDIR}/EventImage.o \
	${OBJECTDIR}/FpsGenerator.o \
	${OBJECTDIR}/FrameMain.o \
	${OBJECTDIR}/FrameProjection.o \
	${OBJECTDIR}/InteractiveTransform.o \
	${OBJECTDIR}/MediaStreamDecode.o \
	${OBJECTDIR}/Outline.o \
	${OBJECTDIR}/PanelSurfaceSources.o \
	${OBJECTDIR}/PhysicalDisplay.o \
	${OBJECTDIR}/RawImage.o \
	${OBJECTDIR}/SEGrid.o \
	${OBJECTDIR}/SETexture.o \
	${OBJECTDIR}/SceneElement.o \
	${OBJECTDIR}/SceneManager.o \
	${OBJECTDIR}/ShaderManager.o \
	${OBJECTDIR}/TreeDisplayManager.o \
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
LDLIBSOPTIONS=-lavdevice -lavcodec -lavresample -lavfilter -lavformat -lavutil -lswscale -lz -lpthread -lpng -lboost_system -lboost_thread -lboost_chrono

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/jackson ${OBJECTFILES} ${LDLIBSOPTIONS} `/usr/local/bin/wx-config --libs`

${OBJECTDIR}/AppProjection.o: AppProjection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AppProjection.o AppProjection.cpp

${OBJECTDIR}/AspectRatio.o: AspectRatio.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AspectRatio.o AspectRatio.cpp

${OBJECTDIR}/CanvasBase.o: CanvasBase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CanvasBase.o CanvasBase.cpp

${OBJECTDIR}/CanvasOpenGL.o: CanvasOpenGL.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CanvasOpenGL.o CanvasOpenGL.cpp

${OBJECTDIR}/Cue.o: Cue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Cue.o Cue.cpp

${OBJECTDIR}/CueList.o: CueList.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CueList.o CueList.cpp

${OBJECTDIR}/DecodeH264.o: DecodeH264.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DecodeH264.o DecodeH264.cpp

${OBJECTDIR}/DndDropTarget.o: DndDropTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndDropTarget.o DndDropTarget.cpp

${OBJECTDIR}/DndSourceButton.o: DndSourceButton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndSourceButton.o DndSourceButton.cpp

${OBJECTDIR}/DndTargetButton.o: DndTargetButton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DndTargetButton.o DndTargetButton.cpp

${OBJECTDIR}/EventGenerateFrame.o: EventGenerateFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventGenerateFrame.o EventGenerateFrame.cpp

${OBJECTDIR}/EventImage.o: EventImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EventImage.o EventImage.cpp

${OBJECTDIR}/FpsGenerator.o: FpsGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FpsGenerator.o FpsGenerator.cpp

${OBJECTDIR}/FrameMain.o: FrameMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FrameMain.o FrameMain.cpp

${OBJECTDIR}/FrameProjection.o: FrameProjection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FrameProjection.o FrameProjection.cpp

${OBJECTDIR}/InteractiveTransform.o: InteractiveTransform.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InteractiveTransform.o InteractiveTransform.cpp

${OBJECTDIR}/MediaStreamDecode.o: MediaStreamDecode.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MediaStreamDecode.o MediaStreamDecode.cpp

${OBJECTDIR}/Outline.o: Outline.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Outline.o Outline.cpp

${OBJECTDIR}/PanelSurfaceSources.o: PanelSurfaceSources.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelSurfaceSources.o PanelSurfaceSources.cpp

${OBJECTDIR}/PhysicalDisplay.o: PhysicalDisplay.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PhysicalDisplay.o PhysicalDisplay.cpp

${OBJECTDIR}/RawImage.o: RawImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RawImage.o RawImage.cpp

${OBJECTDIR}/SEGrid.o: SEGrid.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SEGrid.o SEGrid.cpp

${OBJECTDIR}/SETexture.o: SETexture.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SETexture.o SETexture.cpp

${OBJECTDIR}/SceneElement.o: SceneElement.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneElement.o SceneElement.cpp

${OBJECTDIR}/SceneManager.o: SceneManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneManager.o SceneManager.cpp

${OBJECTDIR}/ShaderManager.o: ShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ShaderManager.o ShaderManager.cpp

${OBJECTDIR}/TreeDisplayManager.o: TreeDisplayManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeDisplayManager.o TreeDisplayManager.cpp

${OBJECTDIR}/tex1.o: tex1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tex1.o tex1.cpp

${OBJECTDIR}/tex2.o: tex2.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tex2.o tex2.cpp

${OBJECTDIR}/tut1.o: tut1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tut1.o tut1.cpp

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
