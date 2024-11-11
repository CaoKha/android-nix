#include "checkupcomputing.h"
#include <algorithm>
#include <iostream>
#include <jni.h>
#include <string>

using namespace kolibree;

// Helper function for JNI class descriptor from Java class
std::string getClassDescriptor(JNIEnv *env, jclass javaClass) {
  jclass classClass = env->FindClass("java/lang/Class");
  jmethodID getNameMethodID =
      env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
  jstring classNameJavaString =
      (jstring)env->CallObjectMethod(javaClass, getNameMethodID);
  const char *classNameCStr =
      env->GetStringUTFChars(classNameJavaString, nullptr);
  std::string classNameStr(classNameCStr);
  env->ReleaseStringUTFChars(classNameJavaString, classNameCStr);
  env->DeleteLocalRef(classNameJavaString);
  env->DeleteLocalRef(classClass);
  std::replace(classNameStr.begin(), classNameStr.end(), '.', '/');
  return "L" + classNameStr + ";";
}

// Macro for defining convertJavaBrushingToCpp functions
#define DEFINE_CONVERT_JAVA_BRUSHING_TO_CPP_FUNC(numZones, MouthZonesType)     \
  Brushing<MouthZonesType> convertJavaBrushingToCpp##numZones(                 \
      JNIEnv *env, jobject javaBrushingObj) {                                  \
    Brushing<MouthZonesType> cppBrushing;                                      \
    jclass brushingClass = env->GetObjectClass(javaBrushingObj);               \
    jfieldID passesFieldID =                                                   \
        env->GetFieldID(brushingClass, "passes", "Ljava/util/List;");          \
    if (!passesFieldID) {                                                      \
      std::cerr << "Field 'passes' not found in Brushing" #numZones "."        \
                << std::endl;                                                  \
      env->DeleteLocalRef(brushingClass);                                      \
      return cppBrushing;                                                      \
    }                                                                          \
    jobject passesListObj =                                                    \
        env->GetObjectField(javaBrushingObj, passesFieldID);                   \
    if (!passesListObj) {                                                      \
      std::cerr << "'passes' field is null in Brushing" #numZones "."          \
                << std::endl;                                                  \
      env->DeleteLocalRef(brushingClass);                                      \
      return cppBrushing;                                                      \
    }                                                                          \
    jclass listClass = env->GetObjectClass(passesListObj);                     \
    jmethodID sizeMethodID = env->GetMethodID(listClass, "size", "()I");       \
    jmethodID getMethodID =                                                    \
        env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");           \
    jint numPasses = env->CallIntMethod(passesListObj, sizeMethodID);          \
    for (jint i = 0; i < numPasses; i++) {                                     \
      jobject javaPassObj =                                                    \
          env->CallObjectMethod(passesListObj, getMethodID, i);                \
      jclass passClass = env->GetObjectClass(javaPassObj);                     \
      jfieldID zoneFieldID = env->GetFieldID(                                  \
          passClass, "zone", "Lcom/kolibree/MouthZones" #numZones ";");        \
      jfieldID timestampFieldID =                                              \
          env->GetFieldID(passClass, "timestamp", "F");                        \
      jfieldID durationFieldID = env->GetFieldID(passClass, "duration", "F");  \
      jobject zoneObj = env->GetObjectField(javaPassObj, zoneFieldID);         \
      jfloat timestamp = env->GetFloatField(javaPassObj, timestampFieldID);    \
      jfloat duration = env->GetFloatField(javaPassObj, durationFieldID);      \
      jclass mouthZonesClass =                                                 \
          env->FindClass("com/kolibree/MouthZones" #numZones);                 \
      jmethodID ordinalMethodID =                                              \
          env->GetMethodID(mouthZonesClass, "ordinal", "()I");                 \
      jint ordinal = env->CallIntMethod(zoneObj, ordinalMethodID);             \
      cppBrushing.passes.push_back(BrushingPass<MouthZonesType>{               \
          timestamp, duration, static_cast<MouthZonesType>(ordinal)});         \
      env->DeleteLocalRef(zoneObj);                                            \
      env->DeleteLocalRef(passClass);                                          \
      env->DeleteLocalRef(javaPassObj);                                        \
      env->DeleteLocalRef(mouthZonesClass);                                    \
    }                                                                          \
    env->DeleteLocalRef(passesListObj);                                        \
    env->DeleteLocalRef(listClass);                                            \
    env->DeleteLocalRef(brushingClass);                                        \
    return cppBrushing;                                                        \
  }

// Macro for defining convertCppCheckupToJava functions
#define DEFINE_CONVERT_CPP_CHECKUP_TO_JAVA_FUNC(numZones, MouthZonesType)      \
  jobject convertCppCheckupToJava##numZones(                                   \
      JNIEnv *env, const Checkup<MouthZonesType> &cppCheckup) {                \
    jclass hashMapClass = env->FindClass("java/util/HashMap");                 \
    jmethodID hashMapConstructorID =                                           \
        env->GetMethodID(hashMapClass, "<init>", "()V");                       \
    jobject javaHashMap = env->NewObject(hashMapClass, hashMapConstructorID);  \
    jmethodID putMethodID = env->GetMethodID(                                  \
        hashMapClass, "put",                                                   \
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");           \
    jclass mouthZonesClass =                                                   \
        env->FindClass("com/kolibree/MouthZones" #numZones);                   \
    jmethodID valueOfMethodID = env->GetStaticMethodID(                        \
        mouthZonesClass, "valueOf",                                            \
        "(Ljava/lang/String;)Lcom/kolibree/MouthZones" #numZones ";");         \
    jclass floatClass = env->FindClass("java/lang/Float");                     \
    jmethodID floatConstructorID =                                             \
        env->GetMethodID(floatClass, "<init>", "(F)V");                        \
    for (int i = 0; i < numZones; ++i) {                                       \
      MouthZonesType zone = static_cast<MouthZonesType>(i);                    \
      float coverage = cppCheckup.coverageForZone(zone);                       \
      std::string zoneStr = enumToString(zone);                                \
      jstring zoneName = env->NewStringUTF(zoneStr.c_str());                   \
      jobject zoneEnumObj = env->CallStaticObjectMethod(                       \
          mouthZonesClass, valueOfMethodID, zoneName);                         \
      jobject coverageFloatObj =                                               \
          env->NewObject(floatClass, floatConstructorID, coverage);            \
      env->CallObjectMethod(javaHashMap, putMethodID, zoneEnumObj,             \
                            coverageFloatObj);                                 \
      env->DeleteLocalRef(zoneName);                                           \
      env->DeleteLocalRef(zoneEnumObj);                                        \
      env->DeleteLocalRef(coverageFloatObj);                                   \
    }                                                                          \
    env->DeleteLocalRef(hashMapClass);                                         \
    env->DeleteLocalRef(mouthZonesClass);                                      \
    env->DeleteLocalRef(floatClass);                                           \
    return javaHashMap;                                                        \
  }

// Macro for defining JNI computeCheckup functions
#define DEFINE_COMPUTE_CHECKUP_JNI_FUNC(numZones, MouthZonesType)              \
  extern "C" JNIEXPORT jobject JNICALL                                         \
      Java_com_kolibree_CheckupComputer_computeCheckup##numZones(              \
          JNIEnv *env, jobject /* this */, jobject javaBrushingObj) {          \
    Brushing<MouthZonesType> cppBrushing =                                     \
        convertJavaBrushingToCpp##numZones(env, javaBrushingObj);              \
    CheckupComputer checkupComputer;                                           \
    Checkup<MouthZonesType> cppCheckup =                                       \
        checkupComputer.computeCheckup(cppBrushing);                           \
    return convertCppCheckupToJava##numZones(env, cppCheckup);                 \
  }

// Define the functions for each number of zones
DEFINE_CONVERT_JAVA_BRUSHING_TO_CPP_FUNC(8, MouthZones8)
DEFINE_CONVERT_CPP_CHECKUP_TO_JAVA_FUNC(8, MouthZones8)
DEFINE_COMPUTE_CHECKUP_JNI_FUNC(8, MouthZones8)

DEFINE_CONVERT_JAVA_BRUSHING_TO_CPP_FUNC(12, MouthZones12)
DEFINE_CONVERT_CPP_CHECKUP_TO_JAVA_FUNC(12, MouthZones12)
DEFINE_COMPUTE_CHECKUP_JNI_FUNC(12, MouthZones12)

DEFINE_CONVERT_JAVA_BRUSHING_TO_CPP_FUNC(16, MouthZones16)
DEFINE_CONVERT_CPP_CHECKUP_TO_JAVA_FUNC(16, MouthZones16)
DEFINE_COMPUTE_CHECKUP_JNI_FUNC(16, MouthZones16)
