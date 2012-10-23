//----------------------------------------------------------------------------- 
// File: enum.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _ENUM_H_
#define _ENUM_H_

enum BRUSHTYPE { NOTHINGBRUSH  = 0,
                 SHEETBRUSH    = 1,
                 CUBEBRUSH     = 2,
                 STAIRSBRUSH   = 3,
                 SPIRALBRUSH   = 4,
                 CYLINDERBRUSH = 5,
                 CIRCLEBRUSH   = 6,
                 CUSTOMBRUSH   = 7 };

enum FACETYPE  { NOTHINGFACE  = 0,
                 TRIANGLEFACE = 1,
                 QUADFACE     = 2 };

enum CLASSTYPE { NOTHINGCLASS   = 0,
                 POLYGONMANAGER = 1,
                 POLYGONCLASS   = 2,
                 FACECLASS      = 3,
                 VERTEXCLASS    = 4,
                 LIGHTCLASS     = 5 };

enum CAMERAVIEW { FORWARDVIEW = 0,
                  LEFTVIEW    = 1,
                  RIGHTVIEW   = 2,
                  UPVIEW      = 3,
                  DOWNVIEW    = 4,
                  NOTHINGVIEW = 5 };

enum RADIOSITYMODE { NOTHINGLIGHTING = 0,
                     BESTLIGHTING    = 1,
                     FASTLIGHTING    = 2 };
#endif