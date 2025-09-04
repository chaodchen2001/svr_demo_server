/**
 * @copyright:  2023 Ace. All Rights Reserved.
 * @filename:   ace_mtp_sdk_import.c
 * @version:    1.0.1
 * @brief:      This file provides the interface implementation of loading and unloading AceSDK.
 * @details:
 */

#include "ace_mtp_sdk.h"

#define ACE_SDK_INIT_WITH_MTP_INTERF_SYM "ace_sdk_init_with_mtp_interf"
#define ACE_SDK_INIT_WITH_MTP_INTERF_ASIA_SYM "ace_sdk_init_with_mtp_interf_asia"
#define ACE_SDK_RELEASE_BUSI_INTERF_SYM "ace_sdk_release_busi_interf"

typedef int (*AceSdkInitWithMTPInterf)(const AceSdkMTPInitInfo *init_info);
typedef int(*AceSdkReleaseBusiInterf)();
typedef const void *(*AceSdkGetInterf)(const void *init_data);
static int g_is_sdk_loaded = 0;

#if defined(WIN32) || defined(_WIN64)

    #include <windows.h>

    #define LIB_NAME    _T("ace_sdk")
    #define SUFFIX      _T("dll")
    #define snprintf    _snprintf
    HMODULE g_handle = NULL;

#else   /* #if defined(WIN32) || defined(_WIN64) */
    #include <dlfcn.h>
    #define LIB_NAME    "libace_sdk"
    #define SUFFIX      "so"
    void *g_handle = NULL;

#endif  /* #if defined(WIN32) || defined(_WIN64) */

#if defined(WIN32) || defined(_WIN64)

int
ace_sdk_load_win(const ACE_TCHAR *shared_lib_file)
{
    if (g_handle != NULL)
    {
        // Has been loaded, no longer load
        return 0;
    }

    g_handle = LoadLibraryEx(shared_lib_file, NULL, 0);

    if (g_handle == NULL)
    {
#pragma warning(push)
#pragma warning(disable:4477)
        _ftprintf(stderr,
                  _T("Open DLL %s failed, error code is %ld.\n"),
                  shared_lib_file,
                  GetLastError());
#pragma warning(pop)
        return -1;
    }

    return 0;
}

void *
ace_sdk_get_syml_win(const char *syml_name)
{
    void *func = NULL;

    if (g_handle == NULL)
    {
        return NULL;
    }

#pragma warning(push)
#pragma warning(disable:4054)
    func = GetProcAddress(g_handle, syml_name);
#pragma warning(pop)

    if (func == NULL)
    {
        _ftprintf(stderr,
                  _T("GetProcAddress failed, error code is %ld.\n"),
                  GetLastError());
        return NULL;
    }

    return func;
}

#else /* #if defined(WIN32) || defined(_WIN64) */

static int
ace_sdk_load_linux(const char *shared_lib_file)
{
    char *error = NULL;

    if (g_handle != NULL)
    {
        // Has been loaded, no longer load
        return 0;
    }

    g_handle = dlopen(shared_lib_file, RTLD_NOW);
    error = dlerror();

    if (error)
    {
        fprintf(stderr, "Open shared lib %s failed, %s.\n", shared_lib_file, error);
        return -1;
    }

    return 0;
}

void *
ace_sdk_get_syml_linux(const char *syml_name)
{
    void *func = NULL;
    char *error = NULL;

    if (g_handle == NULL)
    {
        return NULL;
    }

    func = dlsym(g_handle, syml_name);
    error = dlerror();

    if (error)
    {
        fprintf(stderr, "dlsym failed, %s.\n", error);
        return NULL;
    }

    return func;
}

#endif /* #if defined(WIN32) || defined(_WIN64) */

int
ace_sdk_load_impl(const ACE_TCHAR *shared_lib_file)
{
    int ret = 0;

    if (g_is_sdk_loaded)
    {
        return 0;
    }

#if defined(WIN32) || defined(_WIN64)
    ret = ace_sdk_load_win(shared_lib_file);
#else
    ret = ace_sdk_load_linux(shared_lib_file);
#endif

    if (ret == 0)
    {
        g_is_sdk_loaded = 1;
    }

    return ret;
}

