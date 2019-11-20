# Video SDK for Cocos2d Js 使用指南

## 适用范围

本规范仅适用于使用游密实时音视频引擎Cocos2d-js接口开发多人实时语音功能的开发者。

## SDK目录概述

* **音视频引擎SDK**
引擎SDK中有两个子文件夹：lib、include,下面依次介绍下这两个子文件夹。

 1. `include`：SDK的头文件。
详细介绍下inlude，所有接口都在这个文件夹中。
     * `IYouMeVoiceEngine.h`封装了语音SDK的全部功能接口，集成方可通过IYouMeVoiceEngine::getInstance ()->…来调用语音SDK接口。
     * `IYouMeEventCallback.h`包含语音SDK的所有回调事件接口，例如初始化结果，频道加入成功等，都将通过此接口通知集成方
     * `IYouMeVideoCallback.h`包含语音SDK的视频相关回调事件接口
     * `YouMeConstDefine.h`包含语音SDK的所有枚举类型定义，如错误码等。
 2. `lib`：库文件，分为Android平台和iOS平台。Android平台下包括ARMv5、ARMv7和X86三种CPU架构下的libyoume_voice_engine.so文件，还包括youme_voice_engine.jar。iOS平台下包含libyoume_voice_engine.a文件。

* **js相关SDK**
此SDK是在引擎SDK基础上封装的为cocos-js使用的SDK，包括一部分c++文件和js文件。
 1. `Classes`：对应cocos-js里的frameworks\runtime-src\Classes目录，包括一些封装过的c++文件以及c++转js的文件
 2. `src`：对应cocos-js里的src目录，里面的YoumeSDK.js/constdef.js就是提供给游戏使用的js文件

## 开发环境集成
* **引擎SDK集成**
Cocos2d-js开发环境生成的目录结构如下图所示，将引擎SDK目录更名为youme_voice_engine（内含“include”和“lib”两个子文件夹），并复制到cocos-js项目的frameworks\runtime-src目录下，这个目录下包含了Android和iOS两个平台所需的所有C++头文件和库文件。
![](/doc/images/talk_cocos_project_directory.png)

* **js SDK集成**
把js相关sdk的Classes目录放入cocos-js项目的frameworks\runtime-src\Classes目录，将src目录放入cocos-js项目的src目录，并加入项目。

* **js注册**
修改cocos-js原有项目中的frameworks\runtime-src\Classes\AppDelegate.cpp文件：
 1. 增加：`#include "jsb/jsb_youmetalk.hpp"`
 2. 在`AppDelegate::applicationDidFinishLaunching()`方法内的`sc->start();`之前，加上：`sc->addRegisterCallback(register_all_youmetalk);`


### Android系统Android-Studio开发环境配置

1. 修改proj.android-studio/app/jni/Android.mk文件，对应位置增加指定内容，分别对游密实时语音SDK的动态库进行预编译处理、添加头文件路径、链接动态库。

``` Shell

    LOCAL_PATH := $(call my-dir)
    include $(CLEAR_VARS)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

    #＝＝＝＝＝＝Youme 添加＝＝＝＝＝＝＝＝＝
    LOCAL_MODULE := youme_voice_engine
    LOCAL_SRC_FILES := ../$(LOCAL_PATH)/../../../../youme_voice_engine/lib/android/$(TARGET_ARCH_ABI)/libyoume_voice_engine.so
    include $(PREBUILT_SHARED_LIBRARY)
    #＝＝＝＝＝＝结束 Youme 添加＝＝＝＝＝＝＝

    LOCAL_MODULE := cocos2dcpp_shared
    LOCAL_MODULE_FILENAME := libcocos2dcpp
    LOCAL_SRC_FILES := hellojavascript/main.cpp \
                       ../../../Classes/AppDelegate.cpp \
    #＝＝＝＝＝＝==Youme修改,不要拷贝这一行＝＝＝＝＝＝＝＝＝
                       ../../../Classes/YMRenderTexture.cpp \
                       ../../../Classes/YoumeTalk.cpp \
                       ../../../Classes/jsb/jsb_youmetalk.cpp
    #＝＝＝＝＝＝结束 Youme 修改＝＝＝＝＝＝＝

    #＝＝＝＝＝＝==Youme修改＝＝＝＝＝＝＝＝＝
    LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                        $(LOCAL_PATH)/../../../../youme_voice_engine/include
    #＝＝＝＝＝＝结束 Youme 修改＝＝＝＝＝＝＝

    # _COCOS_HEADER_ANDROID_BEGIN
    # _COCOS_HEADER_ANDROID_END
    LOCAL_STATIC_LIBRARIES := cocos2dx_static

    #＝＝＝＝＝＝Youme 添加＝＝＝＝＝＝＝＝＝
    LOCAL_SHARED_LIBRARIES := youme_voice_engine
    #＝＝＝＝＝＝结束 Youme 添加＝＝＝＝＝＝＝

    # _COCOS_LIB_ANDROID_BEGIN
    # _COCOS_LIB_ANDROID_END
    include $(BUILD_SHARED_LIBRARY)
    $(call import-module,.)
    # _COCOS_LIB_IMPORT_ANDROID_BEGIN
    # _COCOS_LIB_IMPORT_ANDROID_END

```

