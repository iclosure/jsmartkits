#ifndef PRECOMP_H
#define PRECOMP_H

#ifdef QT_CORE_LIB
#include <QtCore>
#endif

#ifdef QT_GUI_LIB
#include <QtGui>
#endif

#ifdef QT_XML_LIB
#include <QtXml>
#endif

#if (QT_VERSION < 0x050000)
//
#else

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#endif

#ifdef QT_OPENGL_LIB
#include <QtOpenGL>
#endif

#endif

#if 0
#if defined(Q_WS_MAC) || defined(Q_OS_MACX)
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#endif

#if (QT_VERSION < 0x050000) && (defined(Q_OS_WIN) || defined(Q_WS_WIN))
#include <Windows.h>
#endif

// -
//#pragma execution_character_set("utf-8")

#endif // PRECOMP_H
