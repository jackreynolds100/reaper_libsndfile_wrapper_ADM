#include "libsndfileImport.h"

int		(*ptr_sf_command)	(SNDFILE *sndfile, int command, void *data, int datasize) ;
SNDFILE* 	(*ptr_sf_open)		(const char *path, int mode, SF_INFO *sfinfo) ;
int		(*ptr_sf_close)		(SNDFILE *sndfile) ;
sf_count_t	(*ptr_sf_read_float)	(SNDFILE *sndfile, float *ptr, sf_count_t items) ;
sf_count_t	(*ptr_sf_read_double)	(SNDFILE *sndfile, double *ptr, sf_count_t items) ;
sf_count_t	(*ptr_sf_seek) 		(SNDFILE *sndfile, sf_count_t frames, int whence) ;
sf_count_t	(*ptr_sf_readf_double)(SNDFILE *sndfile, double *ptr, sf_count_t frames) ;
const char * (*ptr_sf_version_string)(void);

HINSTANCE g_hLibSndFile=0;

int ImportLibSndFileFunctions(const char *dllpath)
{
    if (g_hLibSndFile)
        return 0;
    int errcnt=0;
    if (!dllpath)
        g_hLibSndFile=LoadLibraryA("libsndfile-1.dll");
    else g_hLibSndFile=LoadLibraryA(dllpath);
    if (!g_hLibSndFile)
        errcnt++;
    if (g_hLibSndFile)
    {
        //OutputDebugStringA("libsndfile dll loaded! now loading functions...");
        *((void **)&ptr_sf_command)=(void*)GetProcAddress(g_hLibSndFile,"sf_command");
        if (!ptr_sf_command) errcnt++;
        *((void **)&ptr_sf_open)=(void*)GetProcAddress(g_hLibSndFile,"sf_open");
        if (!ptr_sf_open) errcnt++;
        *((void **)&ptr_sf_close)=(void*)GetProcAddress(g_hLibSndFile,"sf_close");
        if (!ptr_sf_close) errcnt++;
        *((void **)&ptr_sf_read_float)=(void*)GetProcAddress(g_hLibSndFile,"sf_read_float");
        *((void **)&ptr_sf_read_double)=(void*)GetProcAddress(g_hLibSndFile,"sf_read_double");
        if (!ptr_sf_read_double) errcnt++;
        *((void **)&ptr_sf_seek)=(void*)GetProcAddress(g_hLibSndFile,"sf_seek");
        if (!ptr_sf_seek) errcnt++;
        *((void **)&ptr_sf_readf_double)=(void*)GetProcAddress(g_hLibSndFile,"sf_readf_double");
        if (!ptr_sf_readf_double) errcnt++;
        *((void **)&ptr_sf_version_string)=(void*)GetProcAddress(g_hLibSndFile,"sf_version_string");
        if (!ptr_sf_version_string) errcnt++;
        //OutputDebugStringA("libsndfile functions loaded!");

    } //else OutputDebugStringA("libsndfile DLL not loaded!");
    return errcnt;
}
