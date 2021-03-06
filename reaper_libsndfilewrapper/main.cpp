#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"
#include "libsndfileImport.h"
#include <Windows.h>
#include "wrapperclass.h"
#define IMPAPI(x) if (!((*((void **)&(x)) = (void *)rec->GetFunc(#x)))) impapierrcnt++;

REAPER_PLUGIN_HINSTANCE g_hInst=0;

PCM_source *(*PCM_Source_CreateFromSimple)(ISimpleMediaDecoder *dec, const char *fn);
void (*format_timestr)(double tpos, char *buf, int buflen);
void (*update_disk_counters)(int read, int write);
void (*ShowConsoleMsg)(const char* msg);

// output diagnostics messages using Reaper's currently available console
#define REAPER_DEBUG_OUTPUT_TRACING

PCM_source *CreateFromType(const char *type, int priority)
{
    if (priority > 4) // let other plug-ins override "RAW" if they want. or whatever in this case...
    {
        if (!strcmp(type,"XLSFW"))
            return PCM_Source_CreateFromSimple(new LSFW_SimpleMediaDecoder,NULL);
    }

    return NULL;
}

PCM_source *CreateFromFile(const char *filename, int priority)
{
    int lfn=strlen(filename);
    if (priority > 4 && lfn>4 && !stricmp(filename+lfn-4,".paf"))
    {
        PCM_source *w=PCM_Source_CreateFromSimple(new LSFW_SimpleMediaDecoder,filename);
        if (w->IsAvailable() || priority >= 7) return w;
        delete w;
    }
    if (priority > 4 && lfn>4 && !stricmp(filename+lfn-4,".caf"))
    {
        PCM_source *w=PCM_Source_CreateFromSimple(new LSFW_SimpleMediaDecoder,filename);
        if (w->IsAvailable() || priority >= 7) return w;
        delete w;
    }
#ifdef SO_CALLED_SD2_SUPPORT
    if (priority > 4 && lfn>4 && !stricmp(filename+lfn-4,".sd2"))
    {
        PCM_source *w=PCM_Source_CreateFromSimple(new LSFW_SimpleMediaDecoder,filename);
        if (w->IsAvailable() || priority >= 7) return w;
        delete w;
    }
#endif
    return NULL;
}

// this is used for UI only, not so muc
const char *EnumFileExtensions(int i, char **descptr) // call increasing i until returns a string, if descptr's output is NULL, use last description
{
    if (i == 0)
    {
        if (descptr) *descptr = "Paris Audio Files";
        return "PAF";
    }
    if (i == 1)
    {
        if (descptr) *descptr = "Apple Core Audio Files";
        return "CAF";
    }
#ifdef SO_CALLED_SD2_SUPPORT
    if (i == 2)
    {
        if (descptr) *descptr = "Sound Designer II Files";
        return "sd2";
    }
#endif
    if (descptr) *descptr=NULL;
    return NULL;
}


pcmsrc_register_t myRegStruct={CreateFromType,CreateFromFile,EnumFileExtensions};

const char *(*GetExePath)();

extern "C"
{

REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec)
{
    //ShowConsoleMsg("Entering entrypoint function in libsndfile wrapper extension");
    g_hInst=hInstance;
    //g_EntrypointCallCount++;
    if (rec)
    {
        if (rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc)
            return 0;
        int impapierrcnt=0;
        IMPAPI(GetExePath);
        IMPAPI(PCM_Source_CreateFromSimple);
        IMPAPI(format_timestr);
        IMPAPI(update_disk_counters);
        IMPAPI(ShowConsoleMsg);
        if (impapierrcnt)
        {
            ShowConsoleMsg("Errors importing Reaper API functions, aborting loading Xenakios Libsndfile wrapper");
            return 0;
        }
        // we load libsndfile dll and resolve functions manually. if the libsndfile dll import .lib was used
        // the libsndfile dll would need to be in some inconvenient location for the end user
        char lsfpath[2048];
        sprintf(lsfpath,"%s\\Plugins\\libsndfile-1.dll",GetExePath());
#ifdef REAPER_DEBUG_OUTPUT_TRACING
        //ShowConsoleMsg(lsfpath);
#endif
        if (ImportLibSndFileFunctions(lsfpath)>0)
        {
            // loading libsndfile/resolving functions failed
            //ShowConsoleMsg("Errors loading libsndfile, aborting loading wrapper");
            return 0;
        }
        rec->Register("pcmsrc",&myRegStruct);
        return 1;
    }
    return 0;
}
}
