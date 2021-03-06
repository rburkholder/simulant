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
	${OBJECTDIR}/Audio.o \
	${OBJECTDIR}/AudioQueue.o \
	${OBJECTDIR}/CanvasBase.o \
	${OBJECTDIR}/CanvasOpenGL.o \
	${OBJECTDIR}/ConsoleStream.o \
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
	${OBJECTDIR}/KeyFrameDiamond.o \
	${OBJECTDIR}/KeyFrameView.o \
	${OBJECTDIR}/MediaStreamDecode.o \
	${OBJECTDIR}/Outline.o \
	${OBJECTDIR}/PanelLogging.o \
	${OBJECTDIR}/PanelSurfaceSources.o \
	${OBJECTDIR}/PhysicalDisplay.o \
	${OBJECTDIR}/RawImage.o \
	${OBJECTDIR}/ReusableBuffers.o \
	${OBJECTDIR}/SEGrid.o \
	${OBJECTDIR}/SETexture.o \
	${OBJECTDIR}/Scene.o \
	${OBJECTDIR}/SceneElement.o \
	${OBJECTDIR}/SceneElementOpenGL.o \
	${OBJECTDIR}/SceneManager.o \
	${OBJECTDIR}/SceneMgmtView.o \
	${OBJECTDIR}/SceneViewCommon.o \
	${OBJECTDIR}/ShaderManager.o \
	${OBJECTDIR}/TimeLine.o \
	${OBJECTDIR}/TimeLineView.o \
	${OBJECTDIR}/TreeDisplayManager.o \
	${OBJECTDIR}/WaveformView.o \
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
LDLIBSOPTIONS=-lavdevice -lavfilter -lavformat -lavcodec -lavresample -lavutil -lswscale -lz -lpthread -lpng -lboost_system -lboost_thread -lboost_chrono -lGL -lwx_gtk3u_gl-3.1 -lGLU -lSOIL -lboost_filesystem -lbz2 -lrtaudio -lboost_serialization

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

${OBJECTDIR}/Audio.o: Audio.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Audio.o Audio.cpp

${OBJECTDIR}/AudioQueue.o: AudioQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AudioQueue.o AudioQueue.cpp

${OBJECTDIR}/CanvasBase.o: CanvasBase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CanvasBase.o CanvasBase.cpp

${OBJECTDIR}/CanvasOpenGL.o: CanvasOpenGL.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CanvasOpenGL.o CanvasOpenGL.cpp

${OBJECTDIR}/ConsoleStream.o: ConsoleStream.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConsoleStream.o ConsoleStream.cpp

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

${OBJECTDIR}/KeyFrameDiamond.o: KeyFrameDiamond.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/KeyFrameDiamond.o KeyFrameDiamond.cpp

${OBJECTDIR}/KeyFrameView.o: KeyFrameView.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/KeyFrameView.o KeyFrameView.cpp

${OBJECTDIR}/MediaStreamDecode.o: MediaStreamDecode.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MediaStreamDecode.o MediaStreamDecode.cpp

${OBJECTDIR}/Outline.o: Outline.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Outline.o Outline.cpp

${OBJECTDIR}/PanelLogging.o: PanelLogging.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PanelLogging.o PanelLogging.cpp

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

${OBJECTDIR}/ReusableBuffers.o: ReusableBuffers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReusableBuffers.o ReusableBuffers.cpp

${OBJECTDIR}/SEGrid.o: SEGrid.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SEGrid.o SEGrid.cpp

${OBJECTDIR}/SETexture.o: SETexture.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SETexture.o SETexture.cpp

${OBJECTDIR}/Scene.o: Scene.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Scene.o Scene.cpp

${OBJECTDIR}/SceneElement.o: SceneElement.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneElement.o SceneElement.cpp

${OBJECTDIR}/SceneElementOpenGL.o: SceneElementOpenGL.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneElementOpenGL.o SceneElementOpenGL.cpp

${OBJECTDIR}/SceneManager.o: SceneManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneManager.o SceneManager.cpp

${OBJECTDIR}/SceneMgmtView.o: SceneMgmtView.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneMgmtView.o SceneMgmtView.cpp

${OBJECTDIR}/SceneViewCommon.o: SceneViewCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneViewCommon.o SceneViewCommon.cpp

${OBJECTDIR}/ShaderManager.o: ShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ShaderManager.o ShaderManager.cpp

${OBJECTDIR}/TimeLine.o: TimeLine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TimeLine.o TimeLine.cpp

${OBJECTDIR}/TimeLineView.o: TimeLineView.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TimeLineView.o TimeLineView.cpp

${OBJECTDIR}/TreeDisplayManager.o: TreeDisplayManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeDisplayManager.o TreeDisplayManager.cpp

${OBJECTDIR}/WaveformView.o: WaveformView.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -D_DEBUG -DwxUSE_GUI -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WaveformView.o WaveformView.cpp

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
