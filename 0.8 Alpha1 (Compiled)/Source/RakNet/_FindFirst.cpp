/**
* Original file by the_viking, fixed by R√¥mulo Fernandes, fixed by Emmanuel Nars
* Should emulate windows finddata structure
*/
#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32)
#include "_FindFirst.h"
#include "DS_List.h"

#include <sys/stat.h>
#ifndef _PS3
#include <fnmatch.h>
#endif

static DataStructures::List< _findinfo_t* > fileInfo;
	
#include "RakMemoryOverride.h"
#include "RakAssert.h"

/**
* _findfirst - equivalent
*/
long _findfirst(const char *name, _finddata_t *f)
{
	RakNet::RakString nameCopy = name;
        RakNet::RakString filter;

        // This is linux only, so don't bother with '\'
	const char* lastSep = strrchr(name,'/');
	if(!lastSep)
	{
            // filter pattern only is given, search current directory.
            filter = nameCopy;
            nameCopy = ".";
	} else
	{
            // strip filter pattern from directory name, leave
            // trailing '/' intact.
            filter = lastSep+1;
            unsigned sepIndex = lastSep - name;
            nameCopy.Erase(sepIndex+1, nameCopy.GetLength() - sepIndex-1);
	}

	DIR* dir = opendir(nameCopy);
        
	if(!dir) return -1;

	_findinfo_t* fi = new _findinfo_t;
	fi->filter    = filter;
	fi->dirName   = nameCopy;  // we need to remember this for stat()
	fi->openedDir = dir;
	fileInfo.Insert(fi);

        long ret = fileInfo.Size()-1;

        // Retrieve the first file. We cannot rely on the first item
        // being '.'
        if (_findnext(ret, f) == -1) return -1;
        else return ret;
}

#ifdef _PS3
int _findnext(long h, _finddata_t *f)
{
	_findinfo_t* fi = fileInfo[h];

	while(true)
	{
		dirent* entry = readdir(fi->openedDir);
		if(entry == 0)
			return -1;

		// Commented code not supported on a console
		//	if(fnmatch(fi->filter,entry->d_name, 200) == 0)
		if(strcasecmp(fi->filter,entry->d_name) == 0)
		{
			strcpy(f->name, entry->d_name);
			if (entry->d_type == DT_REG)
				f->attrib = _A_NORMAL;
			f->size = entry->d_reclen;
			return 0;
		}
		if (entry->d_type == DT_DIR)
		{
			f->attrib = _A_SUBDIR;
			strcpy(f->name, entry->d_name);
			return 0;
		}
	}

	return -1;
}
#else
int _findnext(long h, _finddata_t *f)
{
	RakAssert(h >= 0 && h < (long)fileInfo.Size());
	if (h < 0 || h >= (long)fileInfo.Size()) return -1;
        
	_findinfo_t* fi = fileInfo[h];

	while(true)
	{
		dirent* entry = readdir(fi->openedDir);
		if(entry == 0) return -1;

                // Only report stuff matching our filter
                if (fnmatch(fi->filter, entry->d_name, FNM_PATHNAME) != 0) continue;

                // To reliably determine the entry's type, we must do
                // a stat...  don't rely on entry->d_type, as this
                // might be unavailable!
                struct stat filestat;
                RakNet::RakString fullPath = fi->dirName + entry->d_name;             
                if (stat(fullPath, &filestat) != 0)
                {
                    printf("Cannot stat %s\n", fullPath.C_String());
                    continue;
                }

                if (S_ISREG(filestat.st_mode))
                {
                    f->attrib = _A_NORMAL;
                } else if (S_ISDIR(filestat.st_mode))
                {
                    f->attrib = _A_SUBDIR;                    
                } else continue; // We are interested in files and
                                 // directories only. Links currently
                                 // are not supported.

                f->size = filestat.st_size;
                strncpy(f->name, entry->d_name, STRING_BUFFER_SIZE);
                
                return 0;
	}

	return -1;
}
#endif




/**
 * _findclose - equivalent
 */
int _findclose(long h)
{
    if (h==-1) return 0;
    
    if (h < 0 || h >= (long)fileInfo.Size())
    {
        RakAssert(false);
        return -1;
    }
    
    _findinfo_t* fi = fileInfo[h];
    fileInfo.RemoveAtIndex(h);
    delete fi;
    return 0;	
}
#endif
