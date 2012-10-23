//----------------------------------------------------------------------------- 
// File: CustomMsg.h
//
// Desc: This file defines custom messages and control IDs that have to remain
//       constant throughout the program
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _CUSTOMMSG_H_
#define _CUSTOMMSG_H_

// Window Messages
#define CMSG_CHANGE_TAB           WM_APP + 1
#define CMSG_ALIGNVIEWS           WM_APP + 2
#define CMSG_ALIGNOTHERPANES      WM_APP + 3
#define CMSG_UPDATETEXTUREPREVIEW WM_APP + 4
#define CMSG_NEWSELECTION         WM_APP + 5
#define CMSG_UPDATEBRUSH          WM_APP + 6
#define CMSG_UPDATELIGHTDATA      WM_APP + 7

// Control IDs
#define ID_TEXTUREUUPDOWN         10001
#define ID_TEXTUREVUPDOWN         10002
#define ID_COLORRED               10003
#define ID_COLORGREEN             10004
#define ID_COLORBLUE              10005
#define ID_SCALE                  10006
#define ID_PANAMOUNT              10007


#endif