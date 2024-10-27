#include "checkupcomputing.h"
#include <algorithm>
#include <iostream>
#include <jni.h> // JNI header provided by JDK
#include <string>

using namespace kolibree;

// Helper function to get the JNI class descriptor from a Java class
std::string getClassDescriptor(JNIEnv *env, jclass javaClass) {
  // Get the java.lang.Class class
  jclass classClass = env->FindClass("java/lang/Class");

  // Get the method ID for the 'getName' method of java.lang.Class
  jmethodID getNameMethodID =
      env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");

  // Call 'getName' on the javaClass object to get the class name as a Java
  // string
  jstring classNameJavaString =
      (jstring)env->CallObjectMethod(javaClass, getNameMethodID);

  // Convert the Java string to a C-style string
  const char *classNameCStr =
      env->GetStringUTFChars(classNameJavaString, nullptr);

  // Create a std::string from the C-style string
  std::string classNameStr(classNameCStr);

  // Release the Java string resources
  env->ReleaseStringUTFChars(classNameJavaString, classNameCStr);

  // Clean up local references
  env->DeleteLocalRef(classNameJavaString);
  env->DeleteLocalRef(classClass);

  // Replace dots with slashes to get the JNI class descriptor format
  std::replace(classNameStr.begin(), classNameStr.end(), '.', '/');

  // Return the JNI class descriptor in the format 'Lcom/example/MyClass;'
  return "L" + classNameStr + ";";
}

// =====================
// JNI Function for Brushing8
// =====================

