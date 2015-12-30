
// Android main file

#include "SDL.h"

#include "../../../src/core/emu.h"
#include "../../../src/non_core/logger.h"

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



int main(int argc, char *argv[]) {

    
    int res;
    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Initing Emu");
    const char *rom_path = get_romfile();

    res = init_emu(rom_path, 0, 0, NO_CONNECT);
    
    if (res) {
        run();
    }

    return 0;
}
