/*[]------------------------------------------------------------[]*/
/*|                                                              |*/
/*|     stbdtr.cpp                                               |*/
/*|                                                              |*/
/*|     Class streambuf                                          |*/
/*|          streambuf::~streambuf()                             |*/
/*|                                                              |*/
/*[]------------------------------------------------------------[]*/

/*
 *      C/C++ Run Time Library - Version 5.0
 *
 *      Copyright (c) 1990, 1992 by Borland International
 *      All Rights Reserved.
 *
 */

#include <ioconfig.h>
#include <iostream.h>

streambuf::~streambuf()
{
    if( alloc_ )
        delete base_;
}


