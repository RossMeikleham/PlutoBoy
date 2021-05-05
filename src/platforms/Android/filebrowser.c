#include "SDL.h"

#include "../../../src/non_core/filebrowser.h"
#include "../../../src/non_core/logger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <android/log.h>
#include <jni.h>


static jobject activity = (jobject)NULL;
static jmethodID getFilesInDir = (jmethodID)NULL;
static jmethodID isDir = (jmethodID)NULL;
static jmethodID assetData = (jmethodID)NULL;
static jmethodID getPath = (jmethodID)NULL;
static JNIEnv *env = NULL;


static int check_env_setup() {
        
    if (env == NULL)
    {
        env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    }

    if (activity == NULL)
    {
        activity = (jobject)SDL_AndroidGetActivity();
    }


    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass c = (*env)->GetObjectClass(env, activity);
    if (c == 0) {
        log_message(LOG_ERROR, "JNI: Unable to find the \"PlutoboyActivity\" class\n");
        return 0; 
    }

    if (getFilesInDir == NULL)
    {
        getFilesInDir = (*env)->GetMethodID(env, c, "getFilesForPath", "(Ljava/lang/String;)[Ljava/lang/String;");
        if (getFilesInDir == NULL)
        { 
            log_message(LOG_ERROR, "JNI: Unable to find the \"getFilesInDir\" method id\n");
            return -1;
        }
    }

    if (isDir == NULL)
    {
        isDir = (*env)->GetMethodID(env, c, "isDir", "(Ljava/lang/String;)Z");
        if (isDir == NULL)
        { 
            log_message(LOG_ERROR, "JNI: Unable to find the \"isDir\" method id\n");
            return -1;
        }
    }

    if (assetData == NULL)
    {
        assetData = (*env)->GetMethodID(env, c, "assetData", "(Ljava/lang/String;)[B");
        if (assetData == NULL)
        { 
            log_message(LOG_ERROR, "JNI: Unable to find the \"assetData\" method id\n");
            return -1;
        }
    }

    if (getPath == NULL)
    {
        getPath = (*env)->GetMethodID(env, c, "getPath", "()Ljava/lang/String;");
        if (getPath == NULL)
        { 
            log_message(LOG_ERROR, "JNI: Unable to find the \"getPath\" method id\n");
            return -1;
        }
    }


    return 0;
}
/*  
const char *get_romfile() {
    
    check_env_setup(); 
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDL Activity

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass c = (*env)->GetObjectClass(env, activity);
    if (c == 0) {
        log_message(LOG_ERROR, "JNI: Unable to find the \"PlutoboyActivity\" class\n");
        return NULL; 
    }

    // Get the method id
    jmethodID mid = (*env)->GetMethodID(env, c, "getFile", "()Ljava/lang/String;");
    if (mid == 0) {
        log_message(LOG_ERROR, "JNI: Unable to find the \"getFile\" method id\n");
        return NULL; 
    }
    
    jstring result = (jstring)((*env)->CallObjectMethod(env, activity, mid)); 
    const char *path = (*env)->GetStringUTFChars(env, result, NULL);

    // clean up the local references.
    (*env)->DeleteLocalRef(env, activity);
    (*env)->DeleteLocalRef(env, c);

    return path;
}*/

dir_t *get_dir(const char * const dir_path) {
   
    check_env_setup();

    log_message(LOG_INFO, "Getting dirn\n");
    // Can get filenames, need to know if dir as well
    jstring jpath = (jstring)(*env)->NewStringUTF(env, dir_path);
    log_message(LOG_INFO, "got jpath\n");
    jobjectArray file_paths = (jobjectArray)((*env)->CallObjectMethod(env, activity, getFilesInDir, jpath));

    log_message(LOG_INFO, "got files in dir %p\n", file_paths);
    if (file_paths == NULL)
    {
        return NULL;
    }

    int count = (*env)->GetArrayLength(env, file_paths);
    
    log_message(LOG_INFO, "got %d files\n", count);

    dir_t *dir = calloc(1, sizeof(dir_t));
    if (dir == NULL)
    {
        goto err;
    }

    log_message(LOG_INFO, "dir path %s\n", dir_path);

    // Each Path is probably nowhere near PATH_MAX in size
    jstring jcur_path = (jstring)((*env)->CallObjectMethod(env, activity, getPath));
    const char *cur_path = (*env)->GetStringUTFChars(env, jcur_path, NULL);
    dir->path = strdup(cur_path);
    (*env)->ReleaseStringUTFChars(env, jcur_path, cur_path);

    dir->entries = calloc(count, sizeof(dir_entry_t));
    if (dir->entries == NULL)
    {
        goto err;
    }

    for (uint32_t i = 0; i < count; i++) 
    {    
        jstring jname = (*env)->GetObjectArrayElement(env, file_paths, i);
        const char *path = (*env)->GetStringUTFChars(env, jname, NULL);
        char *our_path = strdup(path);
        log_message(LOG_INFO, "files in dir: %s\n", our_path);
        dir->entries[i].name = our_path;
        (*env)->ReleaseStringUTFChars(env, jname, path);
        jboolean is_dir = (jboolean)((*env)->CallBooleanMethod(env, activity, isDir, jname));
        dir->entries[i].is_dir = is_dir == JNI_TRUE;
        log_message(LOG_INFO, "is dir: %d\n", dir->entries[i].is_dir);
    }

    dir->entry_count = count;

    return dir;

    err:        
        free_dir(dir);
        return NULL; 
}


void free_dir(dir_t * const dir)
{
    if (dir != NULL)
    {
        if (dir->path != NULL)
        {
            free(dir->path);
        }

        if (dir->entries != NULL)
        {
            for (int32_t i = 0; i < dir->entry_count; i++)
            {
                if (dir->entries[i].name != NULL)
                {
                    free(dir->entries[i].name);
                }
            }
            free(dir->entries);
        }
        free(dir);
    }
}

uint8_t *load_asset(const char * const path, int *size)
{

    jstring jpath = (jstring)(*env)->NewStringUTF(env, path);
    log_message(LOG_INFO, "got jpath\n");
    
    jbyteArray *jasset_data = (jbyteArray*)((*env)->CallObjectMethod(env, activity, assetData, jpath));
    if (jasset_data == NULL)
    {
        log_message(LOG_INFO, "Failed to load asset: %s\n", path);
        return NULL;
    } 
        
    log_message(LOG_INFO, "Loaded asset data\n");

    int length = (*env)->GetArrayLength(env, jasset_data);
    jbyte *jasset_bytes = (*env)->GetByteArrayElements(env, jasset_data, 0);

    uint8_t *asset_data = calloc(1, length);
    if (asset_data == NULL)
    {
        (*env)->ReleaseByteArrayElements(env, jasset_data, jasset_bytes, 0);
        return NULL;
    }

    memcpy(asset_data, jasset_bytes, length);
    (*env)->ReleaseByteArrayElements(env, jasset_data, jasset_bytes, 0);
    
    *size = length;
    return asset_data;
}
