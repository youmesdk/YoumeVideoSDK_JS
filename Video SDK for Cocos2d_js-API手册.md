# Video SDK for Cocos2d Js 接口手册


### 相关异步/同步处理方法介绍
游密语音引擎SDK提供的全部为Js接口,接口调用都会立即返回,凡是本身需要较长耗时的接口调用都会采用异步回调的方式,所有接口都可以在主线程中直接使用。回调在主线程。


### API调用说明
使用YoumeSDK.js的话，API的调用可使用“ym.YouMe().”来直接操作。
接口使用的基本流程为：`初始化`->`收到初始化成功回调通知`->`加入语音频道`->`收到加入频道成功回调通知`->`使用其它接口`->`离开语音频道`->`反初始化`，要确保严格按照上述的顺序使用接口。


### 实现回调

YoumeSDK.js内已经内置了OnMemberChange和OnEvent的回调，用户可以根据自身的需求对该部分进行修改，回调介绍如下：

*  **频道内用户列表的回调**
```
ym.YouMe().OnMemberChange = function (channel,jsretArr,bUpdate){}
```

* *功能*
设置了频道内成员通知的标识后，有成员变更就会收到此回调

* *参数说明*
`channel`： 频道ID
`jsretArr`：频道内成员列表
`bUpdate`：是否是成员变更的标识


* **通用事件回调**
```
ym.YouMe().OnEvent = function( event,  errorcode, channel, param ){}
```
* *功能*
绝大部分异步函数调用后的回调，根据```event```参数来区分是什么回调