2. 如果需要显示指定CPU架构则修改proj.android-studio/app/jni/Application.mk文件，增加指定部分的内容(v5版本为APP_ABI := armeabi)；如果不需要指定CPU架构Application.mk文件, 则不用修改。

``` Shell
  #＝＝＝＝＝＝修改＝＝＝＝＝＝＝＝＝＝＝
  APP_ABI := armeabi-v7a
  #＝＝＝＝＝＝结束修改＝＝＝＝＝＝＝＝＝

  APP_STL := gnustl_static
  APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
  APP_LDFLAGS := -latomic
  ifeq ($(NDK_DEBUG),1)
   APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
   APP_OPTIM := debug
  else
   APP_CPPFLAGS += -DNDEBUG
   APP_OPTIM := release
  endif
```

3. 复制youme_voice_engine/lib/android/youme_voice_engine.jar到proj.android-studio/app/libs/youme_voice_engine.jar。

4. 修改proj.android-studio/app/AndroidManifest.xml文件，确保声明了如下的权限：

``` xml
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
    <uses-permission android:name="android.permission.CHANGE_WIFI_STATE" />
    <uses-permission android:name="android.permission.CHANGE_NETWORK_STATE" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.RECORD_AUDIO" />
    <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    <uses-permission android:name="android.permission.BLUETOOTH" />
    <!-- video -->
    <uses-feature android:name="android.hardware.camera" />
    <uses-feature android:name="android.hardware.camera.autofocus" />

    <uses-permission android:name="android.permission.CAMERA" />
    <uses-permission android:name="android.permission.CAPTURE_VIDEO_OUTPUT" />
```

5. 打开Android Studio，导入上一步Android工程，在项目的第一个启动的AppActivity（找到AppActivity.java文件）中导入package:

``` java
import  com.youme.voiceengine.mgr.YouMeManager;
import  com.youme.voiceengine.*;
```

然后在`onCreate`方法里添加如下代码(没有此方法的话需要自己补上)：

``` java
 @Override
    public void onCreate(Bundle savedInstanceState)
    {
        //以下两个函数调用顺序不能错
        YouMeManager.Init(this);
        super.onCreate(savedInstanceState);

    }

```

### iOS系统XCode开发环境配置

* **音视频引擎配置**
添加头文件和依赖库:
1. 添加头文件路径：在`Build Settings` -> `Search Paths` -> `Header Search Paths`中添加:
    `../../../youme_voice_engine/include`  （建议直接将此include文件夹拖到xcode需要填入的位置，然后路径会自动生成)，
    `../../cocos2d-x/cocos/scripting/js-bindings/manual`;
2. 添加库文件路径：在`Build Settings` -> `Search Paths` -> `Library Search Paths`中添加`../../../youme_voice_engine/lib/ios/Release-universal` （建议直接将此Release-universal文件夹拖到xcode需要填入的位置，然后路径会自动生成);
* **js SDK配置**
参见[js SDK集成](##开发环境集成)
3. 添加依赖库：在`Build Phases`  -> `Link Binary With Libraries`下添加：`libsqlite3.0.tbd`、`libyoume_voice_engine.a`、`libz.dylib`、`libz.1.2.5.tbd`、`libresolv.9.tbd`、`SystemConfiguration.framework`、`CoreTelephony.framework`、`AVFoundation.framework`、`AudioToolBox.framework`、`CFNetwork.framework`。

### 备注：
[详细接口介绍可查看“Video SDK for Cocos2d_js-API手册.md”文档](https://github.com/youmesdk/YoumeVideoSDK_JS/blob/master/Video%20SDK%20for%20Cocos2d_js-API%E6%89%8B%E5%86%8C.md)
