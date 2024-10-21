#include <jni.h>
#include "checkupcomputing.h"

using namespace kolibree;

template<typename EnumType>
Brushing<EnumType> convertJavaBrushingToCpp(JNIEnv* env, jobject brushingObj, jclass enumClass)
{
    Brushing<EnumType> brushing;

    jfieldID passesFieldID = env->GetFieldID(env->GetObjectClass(brushingObj), "passes", "Ljava/util/ArrayList;");
    jobject passesObj = env->GetObjectField(brushingObj, passesFieldID);

    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jmethodID sizeMethod = env->GetMethodID(arrayListClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(arrayListClass, "get", "(I)Ljava/lang/Object;");

    jint numPasses = env->CallIntMethod(passesObj, sizeMethod);

    for (jint i = 0; i < numPasses; i++) {
        jobject passObj = env->CallObjectMethod(passesObj, getMethod, i);
        jclass passClass = env->GetObjectClass(passObj);

        jfieldID zoneFieldID = env->GetFieldID(passClass, "zone", "Ljava/lang/Enum;");
        jfieldID timestampFieldID = env->GetFieldID(passClass, "timestamp", "F");
        jfieldID durationFieldID = env->GetFieldID(passClass, "duration", "F");

        jobject zoneObj = env->GetObjectField(passObj, zoneFieldID);
        jfloat timestamp = env->GetFloatField(passObj, timestampFieldID);
        jfloat duration = env->GetFloatField(passObj, durationFieldID);

        jmethodID ordinalMethod = env->GetMethodID(enumClass, "ordinal", "()I");
        jint ordinal = env->CallIntMethod(zoneObj, ordinalMethod);

        brushing.passes.push_back({timestamp, duration, static_cast<EnumType>(ordinal)});

        env->DeleteLocalRef(passObj);
        env->DeleteLocalRef(zoneObj);
    }

    env->DeleteLocalRef(passesObj);
    env->DeleteLocalRef(arrayListClass);

    return brushing;
}

template<typename EnumType>
jobject convertCppCheckupToJava(JNIEnv* env, const Checkup<EnumType>& checkup, jclass enumClass)
{
    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID hashMapConstructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jobject resultMap = env->NewObject(hashMapClass, hashMapConstructor);

    jmethodID putMethod = env->GetMethodID(hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jclass floatClass = env->FindClass("java/lang/Float");
    jmethodID floatConstructor = env->GetMethodID(floatClass, "<init>", "(F)V");

    for (int i = 0; i < getNumberOfZones<EnumType>(); ++i) {
        EnumType zone = static_cast<EnumType>(i);
        float coverage = checkup.coverageForZone(zone);

        jmethodID valueOfMethod = env->GetStaticMethodID(enumClass, "valueOf", "(Ljava/lang/String;)Ljava/lang/Enum;");
        jstring zoneName = env->NewStringUTF(enumToString(zone).c_str());
        jobject zoneEnum = env->CallStaticObjectMethod(enumClass, valueOfMethod, zoneName);

        jobject coverageFloat = env->NewObject(floatClass, floatConstructor, coverage);

        env->CallObjectMethod(resultMap, putMethod, zoneEnum, coverageFloat);

        env->DeleteLocalRef(zoneName);
        env->DeleteLocalRef(zoneEnum);
        env->DeleteLocalRef(coverageFloat);
    }

    env->DeleteLocalRef(hashMapClass);
    env->DeleteLocalRef(floatClass);

    return resultMap;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_kolibree_checkupsdk_CheckupComputer_computeCheckup(
        JNIEnv* env,
        jobject /* this */,
        jobject brushingObj) {

    jclass brushingClass = env->GetObjectClass(brushingObj);
    jfieldID numZonesFieldID = env->GetFieldID(brushingClass, "numZones", "I");
    jint numZones = env->GetIntField(brushingObj, numZonesFieldID);

    CheckupComputer computer;

    switch (numZones) {
        case 8: {
            jclass enumClass = env->FindClass("com/kolibree/MouthZones8");
            auto brushing = convertJavaBrushingToCpp<MouthZones8>(env, brushingObj, enumClass);
            auto checkup = computer.computeCheckup(brushing);
            return convertCppCheckupToJava(env, checkup, enumClass);
        }
        case 12: {
            jclass enumClass = env->FindClass("com/kolibree/MouthZones12");
            auto brushing = convertJavaBrushingToCpp<MouthZones12>(env, brushingObj, enumClass);
            auto checkup = computer.computeCheckup(brushing);
            return convertCppCheckupToJava(env, checkup, enumClass);
        }
        case 16: {
            jclass enumClass = env->FindClass("com/kolibree/MouthZones16");
            auto brushing = convertJavaBrushingToCpp<MouthZones16>(env, brushingObj, enumClass);
            auto checkup = computer.computeCheckup(brushing);
            return convertCppCheckupToJava(env, checkup, enumClass);
        }
        default:
            // Handle error: unsupported number of zones
            return nullptr;
    }
}