* *参数说明*
```event```：当前是什么事件的回调，具体参见[YouMeEvent类型定义](#YouMeEvent类型定义)。
```errorcode```：当前事件的错误码，具体参见[YouMeErrorCode类型定义](#youmeerrorcode类型定义)。
```channel```：频道ID
```param```：当前事件的参数


### 初始化
* **语法**

```
init( strAppKey, strAPPSecret, serverRegionId, strExtServerRegionName );
```

* **功能**
初始化语音引擎，做APP验证和资源初始化。

* **参数说明**
`strAPPKey`：string，从游密申请到的 app key, 这个你们应用程序的唯一标识。
`strAPPSecret`：string， 对应 strAPPKey 的私钥, 这个需要妥善保存，不要暴露给其他人。
`serverRegionId`：int，设置首选连接服务器的区域码，参见状态码的YOUME_RTC_SERVER_REGION定义。如果在初始化时不能确定区域，可以填10001，后面确定时通过 talk_SetServerRegion 设置。如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: （0） - 表明初始化成功
// event: （1） - 表明初始化失败，最常见的失败原因是网络错误或者 AppKey-AppSecret 错误

```

### 加入语音频道（单频道）

* **语法**

```
joinChannelSingleMode ( strUserID, strChannelID, roleType);
```
* **功能**
加入语音频道（单频道模式，每个时刻只能在一个语音频道里面）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string, 全局唯一的频道标识，全局指在当前应用程序的范围内。
`roleType`：int,用户在语音频道里面的角色，见YouMeUserRole定义。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//涉及到的主要回调事件有：
//OnEvent
//event:2 - 成功进入语音频道
//event:3 - 进入语音频道失败，可能原因是网络或服务器有问题
```

### 加入语音频道（多频道）

* **语法**

```
joinChannelMultiMode ( strUserID,strChannelID, roleType);
```
* **功能**
加入语音频道（多频道模式，可以同时听多个语音频道的内容，但每个时刻只能对着一个频道讲话）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。
`roleType`：int,用户在语音频道里面的角色，见YouMeUserRole定义。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: 2 - 成功进入语音频道
// event: 3 - 进入语音频道失败，可能原因是网络或服务器有问题
```

###指定讲话频道

* **语法**

```
speakToChannel ( strChannelID);
```
* **功能**
多频道模式下，指定当前要讲话的频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 8 - 成功切入到指定语音频道
// event: 9 - 切入指定语音频道失败，可能原因是网络或服务器有问题

```


### 退出指定的语音频道
* **语法**

```
leaveChannelMultiMode (strChannelID);
```
* **功能**
多频道模式下，退出指定的语音频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 4 - 退出指定语音频道完成
```

### 退出所有语音频道

* **语法**

```
leaveChannelAll ();
```
* **功能**
退出所有的语音频道（单频道模式下直接调用此函数离开频道即可）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 5 - 退出所有语音频道完成
```

### 设置扬声器状态

* **语法**

```
setSpeakerMute (mute);
```
* **功能**
打开/关闭扬声器。建议该状态值在加入房间成功后按需再重置一次。

* **参数说明**
`mute`:true——关闭扬声器，false——开启扬声器。


### 获取扬声器状态

* **语法**

```
getSpeakerMute();
```

* **功能**
获取当前扬声器状态。

* **返回值**
true——扬声器关闭，false——扬声器开启。



### 设置麦克风状态

* **语法**

```
setMicrophoneMute (mute);
```

* **功能**
打开／关闭麦克风。建议该状态值在加入房间成功后按需再重置一次。

* **参数说明**
`mute`:true——关闭麦克风，false——开启麦克风。


### 获取麦克风状态
* **语法**

```
getMicrophoneMute ();
```

* **功能**
获取当前麦克风状态。

* **返回值**
true——麦克风关闭，false——麦克风开启。

### 设置是否通知别人麦克风和扬声器的开关

* **语法**

```
setAutoSendStatus( bAutoSend );
```

* **功能**
设置是否通知别人,自己麦克风和扬声器的开关状态

* **参数说明**
`bAutoSend`:true——通知，false——不通知。


* **相关回调**

```
// 涉及到的主要回调事件有(房间里的其他人会收到)：
// OnEvent
// event: 16 - 其他用户麦克风打开
// event: 17 - 其他用户麦克风关闭
// event: 18 - 其他用户扬声器打开
// event: 19 - 其他用户扬声器关闭
```

### 设置音量

* **语法**

```
setVolume (uiVolume);
```

* **功能**
设置当前程序输出音量大小。建议该状态值在加入房间成功后按需再重置一次。

* **参数说明**
`uiVolume`:unsigned int，当前音量大小，范围[0-100]。

### 获取音量

* **语法**

```
getVolume ();
```

* **功能**
获取当前程序输出音量大小。

* **返回值**
unsigned int ，当前音量大小，范围[0-100]。


###设置是否允许使用移动网络

* **语法**

```
setUseMobileNetworkEnabled (bEnabled);
```

* **功能**
设置是否允许使用移动网络。在WIFI和移动网络都可用的情况下会优先使用WIFI，在没有WIFI的情况下，如果设置允许使用移动网络，那么会使用移动网络进行语音通信，否则通信会失败。


* **参数说明**
`bEnabled`:true——允许使用移动网络，false——禁止使用移动网络。

### 获取是否允许使用移动网络

* **语法**

```
getUseMobileNetworkEnabled () ;
```

* **功能**
获取是否允许SDK在没有WIFI的情况使用移动网络进行语音通信。

* **返回值**
true——允许使用移动网络，false——禁止使用移动网络，默认情况下允许使用移动网络。

### 控制他人麦克风

* **语法**

```
setOtherMicMute ( strUserID, mute);
```

* **功能**
控制他人的麦克风状态

* **参数说明**
`strUserID`：string,要控制的用户ID
`mute`：是否静音。true:静音别人的麦克风，false：开启别人的麦克风

* **返回值**
int，如果成功返回0，否则返回错误码，请参考状态码-YouMeErrorCode定义。

* **相关回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 23 - 麦克风被其他用户打开
// event: 24 - 麦克风被其他用户关闭
```

### 控制他人扬声器

* **语法**

```
setOtherSpeakerMute ( strUserID, mute);
```

* **功能**
控制他人的扬声器状态

* **参数说明**
`strUserID`：string,要控制的用户ID
`mute`：是否静音。true:静音别人的扬声器，false：开启别人的扬声器

* **返回值**
int，如果成功返回0，否则返回错误码，请参考状态码-YouMeErrorCode定义。

* **相关回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 25 - 扬声器被其他用户打开
// event: 26 - 扬声器被其他用户关闭
```

### 设置是否听某人的语音

* **语法**

```
setListenOtherVoices (strUserID, on);
```

* **功能**
设置是否听某人的语音。

* **参数说明**
`strUserID`：string,要控制的用户ID。
`on`：true表示开启接收指定用户的语音，false表示屏蔽指定用户的语音。

* **返回值**
int，如果成功返回0，否则返回错误码，请参考状态码-YouMeErrorCode定义。

* **异步回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 27 - 取消屏蔽某人语音
// event: 28 - 屏蔽某人语音
```

### 暂停通话

* **语法**

```
pauseChannel();
```

* **功能**
暂停通话，释放对麦克风等设备资源的占用。当需要用第三方模块临时录音时，可调用这个接口。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 6 - 暂停语音频道完成
```

### 恢复通话

* **语法**

```
resumeChannel();
```

* **功能**
恢复通话，调用PauseChannel暂停通话后，可调用这个接口恢复通话。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 7 - 恢复语音频道完成
```

###设置语音检测

* **语法**

```
setVadCallbackEnabled(bEnabled);
```

* **功能**
设置是否开启语音检测回调。开启后频道内有人正在讲话与结束讲话都会发起相应回调通知。

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考状态码-YouMeErrorCode定义。

### 播放背景音乐

* **语法**

```
playBackgroundMusic ( strFilePath, bRepeat);
```

* **功能**
播放指定的音乐文件。播放的音乐将会通过扬声器输出，并和语音混合后发送给接收方。这个功能适合于主播/指挥等使用。

* **参数说明**
`strFilePath`：音乐文件的路径。
`bRepeat`：是否重复播放，true——重复播放，false——只播放一次就停止播放。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 13 - 通知背景音乐播放结束
// event: 14 - 通知背景音乐播放失败
```

### 停止播放背景音乐
* **语法**

```
stopBackgroundMusic();
```

* **功能**
停止播放当前正在播放的背景音乐。
这是一个同步调用接口，函数返回时，音乐播放也就停止了。

* **返回值**
int，如果成功返回0，表明成功停止了音乐播放流程；否则返回错误码，请参考状态码-YouMeErrorCode定义。

### 设置背景音乐播放音量
* **语法**

```
setBackgroundMusicVolume(vol);
```

* **功能**
设定背景音乐的音量。这个接口用于调整背景音乐和语音之间的相对音量，使得背景音乐和语音混合听起来协调。
这是一个同步调用接口。

* **参数说明**
`vol`:int,背景音乐的音量，范围 [0-100]。

* **返回值**
int，如果成功（表明成功设置了背景音乐的音量）返回0，否则返回错误码，具体请参考状态码-YouMeErrorCode定义。

### 设置监听
* **语法**

```
setHeadsetMonitorOn(micEnabled, bgmEnabled);
```

* **功能**
设置是否用耳机监听自己的声音，当不插耳机或外部输入模式时，这个设置不起作用
这是一个同步调用接口。

* **参数说明**
`micEnabled`:是否监听麦克风 true 监听，false 不监听。
`bgmEnabled`:是否监听背景音乐 true 监听，false 不监听。

* **返回值**
int，如果成功则返回0，其他返回值请参考状态码-YouMeErrorCode定义。

### 设置混响音效

* **语法**

```
setReverbEnabled( bEnabled);
```

* **功能**
设置是否开启混响音效，这个主要对主播/指挥有用。

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考状态码-YouMeErrorCode定义。

### 设置录音时间戳

* **语法**

```
setRecordingTimeMs( timeMs);
```

* **功能**
设置当前录音的时间戳。当通过录游戏脚本进行直播时，要保证观众端音画同步，在主播端需要进行时间对齐。
这个接口设置的就是当前游戏画面录制已经进行到哪个时间点了。

* **参数说明**
`timeMs`:unsigned int, 当前游戏画面对应的时间点，单位为毫秒。

* **返回值**
无。

### 设置播放时间戳

* **语法**

```
setPlayingTimeMs( timeMs);
```

* **功能**
设置当前声音播放的时间戳。当通过录游戏脚本进行直播时，要保证观众端音画同步，游戏画面的播放需要和声音播放进行时间对齐。
这个接口设置的就是当前游戏画面播放已经进行到哪个时间点了。

* **参数说明**
`timeMs`:unsigned int,当前游戏画面播放对应的时间点，单位为毫秒。

* **返回值**
无。


### 设置服务器区域

* **语法**

```
setServerRegion(serverRegionId,strExtRegionName);
```

* **功能**
设置首选连接服务器的区域码.

* **参数说明**
`serverRegionId`：int,如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为 10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。


### RestApi——支持主播相关信息查询
* **语法**

```
requestRestApi( strCommand , strQueryBody  );
```
* **功能**
Rest API , 向服务器请求额外数据。支持主播信息，主播排班等功能查询。详情参看文档<RequestRestAPI接口说明>


* **参数说明**
`strCommand`：请求的命令字符串，标识命令类型。
`strQueryBody`：请求需要的参数,json格式。


* **返回值**
int，小于0，表示错误码，大于0，表示本次查询的requestID。

* **异步回调**

```
//requestID:int,回传ID
//iErrorCode:int,错误码
//strQuery:string,回传查询请求，json格式，包含command（回传strCommand参数）和query（回传strQueryBody参数）字段。
//strResult:string,查询结果，json格式。
onRequestRestAPI( requestID, iErrorCode, strQuery, strResult )
```

### 安全验证码设置

* **语法**

```
setToken( strToken );
```

* **功能**
设置身份验证的token，需要配合后台接口。

* **参数说明**
`strToken`：string,身份验证用token，设置空字符串，清空token值，不进行身份验证。

### 查询频道用户列表

* **语法**

```
getChannelUserList( strChannelID,maxCount, notifyMemChange );
```

* **功能**
查询频道当前的用户列表， 并设置是否获取频道用户进出的通知。（必须自己在频道中）

* **参数说明**
`strChannelID`：string,频道ID。
`maxCount`：int,想要获取的最大人数。-1表示获取全部列表。
`notifyMemChange`：当有人进出频道时，是否获得通知。true，需要通知，false,不需要通知。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考状态码-YouMeErrorCode定义。

* **异步回调**

```
//strChannel:string,频道ID
//memchanges:对象数组，查询获得的用户列表，或变更列表。
//每一个数组的数据，包含userID和isJoin两个字段
onMemberChange(  strChannel,  memchanges, bUpdate )
```

## 视频相关接口
视频的频道属性和语音是绑定的，可以单独控制是否开启/关闭音视频流。**以下接口的调用，必须是在进入频道之后。**

###  绑定视频纹理

* **语法**

```
int bindTexture(userId, width, height, texture);
```

* **功能**
根据用户ID，绑定本地的RenderTexture（此函数一般在首次收到该用户视频开启事件后调用）

* **参数说明**
`userId`：用户ID
`width`：渲染的初始宽度
`height`：渲染的初始高度
`texture`：cc.RenderTexture，最终视频会渲染到的纹理对象（调用此函数时需创建好，不能为空）

* **返回值**
大于等于0时，为渲染ID，说明绑定成功；小于0则为错误码，具体的错误码请参考[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。

###  解绑视频纹理

* **语法**

```
void unbindTexture(userId);
```

* **功能**
将之前绑定过的用户id解绑（此函数一般在收到该用户视频关闭事件后调用）

* **参数说明**
`userId`：用户ID

###  更新所有视频纹理

* **语法**

```
void updateTextures();
```

* **功能**
更新所有视频纹理数据（在游戏update的时候调用）

###  清除所有视频纹理

* **语法**

```
void cleanTextures();
```

* **功能**
清除所有视频纹理数据（在游戏关闭的时候调用）

###  开始捕获本机摄像头数据

* **语法**

```
int startCapture();
```

* **功能**
捕获本机摄像头数据，以便发送给房间内其他人

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。

###  停止捕获本机摄像头数据

* **语法**

```
int stopCapture();
```

* **功能**
停止捕获本机摄像头数据（比如退出房间、程序切换到后台时）

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。

###  切换前后置摄像头

* **语法**

```
int switchCamera();
```

* **功能**
切换前后置摄像头（默认使用的是前置摄像头）

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。


###  重置摄像头

* **语法**

```
YouMeErrorCode resetCamera();
```

* **功能**
权限检测结束后重置摄像头

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。


###  屏蔽/恢复他人视频

* **语法**

```
int maskVideoByUserId(userId, mask);
```

* **功能**
屏蔽他人视频（屏蔽后恢复他人视频也是调用此函数）

* **参数说明**
`userId`：要屏蔽的用户ID
`mask`:true是要屏蔽，false是要取消屏蔽

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。

###  设置本地视频的属性

* **语法**

```
YouMeErrorCode setCaptureProperty(int nFps, int width, int height);
```

* **功能**
设置本地视频数据捕获最终的分辨率和帧率

* **参数说明**
`nFps`:帧率（1-30之间，默认15）
`width`:视频宽度
`height`:视频高度

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。

###  设置视频网络传输过程的分辨率

* **语法**

```
YouMeErrorCode setNetCaptureProperty(int width, int height);
```

* **功能**
设置视频网络传输过程的分辨率

* **参数说明**
`width`:视频宽度
`height`:视频高度

* **返回值**
如果成功则返回0，其它具体参见[YouMeErrorCode类型定义](/doc/TalkStatusCode.html#youmeerrorcode类型定义)。


###  视频接入流程说明（以一个房间2个人为例）
* 1.创建2个空的RenderTexture（指定好宽高以及PixelFormat）
* 2.走正常加入语音频道流程
* 3.在OnEvent回调里面根据不同的事件进行处理：
	* 2 //加入频道成功
		* 调用startCapture开启摄像头捕获
		* 调用bindTexture将当前用户ID和一个RenderTexture绑定起来
	* 5 //离开频道成功
		* 调用stopCapture停止摄像头捕获
		* 调用unbindTexture将当前用户ID与RenderTexture解绑
	* 200 //其他用户视频流打开
		* 调用bindTexture将其他用户ID和另一个RenderTexture绑定起来
	* 208 //其他用户视频关闭
		* 调用unbindTexture将其他用户ID与RenderTexture解绑
* 4.在Layer/Scene->Update的时候，调用updateTextures即可显示最新视频图像
* 5.在关闭程序或者退出频道后，cleanTextures即可清除所有数据


### 反初始化

* **语法**

```
unInit ();
```

* **功能**
反初始化引擎，可在退出游戏时调用，以释放SDK所有资源。

* **返回值**
int，如果成功则返回0，其他返回值请参考状态码-YouMeErrorCode定义。


