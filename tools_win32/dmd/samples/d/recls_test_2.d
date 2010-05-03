/* /////////////////////////////////////////////////////////////////////////////
 * File:        recls_test_2.d
 *
 * Purpose:     Test program for the D mapping of recls. (Now the std.recls 
 *              module in the D standard library). Test classes.
 *
 * Created      
 * Updated:     24th November 2003
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (C) 1999-2003, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 *              email:      software@synesis.com.au
 *
 *              This source code is placed into the public domain 2003
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software. 
 *
 *              This source code is provided by Synesis Software Pty Ltd "as is"
 *              and any warranties, whether expressed or implied, including, but
 *              not limited to, the implied warranties of merchantability and
 *              fitness for a particular purpose are disclaimed. In no event
 *              shall the Synesis Software Pty Ltd be liable for any direct,
 *              indirect, incidental, special, exemplary, or consequential
 *              damages (including, but not limited to, procurement of
 *              substitute goods or services; loss of use, data, or profits; or
 *              business interruption) however caused and on any theory of
 *              liability, whether in contract, strict liability, or tort
 *              (including negligence or otherwise) arising in any way out of
 *              the use of this software, even if advised of the possibility of
 *              such damage. 
 *
 *              Neither the name of Synesis Software Pty Ltd nor the names of
 *              any subdivisions, employees or agents of Synesis Software Pty
 *              Ltd, nor the names of any other contributors to this software
 *              may be used to endorse or promote products derived from this
 *              software without specific prior written permission. 
 *
 */


import std.recls;
import std.string;
import std.c.stdio;
import std.c.stdlib;