// Function to convert Java Brushing8 to C++ Brushing<MouthZones8>
Brushing<MouthZones8> convertJavaBrushingToCpp8(JNIEnv *env,
                                                jobject javaBrushingObj) {
  Brushing<MouthZones8> cppBrushing; // Create an empty C++ Brushing object

  // Get the class of the Java Brushing8 object
  jclass brushingClass = env->GetObjectClass(javaBrushingObj);

  // Get the field ID for the 'passes' field (which is a List)
  jfieldID passesFieldID =
      env->GetFieldID(brushingClass, "passes", "Ljava/util/List;");
  if (passesFieldID == nullptr) {
    // Handle error (field not found)
    std::cerr << "Field 'passes' not found in Brushing8." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the 'passes' field from the Java object
  jobject passesListObj = env->GetObjectField(javaBrushingObj, passesFieldID);
  if (passesListObj == nullptr) {
    // Handle error (field is null)
    std::cerr << "'passes' field is null in Brushing8." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the class of the List
  jclass listClass = env->GetObjectClass(passesListObj);

  // Get method IDs for 'size' and 'get' methods of List
  jmethodID sizeMethodID = env->GetMethodID(listClass, "size", "()I");
  jmethodID getMethodID =
      env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
  if (sizeMethodID == nullptr || getMethodID == nullptr) {
    // Handle error (methods not found)
    std::cerr << "Methods 'size' or 'get' not found in List." << std::endl;
    env->DeleteLocalRef(passesListObj);
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the number of passes in the list
  jint numPasses = env->CallIntMethod(passesListObj, sizeMethodID);

  // Iterate over each pass in the 'passes' list
  for (jint i = 0; i < numPasses; i++) {
    // Get the Pass object at index i
    jobject javaPassObj = env->CallObjectMethod(passesListObj, getMethodID, i);
    if (javaPassObj == nullptr) {
      continue; // Skip null passes
    }

    // Get the class of the Pass object
    jclass passClass = env->GetObjectClass(javaPassObj);

    // Get field IDs for 'zone', 'timestamp', and 'duration' fields
    jfieldID zoneFieldID =
        env->GetFieldID(passClass, "zone", "Lcom/kolibree/MouthZones8;");
    jfieldID timestampFieldID =
        env->GetFieldID(passClass, "timestamp", "F"); // 'F' for float
    jfieldID durationFieldID = env->GetFieldID(passClass, "duration", "F");
    if (zoneFieldID == nullptr || timestampFieldID == nullptr ||
        durationFieldID == nullptr) {
      // Handle error (fields not found)
      std::cerr << "Fields 'zone', 'timestamp', or 'duration' not found in "
                   "BrushingPass8."
                << std::endl;
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      continue;
    }

    // Get the values of the fields from the Pass object
    jobject zoneObj = env->GetObjectField(javaPassObj, zoneFieldID);
    jfloat timestamp = env->GetFloatField(javaPassObj, timestampFieldID);
    jfloat duration = env->GetFloatField(javaPassObj, durationFieldID);

    // Get the ordinal value of the enum (the index of the enum constant)
    jclass mouthZones8Class = env->FindClass("com/kolibree/MouthZones8");
    jmethodID ordinalMethodID =
        env->GetMethodID(mouthZones8Class, "ordinal", "()I");
    if (ordinalMethodID == nullptr) {
      // Handle error (method not found)
      std::cerr << "Method 'ordinal' not found in MouthZones8." << std::endl;
      env->DeleteLocalRef(zoneObj);
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      env->DeleteLocalRef(mouthZones8Class);
      continue;
    }
    jint ordinal = env->CallIntMethod(zoneObj, ordinalMethodID);

    // Create a C++ BrushingPass object and add it to the vector
    cppBrushing.passes.push_back(BrushingPass<MouthZones8>{
        timestamp, duration, static_cast<MouthZones8>(ordinal)});

    // Clean up local references
    env->DeleteLocalRef(zoneObj);
    env->DeleteLocalRef(passClass);
    env->DeleteLocalRef(javaPassObj);
    env->DeleteLocalRef(mouthZones8Class);
  }

  // Clean up local references
  env->DeleteLocalRef(passesListObj);
  env->DeleteLocalRef(listClass);
  env->DeleteLocalRef(brushingClass);

  return cppBrushing; // Return the populated C++ Brushing object
}

// Function to convert C++ Checkup<MouthZones8> to Java HashMap<MouthZones8,
// Float>
jobject convertCppCheckupToJava8(JNIEnv *env,
                                 const Checkup<MouthZones8> &cppCheckup) {
  // Create a new Java HashMap
  jclass hashMapClass = env->FindClass("java/util/HashMap");
  jmethodID hashMapConstructorID =
      env->GetMethodID(hashMapClass, "<init>", "()V");
  jobject javaHashMap = env->NewObject(hashMapClass, hashMapConstructorID);

  // Get the 'put' method ID
  jmethodID putMethodID = env->GetMethodID(
      hashMapClass, "put",
      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (putMethodID == nullptr) {
    std::cerr << "Method 'put' not found in HashMap." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the MouthZones8 class
  jclass mouthZones8Class = env->FindClass("com/kolibree/MouthZones8");
  if (mouthZones8Class == nullptr) {
    std::cerr << "Class 'MouthZones8' not found." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the 'valueOf' static method ID for MouthZones8
  jmethodID valueOfMethodID =
      env->GetStaticMethodID(mouthZones8Class, "valueOf",
                             "(Ljava/lang/String;)Lcom/kolibree/MouthZones8;");
  if (valueOfMethodID == nullptr) {
    std::cerr << "Method 'valueOf' not found in MouthZones8." << std::endl;
    env->DeleteLocalRef(mouthZones8Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the Float class and its constructor
  jclass floatClass = env->FindClass("java/lang/Float");
  jmethodID floatConstructorID = env->GetMethodID(floatClass, "<init>", "(F)V");
  if (floatConstructorID == nullptr) {
    std::cerr << "Constructor '<init>(F)V' not found in Float." << std::endl;
    env->DeleteLocalRef(floatClass);
    env->DeleteLocalRef(mouthZones8Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Iterate over each MouthZones8 enum constant
  for (int i = 0; i < 8; ++i) {
    MouthZones8 zone = static_cast<MouthZones8>(i);
    float coverage = cppCheckup.coverageForZone(zone);

    // Convert enum to string
    std::string zoneStr = enumToString(zone);
    jstring zoneName = env->NewStringUTF(zoneStr.c_str());

    // Convert the zone name string to a Java enum instance using 'valueOf'
    // method
    jobject zoneEnumObj = env->CallStaticObjectMethod(
        mouthZones8Class, valueOfMethodID, zoneName);
    env->DeleteLocalRef(zoneName);

    if (zoneEnumObj == nullptr) {
      std::cerr << "Failed to convert string to MouthZones8 enum." << std::endl;
      continue;
    }

    // Create a Java Float object for the coverage value
    jobject coverageFloatObj =
        env->NewObject(floatClass, floatConstructorID, coverage);

    // Put the (zoneEnumObj, coverageFloatObj) pair into the HashMap
    env->CallObjectMethod(javaHashMap, putMethodID, zoneEnumObj,
                          coverageFloatObj);

    // Clean up local references
    env->DeleteLocalRef(zoneEnumObj);
    env->DeleteLocalRef(coverageFloatObj);
  }

  // Clean up local references
  env->DeleteLocalRef(hashMapClass);
  env->DeleteLocalRef(mouthZones8Class);
  env->DeleteLocalRef(floatClass);

  return javaHashMap; // Return the populated Java HashMap object
}

// JNI function for 8 zones
extern "C" JNIEXPORT jobject JNICALL
Java_com_kolibree_CheckupComputer_computeCheckup8(JNIEnv *env,
                                                  jobject /* this */,
                                                  jobject javaBrushingObj) {

  // Convert Java Brushing8 to C++ Brushing<MouthZones8>
  Brushing<MouthZones8> cppBrushing =
      convertJavaBrushingToCpp8(env, javaBrushingObj);

  // Compute the checkup using the C++ CheckupComputer
  CheckupComputer checkupComputer; // Ensure this class is properly defined
  Checkup<MouthZones8> cppCheckup = checkupComputer.computeCheckup(cppBrushing);

  // Convert the C++ Checkup back to a Java HashMap
  jobject result = convertCppCheckupToJava8(env, cppCheckup);

  return result; // Return the populated Java HashMap object
}

// =====================
// JNI Function for Brushing12
// =====================

// Function to convert Java Brushing12 to C++ Brushing<MouthZones12>
Brushing<MouthZones12> convertJavaBrushingToCpp12(JNIEnv *env,
                                                  jobject javaBrushingObj) {
  Brushing<MouthZones12> cppBrushing; // Create an empty C++ Brushing object

  // Get the class of the Java Brushing12 object
  jclass brushingClass = env->GetObjectClass(javaBrushingObj);

  // Get the field ID for the 'passes' field (which is a List)
  jfieldID passesFieldID =
      env->GetFieldID(brushingClass, "passes", "Ljava/util/List;");
  if (passesFieldID == nullptr) {
    // Handle error (field not found)
    std::cerr << "Field 'passes' not found in Brushing12." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the 'passes' field from the Java object
  jobject passesListObj = env->GetObjectField(javaBrushingObj, passesFieldID);
  if (passesListObj == nullptr) {
    // Handle error (field is null)
    std::cerr << "'passes' field is null in Brushing12." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the class of the List
  jclass listClass = env->GetObjectClass(passesListObj);

  // Get method IDs for 'size' and 'get' methods of List
  jmethodID sizeMethodID = env->GetMethodID(listClass, "size", "()I");
  jmethodID getMethodID =
      env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
  if (sizeMethodID == nullptr || getMethodID == nullptr) {
    // Handle error (methods not found)
    std::cerr << "Methods 'size' or 'get' not found in List." << std::endl;
    env->DeleteLocalRef(passesListObj);
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the number of passes in the list
  jint numPasses = env->CallIntMethod(passesListObj, sizeMethodID);

  // Iterate over each pass in the 'passes' list
  for (jint i = 0; i < numPasses; i++) {
    // Get the Pass object at index i
    jobject javaPassObj = env->CallObjectMethod(passesListObj, getMethodID, i);
    if (javaPassObj == nullptr) {
      continue; // Skip null passes
    }

    // Get the class of the Pass object
    jclass passClass = env->GetObjectClass(javaPassObj);

    // Get field IDs for 'zone', 'timestamp', and 'duration' fields
    jfieldID zoneFieldID =
        env->GetFieldID(passClass, "zone", "Lcom/kolibree/MouthZones12;");
    jfieldID timestampFieldID =
        env->GetFieldID(passClass, "timestamp", "F"); // 'F' for float
    jfieldID durationFieldID = env->GetFieldID(passClass, "duration", "F");
    if (zoneFieldID == nullptr || timestampFieldID == nullptr ||
        durationFieldID == nullptr) {
      // Handle error (fields not found)
      std::cerr << "Fields 'zone', 'timestamp', or 'duration' not found in "
                   "BrushingPass12."
                << std::endl;
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      continue;
    }

    // Get the values of the fields from the Pass object
    jobject zoneObj = env->GetObjectField(javaPassObj, zoneFieldID);
    jfloat timestamp = env->GetFloatField(javaPassObj, timestampFieldID);
    jfloat duration = env->GetFloatField(javaPassObj, durationFieldID);

    // Get the ordinal value of the enum (the index of the enum constant)
    jclass mouthZones12Class = env->FindClass("com/kolibree/MouthZones12");
    jmethodID ordinalMethodID =
        env->GetMethodID(mouthZones12Class, "ordinal", "()I");
    if (ordinalMethodID == nullptr) {
      // Handle error (method not found)
      std::cerr << "Method 'ordinal' not found in MouthZones12." << std::endl;
      env->DeleteLocalRef(zoneObj);
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      env->DeleteLocalRef(mouthZones12Class);
      continue;
    }
    jint ordinal = env->CallIntMethod(zoneObj, ordinalMethodID);

    // Create a C++ BrushingPass object and add it to the vector
    cppBrushing.passes.push_back(BrushingPass<MouthZones12>{
        timestamp, duration, static_cast<MouthZones12>(ordinal)});

    // Clean up local references
    env->DeleteLocalRef(zoneObj);
    env->DeleteLocalRef(passClass);
    env->DeleteLocalRef(javaPassObj);
    env->DeleteLocalRef(mouthZones12Class);
  }

  // Clean up local references
  env->DeleteLocalRef(passesListObj);
  env->DeleteLocalRef(listClass);
  env->DeleteLocalRef(brushingClass);

  return cppBrushing; // Return the populated C++ Brushing object
}

// Function to convert C++ Checkup<MouthZones12> to Java HashMap<MouthZones12,
// Float>
jobject convertCppCheckupToJava12(JNIEnv *env,
                                  const Checkup<MouthZones12> &cppCheckup) {
  // Create a new Java HashMap
  jclass hashMapClass = env->FindClass("java/util/HashMap");
  jmethodID hashMapConstructorID =
      env->GetMethodID(hashMapClass, "<init>", "()V");
  jobject javaHashMap = env->NewObject(hashMapClass, hashMapConstructorID);

  // Get the 'put' method ID
  jmethodID putMethodID = env->GetMethodID(
      hashMapClass, "put",
      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (putMethodID == nullptr) {
    std::cerr << "Method 'put' not found in HashMap." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the MouthZones12 class
  jclass mouthZones12Class = env->FindClass("com/kolibree/MouthZones12");
  if (mouthZones12Class == nullptr) {
    std::cerr << "Class 'MouthZones12' not found." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the 'valueOf' static method ID for MouthZones12
  jmethodID valueOfMethodID =
      env->GetStaticMethodID(mouthZones12Class, "valueOf",
                             "(Ljava/lang/String;)Lcom/kolibree/MouthZones12;");
  if (valueOfMethodID == nullptr) {
    std::cerr << "Method 'valueOf' not found in MouthZones12." << std::endl;
    env->DeleteLocalRef(mouthZones12Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the Float class and its constructor
  jclass floatClass = env->FindClass("java/lang/Float");
  jmethodID floatConstructorID = env->GetMethodID(floatClass, "<init>", "(F)V");
  if (floatConstructorID == nullptr) {
    std::cerr << "Constructor '<init>(F)V' not found in Float." << std::endl;
    env->DeleteLocalRef(floatClass);
    env->DeleteLocalRef(mouthZones12Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Iterate over each MouthZones12 enum constant
  for (int i = 0; i < 12; ++i) {
    MouthZones12 zone = static_cast<MouthZones12>(i);
    float coverage = cppCheckup.coverageForZone(zone);

    // Convert enum to string
    std::string zoneStr = enumToString(zone);
    jstring zoneName = env->NewStringUTF(zoneStr.c_str());

    // Convert the zone name string to a Java enum instance using 'valueOf'
    // method
    jobject zoneEnumObj = env->CallStaticObjectMethod(
        mouthZones12Class, valueOfMethodID, zoneName);
    env->DeleteLocalRef(zoneName);

    if (zoneEnumObj == nullptr) {
      std::cerr << "Failed to convert string to MouthZones12 enum."
                << std::endl;
      continue;
    }

    // Create a Java Float object for the coverage value
    jobject coverageFloatObj =
        env->NewObject(floatClass, floatConstructorID, coverage);

    // Put the (zoneEnumObj, coverageFloatObj) pair into the HashMap
    env->CallObjectMethod(javaHashMap, putMethodID, zoneEnumObj,
                          coverageFloatObj);

    // Clean up local references
    env->DeleteLocalRef(zoneEnumObj);
    env->DeleteLocalRef(coverageFloatObj);
  }

  // Clean up local references
  env->DeleteLocalRef(hashMapClass);
  env->DeleteLocalRef(mouthZones12Class);
  env->DeleteLocalRef(floatClass);

  return javaHashMap; // Return the populated Java HashMap object
}

// JNI function for 12 zones
extern "C" JNIEXPORT jobject JNICALL
Java_com_kolibree_CheckupComputer_computeCheckup12(JNIEnv *env,
                                                   jobject /* this */,
                                                   jobject javaBrushingObj) {

  // Convert Java Brushing12 to C++ Brushing<MouthZones12>
  Brushing<MouthZones12> cppBrushing =
      convertJavaBrushingToCpp12(env, javaBrushingObj);

  // Compute the checkup using the C++ CheckupComputer
  CheckupComputer checkupComputer; // Ensure this class is properly defined
  Checkup<MouthZones12> cppCheckup =
      checkupComputer.computeCheckup(cppBrushing);

  // Convert the C++ Checkup back to a Java HashMap
  jobject result = convertCppCheckupToJava12(env, cppCheckup);

  return result; // Return the populated Java HashMap object
}

// =====================
// JNI Function for Brushing16
// =====================

// Function to convert Java Brushing16 to C++ Brushing<MouthZones16>
Brushing<MouthZones16> convertJavaBrushingToCpp16(JNIEnv *env,
                                                  jobject javaBrushingObj) {
  Brushing<MouthZones16> cppBrushing; // Create an empty C++ Brushing object

  // Get the class of the Java Brushing16 object
  jclass brushingClass = env->GetObjectClass(javaBrushingObj);

  // Get the field ID for the 'passes' field (which is a List)
  jfieldID passesFieldID =
      env->GetFieldID(brushingClass, "passes", "Ljava/util/List;");
  if (passesFieldID == nullptr) {
    // Handle error (field not found)
    std::cerr << "Field 'passes' not found in Brushing16." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the 'passes' field from the Java object
  jobject passesListObj = env->GetObjectField(javaBrushingObj, passesFieldID);
  if (passesListObj == nullptr) {
    // Handle error (field is null)
    std::cerr << "'passes' field is null in Brushing16." << std::endl;
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the class of the List
  jclass listClass = env->GetObjectClass(passesListObj);

  // Get method IDs for 'size' and 'get' methods of List
  jmethodID sizeMethodID = env->GetMethodID(listClass, "size", "()I");
  jmethodID getMethodID =
      env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
  if (sizeMethodID == nullptr || getMethodID == nullptr) {
    // Handle error (methods not found)
    std::cerr << "Methods 'size' or 'get' not found in List." << std::endl;
    env->DeleteLocalRef(passesListObj);
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(brushingClass);
    return cppBrushing;
  }

  // Get the number of passes in the list
  jint numPasses = env->CallIntMethod(passesListObj, sizeMethodID);

  // Iterate over each pass in the 'passes' list
  for (jint i = 0; i < numPasses; i++) {
    // Get the Pass object at index i
    jobject javaPassObj = env->CallObjectMethod(passesListObj, getMethodID, i);
    if (javaPassObj == nullptr) {
      continue; // Skip null passes
    }

    // Get the class of the Pass object
    jclass passClass = env->GetObjectClass(javaPassObj);

    // Get field IDs for 'zone', 'timestamp', and 'duration' fields
    jfieldID zoneFieldID =
        env->GetFieldID(passClass, "zone", "Lcom/kolibree/MouthZones16;");
    jfieldID timestampFieldID =
        env->GetFieldID(passClass, "timestamp", "F"); // 'F' for float
    jfieldID durationFieldID = env->GetFieldID(passClass, "duration", "F");
    if (zoneFieldID == nullptr || timestampFieldID == nullptr ||
        durationFieldID == nullptr) {
      // Handle error (fields not found)
      std::cerr << "Fields 'zone', 'timestamp', or 'duration' not found in "
                   "BrushingPass16."
                << std::endl;
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      continue;
    }

    // Get the values of the fields from the Pass object
    jobject zoneObj = env->GetObjectField(javaPassObj, zoneFieldID);
    jfloat timestamp = env->GetFloatField(javaPassObj, timestampFieldID);
    jfloat duration = env->GetFloatField(javaPassObj, durationFieldID);

    // Get the ordinal value of the enum (the index of the enum constant)
    jclass mouthZones16Class = env->FindClass("com/kolibree/MouthZones16");
    jmethodID ordinalMethodID =
        env->GetMethodID(mouthZones16Class, "ordinal", "()I");
    if (ordinalMethodID == nullptr) {
      // Handle error (method not found)
      std::cerr << "Method 'ordinal' not found in MouthZones16." << std::endl;
      env->DeleteLocalRef(zoneObj);
      env->DeleteLocalRef(passClass);
      env->DeleteLocalRef(javaPassObj);
      env->DeleteLocalRef(mouthZones16Class);
      continue;
    }
    jint ordinal = env->CallIntMethod(zoneObj, ordinalMethodID);

    // Create a C++ BrushingPass object and add it to the vector
    cppBrushing.passes.push_back(BrushingPass<MouthZones16>{
        timestamp, duration, static_cast<MouthZones16>(ordinal)});

    // Clean up local references
    env->DeleteLocalRef(zoneObj);
    env->DeleteLocalRef(passClass);
    env->DeleteLocalRef(javaPassObj);
    env->DeleteLocalRef(mouthZones16Class);
  }

  // Clean up local references
  env->DeleteLocalRef(passesListObj);
  env->DeleteLocalRef(listClass);
  env->DeleteLocalRef(brushingClass);

  return cppBrushing; // Return the populated C++ Brushing object
}

// Function to convert C++ Checkup<MouthZones16> to Java HashMap<MouthZones16,
// Float>
jobject convertCppCheckupToJava16(JNIEnv *env,
                                  const Checkup<MouthZones16> &cppCheckup) {
  // Create a new Java HashMap
  jclass hashMapClass = env->FindClass("java/util/HashMap");
  jmethodID hashMapConstructorID =
      env->GetMethodID(hashMapClass, "<init>", "()V");
  jobject javaHashMap = env->NewObject(hashMapClass, hashMapConstructorID);

  // Get the 'put' method ID
  jmethodID putMethodID = env->GetMethodID(
      hashMapClass, "put",
      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (putMethodID == nullptr) {
    std::cerr << "Method 'put' not found in HashMap." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the MouthZones16 class
  jclass mouthZones16Class = env->FindClass("com/kolibree/MouthZones16");
  if (mouthZones16Class == nullptr) {
    std::cerr << "Class 'MouthZones16' not found." << std::endl;
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the 'valueOf' static method ID for MouthZones16
  jmethodID valueOfMethodID =
      env->GetStaticMethodID(mouthZones16Class, "valueOf",
                             "(Ljava/lang/String;)Lcom/kolibree/MouthZones16;");
  if (valueOfMethodID == nullptr) {
    std::cerr << "Method 'valueOf' not found in MouthZones16." << std::endl;
    env->DeleteLocalRef(mouthZones16Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Get the Float class and its constructor
  jclass floatClass = env->FindClass("java/lang/Float");
  jmethodID floatConstructorID = env->GetMethodID(floatClass, "<init>", "(F)V");
  if (floatConstructorID == nullptr) {
    std::cerr << "Constructor '<init>(F)V' not found in Float." << std::endl;
    env->DeleteLocalRef(floatClass);
    env->DeleteLocalRef(mouthZones16Class);
    env->DeleteLocalRef(hashMapClass);
    return nullptr;
  }

  // Iterate over each MouthZones16 enum constant
  for (int i = 0; i < 16; ++i) {
    MouthZones16 zone = static_cast<MouthZones16>(i);
    float coverage = cppCheckup.coverageForZone(zone);

    // Convert enum to string
    std::string zoneStr = enumToString(zone);
    jstring zoneName = env->NewStringUTF(zoneStr.c_str());

    // Convert the zone name string to a Java enum instance using 'valueOf'
    // method
    jobject zoneEnumObj = env->CallStaticObjectMethod(
        mouthZones16Class, valueOfMethodID, zoneName);
    env->DeleteLocalRef(zoneName);

    if (zoneEnumObj == nullptr) {
      std::cerr << "Failed to convert string to MouthZones16 enum."
                << std::endl;
      continue;
    }

    // Create a Java Float object for the coverage value
    jobject coverageFloatObj =
        env->NewObject(floatClass, floatConstructorID, coverage);

    // Put the (zoneEnumObj, coverageFloatObj) pair into the HashMap
    env->CallObjectMethod(javaHashMap, putMethodID, zoneEnumObj,
                          coverageFloatObj);

    // Clean up local references
    env->DeleteLocalRef(zoneEnumObj);
    env->DeleteLocalRef(coverageFloatObj);
  }

  // Clean up local references
  env->DeleteLocalRef(hashMapClass);
  env->DeleteLocalRef(mouthZones16Class);
  env->DeleteLocalRef(floatClass);

  return javaHashMap; // Return the populated Java HashMap object
}

// JNI function for 16 zones
extern "C" JNIEXPORT jobject JNICALL
Java_com_kolibree_CheckupComputer_computeCheckup16(JNIEnv *env,
                                                   jobject /* this */,
                                                   jobject javaBrushingObj) {

  // Convert Java Brushing16 to C++ Brushing<MouthZones16>
  Brushing<MouthZones16> cppBrushing =
      convertJavaBrushingToCpp16(env, javaBrushingObj);

  // Compute the checkup using the C++ CheckupComputer
  CheckupComputer checkupComputer; // Ensure this class is properly defined
  Checkup<MouthZones16> cppCheckup =
      checkupComputer.computeCheckup(cppBrushing);

  // Convert the C++ Checkup back to a Java HashMap
  jobject result = convertCppCheckupToJava16(env, cppCheckup);

  return result; // Return the populated Java HashMap object
}
