#include "base/ccConfig.h"
#ifndef __cocos2dx_youmetalk_h__
#define __cocos2dx_youmetalk_h__

#include "jsapi.h"
#include "jsfriendapi.h"

extern JSClass  *jsb_YouMeTalk_class;
extern JSObject *jsb_YouMeTalk_prototype;

bool js_cocos2dx_extension_YouMeTalk_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_cocos2dx_extension_YouMeTalk_finalize(JSContext *cx, JSObject *obj);
void js_register_cocos2dx_extension_YouMeTalk(JSContext *cx, JS::HandleObject global);
void register_all_youmetalk(JSContext* cx, JS::HandleObject obj);
bool js_cocos2dx_extension_YouMeTalk_leaveChannelAll(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getVolume(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_requestRestApi(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setSpeakerMute(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setMicrophoneMute(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_pauseChannel(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_createRender(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_deleteRender(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_resumeChannel(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_maskVideoByUserId(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setReverbEnabled(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getSDKVersion(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setServerRegion(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_switchCamera(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_resetCamera(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_speakToChannel(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_startCapture(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getSpeakerMute(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_cleanTextures(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setGrabMicOption(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setInviteMicOption(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_init(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_stopInviteMic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_responseInviteMic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setMicLevelCallback(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getChannelUserList(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setVolume(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_unbindTexture(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_openVideoEncoder(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_requestGrabMic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_playBackgroundMusic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_startGrabMicAction(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_requestInviteMic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setCaptureProperty(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setListenOtherVoice(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_stopGrabMicAction(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setToken(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setAutoSendStatus(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getMicrophoneMute(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_unInit(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_releaseGrabMic(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_bindTexture(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_updateTextures(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_stopCapture(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_destroy(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_getInstance(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_releaseMicSync(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_resumeMicSync(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_openBeautify(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_beautifyChanged(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_extension_YouMeTalk_stretchFace(JSContext *cx, uint32_t argc, jsval *vp);

#endif // __cocos2dx_youmetalk_h__