void *
ace_sdk_get_syml_impl(const char *syml_name)
{
    void *func = NULL;

    if (!g_is_sdk_loaded)
    {
        return NULL;
    }

#if defined(WIN32) || defined(_WIN64)
    func = ace_sdk_get_syml_win(syml_name);
#else
    func = ace_sdk_get_syml_linux(syml_name);
#endif
    return func;
}

/* init AceSdk */
static int
ace_sdk_init(const AceSdkMTPInitInfo *init_info, const char *syml_name)
{
    if (init_info == NULL || init_info->ace_sdk_lib_dir_ == NULL || syml_name == NULL)
    {
        return -1;
    }
    
    AceSdkInitWithMTPInterf func = NULL;
#if defined(WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable:4055)
#endif
    func = (AceSdkInitWithMTPInterf)ace_sdk_get_syml_impl(syml_name);
#if defined(WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

    if (func != NULL)
    {
        return func(init_info);
    }

    return -1;
}

/* load AceSdk */
int
ace_sdk_load(const AceSdkMTPInitInfo *init_info)
{
    ACE_TCHAR shared_lib_file[1024] = {0};

    if (init_info == NULL || init_info->ace_sdk_lib_dir_ == NULL)
    {
        return -1;
    }

#if (defined(WIN32) || defined(_WIN64))
#define FMT (_T("%s/%s.%s"))
#else
#define FMT "%s/%s.%s"
#endif
    snprintf(shared_lib_file,
             sizeof(shared_lib_file) - 1,
             FMT,
             init_info->ace_sdk_lib_dir_,
             LIB_NAME,
             SUFFIX);
    return ace_sdk_load_impl(shared_lib_file);
}

/* load and initialize AceSdk */
int
ace_sdk_load_and_init(const AceSdkMTPInitInfo *init_info)
{
    int ret = 0;
    if (init_info == NULL || init_info->ace_sdk_lib_dir_ == NULL)
    {
        return -1;
    }

    ret = ace_sdk_load(init_info);
    if (ret != 0)
    {
        // load library fail
        return ret;
    }
    
    return ace_sdk_init(init_info, ACE_SDK_INIT_WITH_MTP_INTERF_SYM);
}

int
ace_sdk_load_and_init_asia(const AceSdkMTPInitInfo *init_info)
{
    int ret = 0;
    if (init_info == NULL || init_info->ace_sdk_lib_dir_ == NULL)
    {
        return -1;
    }

    ret = ace_sdk_load(init_info);
    if (ret != 0)
    {
        // load library fail
        return ret;
    }
    
    return ace_sdk_init(init_info, ACE_SDK_INIT_WITH_MTP_INTERF_ASIA_SYM);
}

/* unload AceSdk */
int
ace_sdk_unload()
{
    int rc = 0;
    AceSdkReleaseBusiInterf func = NULL;
#if defined(WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable:4055)
#endif
    func = (AceSdkReleaseBusiInterf)ace_sdk_get_syml_impl(ACE_SDK_RELEASE_BUSI_INTERF_SYM);
#if defined(WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

    if (func != NULL)
    {
        // call release function
        func();
    }

#if defined(WIN32) || defined(_WIN64)
    rc = FreeLibrary(g_handle);

    if (!rc)
    {
        return -2;
    }

    _ftprintf(stdout, _T("Unload ACE SDK OK.\n"));
#else
    rc = dlclose(g_handle);

    if (rc != 0)
    {
        return -2;
    }

    fprintf(stdout, "Unload ACE SDK OK.\n");
#endif
    g_is_sdk_loaded = 0;
    return 0;
}

// get AceSdk interface of a particular business
const void *ace_sdk_get_busi_interf(const char *syml_name, const void *data)
{
    AceSdkGetInterf func = NULL;
#if defined(WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable:4055)
#endif
    func = (AceSdkGetInterf)ace_sdk_get_syml_impl(syml_name);
#if defined(WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

    if (func != NULL)
    {
        return func(data);
    }

    // do not find the corresponding function
    return NULL;
}

