
// Copyright (c) 1999-2005 by Digital Mars
// All Rights Reserved
// written by Walter Bright
// www.digitalmars.com
// License for redistribution is by either the Artistic License
// in artistic.txt, or the GNU General Public License in gnu.txt.
// See the included readme.txt for details.

#ifndef DMD_IDENTIFIER_H
#define DMD_IDENTIFIER_H

#ifdef __DMC__
#pragma once
#endif /* __DMC__ */

#include "root.h"

struct Identifier : Object
{
    int value;
    const char *string;
    unsigned len;

    Identifier(const char *string, int value);
    int equals(Object *o);
    unsigned hashCode();
    int compare(Object *o);
    void print();
    char *toChars();
#ifdef _DH
    char *toHChars();
#endif
    char *toHChars2();
    int dyncast();

    static Identifier *generateId(char *prefix);
};

#endif /* DMD_IDENTIFIER_H */
