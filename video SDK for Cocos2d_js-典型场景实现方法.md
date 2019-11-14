# Video SDK for Cocos2d Js 典型场景实现方法

## 实现视频通话 ##

### 相关接口

`init` 引擎初始化
`setVideoLocalResolution` 设置本地采集分辨率
`setVideoNetResolution` 设置网络传输分辨率
`joinChannelSingleMode` 加入房间
`createRender` 创建渲染
`startCapture` 开始摄像头采集
`setMicrophoneMute`	设置麦克风状态
`setSpeakerMute`	设置扬声器状态

### 1.初始化（init）设置本地采集及网络传输分辨率（也可采用默认设置）

### 2.加入房间（）。打开摄像头（startCapture），麦克风扬声器等设备（setMicrophoneMute，setSpeakerMute）

### 3.接收到视频数据回调后，创建渲染（createrender）


## 实现视频直播 ##

###相关接口

`init` 引擎初始化
`setVideoLocalResolution` 设置本地采集分辨率
`setVideoNetResolution` 设置网络传输分辨率
`joinChannelSingleMode` 加入房间
`createRender` 创建渲染
`startCapture` 开始摄像头采集
`setMicrophoneMute`	设置麦克风状态
`setSpeakerMute`	设置扬声器状态
`setHeadsetMonitorOn` 设置监听
`setBackgroundMusicVolume`	设置背景音乐播放音量
`playBackgroundMusic`	播放背景音乐

#### 1.以主播身份进入频道 初始化（init）->joinChannelSingleMode(参数三传主播身份YOUME_USER_HOST), 观众传的身份可以是听众 也可以是自由人（住进自由人进入频道 需要关闭麦克风）

#### 2.主播打开摄像头，麦克风等设备

#### 3.设置监听 setHeadsetMonitorOn（true,true）参数1表示是否监听麦克风 true表示监听,false表示不监听 ,参数2表示是否监听背景音乐,true表示监听,false表示不监听

#### 4.setBackgroundMusicVolume（70）调节背景音量大小

#### 5.playBackgroundMusic(string pFilePath, bool bRepeat) 播放本地的mp3音乐。 参数一 本地音乐路径， 参数二 是否重复播放 true重复，false不重复

#### 6.远端有视频流过来，会通知 `YOUME_EVENT_OTHERS_VIDEO_ON` 事件,此时调用createRender创建相关渲染。