/** Program main */
int main(char[][] args)
{
    int                 iRet        =   0;
    int                 i;
    char[]              pattern     =   null;
    char[]              rootDir     =   null;
    hrecls_t            hSrch;
    recls_rc_t          rc;
    recls_uint32_t      flags       =   RECLS_FLAG.RECLS_F_RECURSIVE;

    for(i = 1; i < args.length; ++i)
    {
        char[]  arg = args[i];

        if(arg[0] == '-')
        {
            if(arg[1] == '-')
            {
                /* -- arguments */
            }
            else
            {
                /* - arguments */
                switch(arg[1])
                {
                    case    'R':    /* Do not recurse */
                        flags &= ~(RECLS_FLAG.RECLS_F_RECURSIVE);
                        break;
                    case    'p':    /* Show directory parts */
                        flags |= RECLS_FLAG.RECLS_F_DIRECTORY_PARTS;
                        break;
                    case    'f':    /* Find files */
                        flags |= RECLS_FLAG.RECLS_F_FILES;
                        break;
                    case    'd':    /* Find directories */
                        flags |= RECLS_FLAG.RECLS_F_DIRECTORIES;
                        break;
                    default:
                        usage(1);
                        break;
                }
            }
        }
        else
        {
            /* other arguments */
            if(null == pattern)
            {
                pattern = arg;
            }
            else if(null == rootDir)
            {
                rootDir = arg;
            }
            else
            {
                usage(1);
            }
        }
    }

    /* Search for files if neither files or directories specified/
     *
     * Even though this is not necessary, because the recls API provides the
     * same interpretation, it's best to be explicit.
     */
    if(0 == (flags & (RECLS_FLAG.RECLS_F_FILES | RECLS_FLAG.RECLS_F_DIRECTORIES)))
    {
        flags |= RECLS_FLAG.RECLS_F_FILES;
    }

    if(null == pattern)
    {
version(Windows)
{
        pattern = "*.*";
}
else
{
        pattern = "*";
}
    }

    if(null == rootDir)
    {
        rootDir = ".";
    }

    Search  search = new Search(rootDir, pattern, flags);

    foreach(inout Entry entry; search)
    {
/*      printf("Entry: 0x%08x\n", entry);
        if(entry.IsReadOnly == true)
        {
            printf("  read-only\n");
        }
        else
        {
            printf("  writeable\n");
        }
        printf("  Size: %lu\n", entry.Size());
 */
        int                 i;
        int                 off;
        int                 extLen;
        int                 cDirParts;
        char[]              pathCheck;
        char[]              dirCheck;
        char[]              fileCheck;

        // Path
        char[]              path        =   entry.Path;             printf("  %.*s\n", path);

        // Drive
version(Windows)
{
        char                drive       =   entry.Drive;

        // This complex dance is necessitated because string and ctype seem
        // unable to coexist (with v0.74, anyway)
        if(drive != path[0])
        {
            char    p0upper = toupper(path[0 .. 1])[0];
            char    p0lower = tolower(path[0 .. 1])[0];

            if(drive == p0upper)
            {
                drive = p0lower;
            }
            else
            {
                drive = p0upper;
            }
        }

                                                                    printf("  %c\n", drive);

        pathCheck ~= drive;
        pathCheck ~= ':';
}

        // Directory
        char[]              dir         =   entry.Directory;        printf("    %.*s\n", dir);

        // DirectoryPath
        char[]              dirPath     =   entry.DirectoryPath;    printf("  %.*s\n", dirPath);

        // DirectoryParts
        off = 0;
        if((flags & RECLS_FLAG.RECLS_F_DIRECTORY_PARTS) == RECLS_FLAG.RECLS_F_DIRECTORY_PARTS)
        {
            char[][]    parts   =   entry.DirectoryParts;

            foreach(char[] part; parts)
            {
                printf("    %*s", off, (char*)"");
                printf("%.*s\n", part);
                off += part.length;

                pathCheck ~= part;
                dirCheck ~= part;
            }
        }
        else
        {
            off += dir.length;
            pathCheck ~= dir;
        }

        // File, FileName, FileExt
        char[]              file        =   entry.File;
        char[]              fileName    =   entry.FileName;
        char[]              fileExt     =   entry.FileExt;

        printf("    %*s%.*s\n", off, (char*)"", file);
        printf("    %*s%.*s\n", off, (char*)"", fileName);
        off     +=  file.length;
        extLen  =   fileExt.length;
        printf("    %*s%.*s\n", off - extLen, (char*)"", fileExt);
        fileCheck = fileName;
        if(0 < extLen)
        {
            fileCheck ~= '.';
            fileCheck ~= fileExt;
        }
        pathCheck ~= file;

        // Size
        recls_filesize_t    size        =   entry.Size;

        printf("    %lu bytes\n", size);

        // IsReadOnly
        if(entry.IsReadOnly())
        {
            printf("    - Read-only\n");
        }
        if(entry.IsDirectory())
        {
            printf("    - Directory\n");
        }
        else
        {
            printf("    - File\n");
        }
        if(entry.IsLink())
        {
            printf("    - Link\n");
        }


        // Path corresponds to the path components
        if(path != pathCheck)
        {
            fprintf(stderr, "Path is different from path components\n\tpath:  %.*s\n\tpathCheck: %.*s\n\n", path, pathCheck);

            exit(1);
        }
        assert(path == pathCheck);

        // DirectoryPath and (Drive +) Directory
version(Windows)
{
        if( dirPath[0] != drive ||
            dir != dirPath[2 .. dirPath.length])
        {
            fprintf(stderr, "DirectoryPath is different from Directory\n\tdirPath:  %.*s (%d)\n\tdir:      %c:%.*s (%d)\n\n", dirPath, dirPath.length - 2, drive, dir, dir.length);

            exit(1);
        }
        assert(dirPath[0] == drive && dir == dirPath[2 .. dirPath.length]);
}
else
{
        if(dirPath != dir)
        {
            fprintf(stderr, "DirectoryPath is different from Directory\n\tdirPath:  %.*s\n\tdir: %.*s\n\n", dirPath, dir);

            exit(1);
        }
        assert(dirPath == dir);
}


        if((flags & RECLS_FLAG.RECLS_F_DIRECTORY_PARTS) == RECLS_FLAG.RECLS_F_DIRECTORY_PARTS)
        {
            if(dir != dirCheck)
            {
                fprintf(    stderr
                        ,   "Directory is different from directory components\n"
                            "  dir:      %.*s\n"
                            "  dirCheck: %.*s\n\n"
                        ,   dir
                        ,   dirCheck);

                exit(1);
            }
            assert(dir == dirCheck);
        }
        if(file != fileCheck)
        {
            fprintf(stderr, "File is different from file components\n\tfile:  %d:%.*s\n\tfileCheck: %d:%.*s\n\n", file.length, file, fileCheck.length, fileCheck);

            exit(1);
        }
        assert(file == fileCheck);


        printf("\n");
    }

    return iRet;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Function implementations
 */

/** Displays program usage */
void usage(int bExit)
{
    fprintf(stderr, "recls D Test Program: recls_test_2\n\n");
    fprintf(stderr, "Usage: recls_test_2 [-d] [-f] [-d] [-R] [<pattern>] [<root-dir>]\n");
    fprintf(stderr, "\t-d               -   search for directories\n");
    fprintf(stderr, "\t-f               -   search for files. (default if neither f or d specified)\n");
    fprintf(stderr, "\t-p               -   include directory parts in search, and display\n");
    fprintf(stderr, "\t-R               -   does not recurse. (recursive search is the default)\n");
    fprintf(stderr, "\t<pattern>        -   search pattern, e.g. \"*.cpp\" (default is to search for all files)\n");
    fprintf(stderr, "\t<root-dir>       -   root directory of search; default is current working directory\n");

    if(bExit)
    {
        exit(1);
    }
}

/* //////////////////////////////////////////////////////////////////////////
 * End of file
 */
