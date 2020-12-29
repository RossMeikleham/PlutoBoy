
// Android main file

#include "SDL.h"

#include "../../../src/core/emu.h"
#include "../../../src/non_core/logger.h"
#include "../../../src/non_core/menu.h"

#include <android/log.h>
#include <jni.h>

const char *get_romfile() {
   
    // Retrieve the JNI environment
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDL Activity
    jobject activity = (jobject)SDL_AndroidGetActivity();

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
}


char *get_full_path(char *fname)
{
    // Retrieve the JNI environment
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDL Activity
    jobject activity = (jobject)SDL_AndroidGetActivity();

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass c = (*env)->GetObjectClass(env, activity);
    if (c == 0) {
        log_message(LOG_ERROR, "JNI: Unable to find the \"PlutoboyActivity\" class\n");
        return NULL; 
    }

	jmethodID getPath = (*env)->GetMethodID(env, c, "getPath", "()Ljava/lang/String;");
    if (getPath == NULL)
    { 
   		log_message(LOG_ERROR, "JNI: Unable to find the \"getPath\" method id\n");
            return 0;
    }
    
    jstring jcur_path = (jstring)((*env)->CallObjectMethod(env, activity, getPath));
    const char *cur_path = (*env)->GetStringUTFChars(env, jcur_path, NULL);

	char *full_path = calloc(1, strlen(fname) + strlen(cur_path) + 1);
	memcpy(full_path, cur_path, strlen(cur_path));
	memcpy(full_path + strlen(cur_path), fname, strlen(fname));

    (*env)->ReleaseStringUTFChars(env, jcur_path, cur_path);
	
	log_message(LOG_INFO, "Full path: %s\n:", full_path);
	return full_path;
}


int main(int argc, char *argv[]) {

    
    int res;
    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Initing Emu");
    //const char *rom_path = get_romfile();

    char *file_name = NULL;
    int ret_val = 0;
    log_message(LOG_INFO, "Menu\n");
    int result = ui_menu(&file_name, &ret_val);
    if (result != 0 || ret_val != 0)
    {
        return -1;
    }

	char *file_path = file_name; //get_full_path(file_name);

    res = init_emu(file_path, 0, 0, NO_CONNECT);
    
    if (res) {
        run();
    }

    return 0;
}
