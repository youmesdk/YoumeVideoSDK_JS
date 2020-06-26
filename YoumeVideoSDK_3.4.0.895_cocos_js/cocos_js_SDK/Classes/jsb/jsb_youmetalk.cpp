#include "jsb_youmetalk.hpp"
#include "cocos2d_specifics.hpp"
#include "YouMeTalk.h"

class CYouMeTalkJsWrapper;
CYouMeTalkJsWrapper* g_SingleInstance;

class CYouMeTalkJsWrapper :
	public IYouMeEventCallback, 
	public IRestApiCallback, 
	public IYouMeMemberChangeCallback, 
	public IYouMeChannelMsgCallback
{
public:

	CYouMeTalkJsWrapper()
	{
		JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
		_JSDelegate.construct(cx);
	}

public:
	mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;


	void onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param) override;
	void onRequestRestAPI( int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*  strResult ) override;
	void onMemberChange( const char* channel, std::list<MemberChange>& listMemberChange, bool bUpdate ) override;
	void onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent) override;
};

void CYouMeTalkJsWrapper::onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param)
{
	std::string strChannel = channel;
	std::string strParam = param;
	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
			JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
		jsval params[4];
		params[0] = int32_to_jsval(cx, event);
		params[1] = int32_to_jsval(cx, error);
		params[2] = std_string_to_jsval(cx, strChannel);
		params[3] = std_string_to_jsval(cx, strParam);

		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(this->_JSDelegate.ref()), "OnEvent", 4, params);

	});
}

void CYouMeTalkJsWrapper::onRequestRestAPI( int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*  strResult ){
    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        jsval params[4];
        params[0] = int32_to_jsval(cx, requestID);
        params[1] = int32_to_jsval(cx, iErrorCode);
        params[2] = std_string_to_jsval( cx, strQuery );
        params[3] = std_string_to_jsval( cx, strResult );
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnRequestRestApi", 4, params);
        
    });
}

jsval memberChange_to_jsval(JSContext* cx, const std::string& userID ){
    JS::RootedObject proto(cx);
    JS::RootedObject parent(cx);
    
    JS::RootedObject tmp(cx, JS_NewObject(cx, NULL, proto, parent));
    if (!tmp) return JSVAL_NULL;
    
    JS::RootedValue uid(cx);
    uid.set(std_string_to_jsval(cx, userID));

    bool ok = JS_DefineProperty(cx, tmp, "UserID", uid, JSPROP_ENUMERATE | JSPROP_PERMANENT);
   
    if (ok) {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

void CYouMeTalkJsWrapper::onMemberChange( const char* channel, std::list<MemberChange>& listMemberChange, bool bUpdate ){
    std::list<std::string> listChange;
	for( auto it = listMemberChange.begin(); it != listMemberChange.end(); ++it )
        {
            if(!(*it).isJoin)
			{
				listChange.push_back((*it).userID);
				CCLOG("wanran BEFORE %s" , (*it).userID);
			}
        }
	
    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        jsval params[3];
        
        params[0] = std_string_to_jsval( cx, channel );
//        params[1] = int32_to_jsval(cx,  listMemberChange.size() );
        JS::RootedObject jsretArr(cx, JS_NewArrayObject(cx, 0));
        
        int i = 0;
        for( auto it = listChange.begin(); it != listChange.end(); ++it )
        {
            JS::RootedValue arrElement(cx);
            arrElement = memberChange_to_jsval(cx,  *it );
            if (!JS_SetElement(cx, jsretArr, i, arrElement)) {
                break;
            }
            
            i++;
        }
        
        params[1] = OBJECT_TO_JSVAL(jsretArr);
        params[2] = BOOLEAN_TO_JSVAL(bUpdate);

        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnMemberChange", 3, params);
        
    });

}

void CYouMeTalkJsWrapper::onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent)
{
	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
			JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
		jsval params[5];
		params[0] = int32_to_jsval(cx, bc);
		params[1] = std_string_to_jsval(cx, channel);
		params[2] = std_string_to_jsval(cx, param1);
		params[3] = std_string_to_jsval(cx, param2);
		params[4] = std_string_to_jsval(cx, strContent);
		
		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(this->_JSDelegate.ref()), "OnBroadcast", 5, params);

	});	
}

template<class T>
static bool dummy_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS_ReportError(cx, "Constructor for the requested class is not available, please refer to the API reference.");
    return false;
}

static bool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
    return false;
}

static bool js_is_native_obj(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    args.rval().setBoolean(true);
    return true;    
}
JSClass  *jsb_YouMeTalk_class;
JSObject *jsb_YouMeTalk_prototype;

bool js_cocos2dx_extension_YouMeTalk_leaveChannelAll(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelAll : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->leaveChannelAll();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_leaveChannelAll : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getVolume(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getVolume : Invalid Native Object");
    if (argc == 0) {
        unsigned int ret = cobj->getVolume();
        jsval jsret = JSVAL_NULL;
        jsret = uint32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getVolume : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        bool arg1;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        arg1 = JS::ToBoolean(args.get(1));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : Error processing arguments");
        int ret = cobj->avoidOtherVoiceStatus(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_requestRestApi(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Invalid Native Object");
    if (argc == 3) {
        std::string arg0;
        std::string arg1;
        int* arg2 = 0;
        ok &= jsval_to_std_string(cx, args.get(0), &arg0);
        ok &= jsval_to_std_string(cx, args.get(1), &arg1);
        #pragma warning NO CONVERSION TO NATIVE FOR int*
		ok = false;
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Error processing arguments");
        int ret = cobj->requestRestApi(arg0.c_str(), arg1.c_str(), arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_requestRestApi : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setSpeakerMute(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setSpeakerMute : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setSpeakerMute : Error processing arguments");
        cobj->setSpeakerMute(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setSpeakerMute : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setMicrophoneMute(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setMicrophoneMute : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setMicrophoneMute : Error processing arguments");
        cobj->setMicrophoneMute(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setMicrophoneMute : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_pauseChannel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_pauseChannel : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->pauseChannel();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_pauseChannel : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_createRender(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_createRender : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_createRender : Error processing arguments");
        int ret = cobj->createRender(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_createRender : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_deleteRender(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_deleteRender : Invalid Native Object");
    if (argc == 1) {
		int arg0 = 0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_deleteRender : Error processing arguments");
        int ret = cobj->deleteRender(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_deleteRender : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_resumeChannel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_resumeChannel : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->resumeChannel();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_resumeChannel : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_maskVideoByUserId(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_maskVideoByUserId : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_maskVideoByUserId : Error processing arguments");
        int ret = cobj->maskVideoByUserId(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_maskVideoByUserId : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : Error processing arguments");
        int ret = cobj->setVadCallbackEnabled(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setReverbEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : Error processing arguments");
        int ret = cobj->setReverbEnabled(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getSDKVersion(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getSDKVersion : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->getSDKVersion();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getSDKVersion : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : Invalid Native Object");
    if (argc == 1) {
        int arg0 = 0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : Error processing arguments");
        int ret = cobj->setBackgroundMusicVolume(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : Error processing arguments");
        int ret = cobj->setHeadsetMonitorOn(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setServerRegion(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setServerRegion : Invalid Native Object");
    if (argc == 3) {
        int arg0 = 0;
        const char* arg1 = nullptr;
        bool arg2;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        arg2 = JS::ToBoolean(args.get(2));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setServerRegion : Error processing arguments");
        cobj->setServerRegion(arg0, arg1, arg2);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setServerRegion : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_switchCamera(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_switchCamera : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->switchCamera();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_switchCamera : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_resetCamera(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_resetCamera : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->resetCamera();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_resetCamera : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_speakToChannel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_speakToChannel : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_speakToChannel : Error processing arguments");
        int ret = cobj->speakToChannel(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_speakToChannel : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_startCapture(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_startCapture : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->startCapture();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_startCapture : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getSpeakerMute(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getSpeakerMute : Invalid Native Object");
    if (argc == 0) {
        bool ret = cobj->getSpeakerMute();
        jsval jsret = JSVAL_NULL;
        jsret = BOOLEAN_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getSpeakerMute : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_cleanTextures(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_cleanTextures : Invalid Native Object");
    if (argc == 0) {
        cobj->cleanTextures();
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_cleanTextures : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setGrabMicOption(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Invalid Native Object");
    if (argc == 5) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        int arg2 = 0;
        int arg3 = 0;
        unsigned int arg4 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        ok &= jsval_to_int32(cx, args.get(3), (int32_t *)&arg3);
        ok &= jsval_to_uint32(cx, args.get(4), &arg4);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Error processing arguments");
        int ret = cobj->setGrabMicOption(arg0, arg1, arg2, arg3, arg4);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : wrong number of arguments: %d, was expecting %d", argc, 5);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setInviteMicOption(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        int arg2 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Error processing arguments");
        int ret = cobj->setInviteMicOption(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->stopBackgroundMusic();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled : Error processing arguments");
        cobj->setUseMobileNetworkEnabled(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_init(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_init : Invalid Native Object");
    if (argc == 4) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        int arg2 = 0;
        const char* arg3 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        std::string arg3_tmp; ok &= jsval_to_std_string(cx, args.get(3), &arg3_tmp); arg3 = arg3_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_init : Error processing arguments");
        int ret = cobj->init(arg0, arg1, arg2, arg3);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_init : wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_stopInviteMic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->stopInviteMic();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_responseInviteMic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        bool arg1;
        const char* arg2 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        arg1 = JS::ToBoolean(args.get(1));
        std::string arg2_tmp; ok &= jsval_to_std_string(cx, args.get(2), &arg2_tmp); arg2 = arg2_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Error processing arguments");
        int ret = cobj->responseInviteMic(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setMicLevelCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : Invalid Native Object");
    if (argc == 1) {
        int arg0 = 0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : Error processing arguments");
        int ret = cobj->setMicLevelCallback(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getChannelUserList(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        bool arg2;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        arg2 = JS::ToBoolean(args.get(2));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : Error processing arguments");
        int ret = cobj->getChannelUserList(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setVolume(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setVolume : Invalid Native Object");
    if (argc == 1) {
        unsigned int arg0 = 0;
        ok &= jsval_to_uint32(cx, args.get(0), &arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setVolume : Error processing arguments");
        cobj->setVolume(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setVolume : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_unbindTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_unbindTexture : Invalid Native Object");
    if (argc == 1) {
        std::string arg0;
        ok &= jsval_to_std_string(cx, args.get(0), &arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_unbindTexture : Error processing arguments");
        cobj->unbindTexture(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_unbindTexture : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_openVideoEncoder(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_openVideoEncoder : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_openVideoEncoder : Error processing arguments");
        int ret = cobj->openVideoEncoder(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_openVideoEncoder : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_requestGrabMic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Invalid Native Object");
    if (argc == 4) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        bool arg2;
        const char* arg3 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        arg2 = JS::ToBoolean(args.get(2));
        std::string arg3_tmp; ok &= jsval_to_std_string(cx, args.get(3), &arg3_tmp); arg3 = arg3_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Error processing arguments");
        int ret = cobj->requestGrabMic(arg0, arg1, arg2, arg3);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : Error processing arguments");
        int ret = cobj->leaveChannelMultiMode(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : Error processing arguments");
        int ret = cobj->setReleaseMicWhenMute(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_playBackgroundMusic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        bool arg1;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        arg1 = JS::ToBoolean(args.get(1));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : Error processing arguments");
        int ret = cobj->playBackgroundMusic(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_startGrabMicAction(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Error processing arguments");
        int ret = cobj->startGrabMicAction(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled : Invalid Native Object");
    if (argc == 0) {
        bool ret = cobj->getUseMobileNetworkEnabled();
        jsval jsret = JSVAL_NULL;
        jsret = BOOLEAN_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_requestInviteMic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        const char* arg2 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        std::string arg2_tmp; ok &= jsval_to_std_string(cx, args.get(2), &arg2_tmp); arg2 = arg2_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Error processing arguments");
        int ret = cobj->requestInviteMic(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setCaptureProperty(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setCaptureProperty : Invalid Native Object");
    if (argc == 3) {
        int arg0 = 0;
        int arg1 = 0;
        int arg2 = 0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setCaptureProperty : Error processing arguments");
        int ret = cobj->setCaptureProperty(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setCaptureProperty : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty : Invalid Native Object");
    if (argc == 2) {
        int arg0 = 0;
        int arg1 = 0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);

        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty : Error processing arguments");
        int ret = cobj->setNetCaptureProperty(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setListenOtherVoice(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        bool arg1;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        arg1 = JS::ToBoolean(args.get(1));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : Error processing arguments");
        int ret = cobj->setListenOtherVoice(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_stopGrabMicAction(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Error processing arguments");
        int ret = cobj->stopGrabMicAction(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setToken(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setToken : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setToken : Error processing arguments");
        cobj->setToken(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setToken : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setAutoSendStatus(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setAutoSendStatus : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setAutoSendStatus : Error processing arguments");
        cobj->setAutoSendStatus(arg0);
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setAutoSendStatus : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        int arg2 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Error processing arguments");
        int ret = cobj->joinChannelMultiMode(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_getMicrophoneMute(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_getMicrophoneMute : Invalid Native Object");
    if (argc == 0) {
        bool ret = cobj->getMicrophoneMute();
        jsval jsret = JSVAL_NULL;
        jsret = BOOLEAN_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getMicrophoneMute : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_unInit(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_unInit : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->unInit();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_unInit : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        int arg2 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : Error processing arguments");
        int ret = cobj->joinChannelSingleMode(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable : Invalid Native Object");
    if (argc == 1) {
        bool arg0;
        arg0 = JS::ToBoolean(args.get(0));
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable : Error processing arguments");
        int ret = cobj->setCaptureFrontCameraEnable(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_releaseGrabMic(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Error processing arguments");
        int ret = cobj->releaseGrabMic(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_bindTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_bindTexture : Invalid Native Object");
    if (argc == 4) {
        std::string arg0;
        int arg1 = 0;
        int arg2 = 0;
        cocos2d::RenderTexture* arg3 = nullptr;
        ok &= jsval_to_std_string(cx, args.get(0), &arg0);
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        do {
            if (args.get(3).isNull()) { arg3 = nullptr; break; }
            if (!args.get(3).isObject()) { ok = false; break; }
            js_proxy_t *jsProxy;
            JS::RootedObject tmpObj(cx, args.get(3).toObjectOrNull());
            jsProxy = jsb_get_js_proxy(tmpObj);
            arg3 = (cocos2d::RenderTexture*)(jsProxy ? jsProxy->ptr : NULL);
            JSB_PRECONDITION2( arg3, cx, false, "Invalid Native Object");
        } while (0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_bindTexture : Error processing arguments");
        int ret = cobj->bindTexture(arg0, arg1, arg2, arg3);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_bindTexture : wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_updateTextures(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_updateTextures : Invalid Native Object");
    if (argc == 0) {
        cobj->updateTextures();
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_updateTextures : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_stopCapture(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopCapture : Invalid Native Object");
    if (argc == 0) {
 
        return true;
    }

    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopCapture : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_destroy(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (argc == 0) {
        YouMeTalk::destroy();
        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_destroy : wrong number of arguments");
    return false;
}

bool js_cocos2dx_extension_YouMeTalk_getInstance(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (argc == 0) {

        YouMeTalk* ret = YouMeTalk::getInstance();
        jsval jsret = JSVAL_NULL;
        if (ret) {
        jsret = OBJECT_TO_JSVAL(js_get_or_create_jsobject<YouMeTalk>(cx, (YouMeTalk*)ret));
    } else {
        jsret = JSVAL_NULL;
    };
        args.rval().set(jsret);
        return true;
    }
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_getInstance : wrong number of arguments");
    return false;
}

bool js_cocos2dx_extension_YouMeTalk_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    YouMeTalk* cobj = YouMeTalk::getInstance();
	if (g_SingleInstance == NULL)
	{
		g_SingleInstance = new CYouMeTalkJsWrapper();
	}
	cobj->SetEventCallback(g_SingleInstance);
	cobj->setRestApiCallback(g_SingleInstance);
	cobj->setMemberChangeCallback(g_SingleInstance);
	cobj->setNotifyCallback(g_SingleInstance);
    js_type_class_t *typeClass = js_get_type_from_native<YouMeTalk>(cobj);

    // link the native object with the javascript object
    JS::RootedObject proto(cx, typeClass->proto.ref());
    JS::RootedObject parent(cx, typeClass->parentProto.ref());
    JS::RootedObject jsobj(cx, JS_NewObject(cx, typeClass->jsclass, proto, parent));
	g_SingleInstance->_JSDelegate.ref() = jsobj;    
	
	js_proxy_t* p = jsb_new_proxy(cobj, jsobj);
    AddNamedObjectRoot(cx, &p->obj, "YouMeTalk");
    args.rval().set(OBJECT_TO_JSVAL(jsobj));
    if (JS_HasProperty(cx, jsobj, "_ctor", &ok) && ok)
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(jsobj), "_ctor", args);
    return true;
}


void js_YouMeTalk_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOGINFO("jsbindings: finalizing JS object %p (YouMeTalk)", obj);
    js_proxy_t* nproxy;
    js_proxy_t* jsproxy;
    JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
    JS::RootedObject jsobj(cx, obj);
    jsproxy = jsb_get_js_proxy(jsobj);
    if (jsproxy) {
        YouMeTalk *nobj = static_cast<YouMeTalk *>(jsproxy->ptr);
        nproxy = jsb_get_native_proxy(jsproxy->ptr);

        if (nobj) {
            jsb_remove_proxy(nproxy, jsproxy);
            nobj->destroy();
        }
        else
            jsb_remove_proxy(nullptr, jsproxy);
    }
}

bool js_cocos2dx_extension_YouMeTalk_releaseMicSync(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_releaseMicSync : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->releaseMicSync();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopCapture : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_resumeMicSync(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_resumeMicSync : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->resumeMicSync();
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopCapture : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_openBeautify(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_openBeautify : Invalid Native Object");
    if (argc == 1) {
        bool arg0 = JS::ToBoolean(args.get(0));
        int ret = cobj->openBeautify(arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_openBeautify : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_cocos2dx_extension_YouMeTalk_beautifyChanged(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_beautifyChanged : Invalid Native Object");
    if (argc == 1) {
        double arg0 = 0.0;
        ok &= JS::ToNumber(cx, args.get(0), &arg0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_beautifyChanged : Error processing arguments");
        int ret = cobj->beautifyChanged((float)arg0);
        jsval jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_beautifyChanged : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}
// bool js_cocos2dx_extension_YouMeTalk_stretchFace(JSContext *cx, uint32_t argc, jsval *vp)
// {
//     JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
//     bool ok = true;
//     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
//     js_proxy_t *proxy = jsb_get_js_proxy(obj);
//     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
//     JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stretchFace : Invalid Native Object");
//     if (argc == 1) {
//         bool arg0 = JS::ToBoolean(args.get(0));
//         JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_stretchFace : Error processing arguments");
//         int ret = cobj->stretchFace(arg0);
//         jsval jsret = JSVAL_NULL;
//         jsret = int32_to_jsval(cx, ret);
//         args.rval().set(jsret);
//         return true;
//     }
    
//     JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stretchFace : wrong number of arguments: %d, was expecting %d", argc, 1);
//     return false;
// }

void js_register_cocos2dx_extension_YouMeTalk(JSContext *cx, JS::HandleObject global) {
    jsb_YouMeTalk_class = (JSClass *)calloc(1, sizeof(JSClass));
    jsb_YouMeTalk_class->name = "YouMeTalk";
    jsb_YouMeTalk_class->addProperty = JS_PropertyStub;
    jsb_YouMeTalk_class->delProperty = JS_DeletePropertyStub;
    jsb_YouMeTalk_class->getProperty = JS_PropertyStub;
    jsb_YouMeTalk_class->setProperty = JS_StrictPropertyStub;
    jsb_YouMeTalk_class->enumerate = JS_EnumerateStub;
    jsb_YouMeTalk_class->resolve = JS_ResolveStub;
    jsb_YouMeTalk_class->convert = JS_ConvertStub;
    jsb_YouMeTalk_class->finalize = js_YouMeTalk_finalize;
    jsb_YouMeTalk_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    static JSPropertySpec properties[] = {
        JS_PSG("__nativeObj", js_is_native_obj, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_PS_END
    };

    static JSFunctionSpec funcs[] = {
        JS_FN("leaveChannelAll", js_cocos2dx_extension_YouMeTalk_leaveChannelAll, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getVolume", js_cocos2dx_extension_YouMeTalk_getVolume, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("avoidOtherVoiceStatus", js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("requestRestApi", js_cocos2dx_extension_YouMeTalk_requestRestApi, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setSpeakerMute", js_cocos2dx_extension_YouMeTalk_setSpeakerMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setMicrophoneMute", js_cocos2dx_extension_YouMeTalk_setMicrophoneMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("pauseChannel", js_cocos2dx_extension_YouMeTalk_pauseChannel, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("createRender", js_cocos2dx_extension_YouMeTalk_createRender, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("deleteRender", js_cocos2dx_extension_YouMeTalk_deleteRender, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),		
        JS_FN("resumeChannel", js_cocos2dx_extension_YouMeTalk_resumeChannel, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("maskVideoByUserId", js_cocos2dx_extension_YouMeTalk_maskVideoByUserId, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setVadCallbackEnabled", js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setReverbEnabled", js_cocos2dx_extension_YouMeTalk_setReverbEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getSDKVersion", js_cocos2dx_extension_YouMeTalk_getSDKVersion, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setBackgroundMusicVolume", js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setHeadsetMonitorOn", js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setServerRegion", js_cocos2dx_extension_YouMeTalk_setServerRegion, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("switchCamera", js_cocos2dx_extension_YouMeTalk_switchCamera, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("resetCamera", js_cocos2dx_extension_YouMeTalk_resetCamera, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),		
        JS_FN("speakToChannel", js_cocos2dx_extension_YouMeTalk_speakToChannel, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("startCapture", js_cocos2dx_extension_YouMeTalk_startCapture, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getSpeakerMute", js_cocos2dx_extension_YouMeTalk_getSpeakerMute, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("cleanTextures", js_cocos2dx_extension_YouMeTalk_cleanTextures, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setGrabMicOption", js_cocos2dx_extension_YouMeTalk_setGrabMicOption, 5, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setInviteMicOption", js_cocos2dx_extension_YouMeTalk_setInviteMicOption, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("stopBackgroundMusic", js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setUseMobileNetworkEnabled", js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("init", js_cocos2dx_extension_YouMeTalk_init, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("stopInviteMic", js_cocos2dx_extension_YouMeTalk_stopInviteMic, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("responseInviteMic", js_cocos2dx_extension_YouMeTalk_responseInviteMic, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setMicLevelCallback", js_cocos2dx_extension_YouMeTalk_setMicLevelCallback, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getChannelUserList", js_cocos2dx_extension_YouMeTalk_getChannelUserList, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setVolume", js_cocos2dx_extension_YouMeTalk_setVolume, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("unbindTexture", js_cocos2dx_extension_YouMeTalk_unbindTexture, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("openVideoEncoder", js_cocos2dx_extension_YouMeTalk_openVideoEncoder, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("requestGrabMic", js_cocos2dx_extension_YouMeTalk_requestGrabMic, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("leaveChannelMultiMode", js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setReleaseMicWhenMute", js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("playBackgroundMusic", js_cocos2dx_extension_YouMeTalk_playBackgroundMusic, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("startGrabMicAction", js_cocos2dx_extension_YouMeTalk_startGrabMicAction, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getUseMobileNetworkEnabled", js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("requestInviteMic", js_cocos2dx_extension_YouMeTalk_requestInviteMic, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setCaptureProperty", js_cocos2dx_extension_YouMeTalk_setCaptureProperty, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setNetCaptureProperty", js_cocos2dx_extension_YouMeTalk_setNetCaptureProperty, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setListenOtherVoice", js_cocos2dx_extension_YouMeTalk_setListenOtherVoice, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("stopGrabMicAction", js_cocos2dx_extension_YouMeTalk_stopGrabMicAction, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setToken", js_cocos2dx_extension_YouMeTalk_setToken, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setAutoSendStatus", js_cocos2dx_extension_YouMeTalk_setAutoSendStatus, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("joinChannelMultiMode", js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getMicrophoneMute", js_cocos2dx_extension_YouMeTalk_getMicrophoneMute, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("unInit", js_cocos2dx_extension_YouMeTalk_unInit, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("joinChannelSingleMode", js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setCaptureFrontCameraEnable", js_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("releaseGrabMic", js_cocos2dx_extension_YouMeTalk_releaseGrabMic, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("bindTexture", js_cocos2dx_extension_YouMeTalk_bindTexture, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("updateTextures", js_cocos2dx_extension_YouMeTalk_updateTextures, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("stopCapture", js_cocos2dx_extension_YouMeTalk_stopCapture, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("releaseMicSync", js_cocos2dx_extension_YouMeTalk_releaseMicSync, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("resumeMicSync", js_cocos2dx_extension_YouMeTalk_resumeMicSync, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("openBeautify", js_cocos2dx_extension_YouMeTalk_openBeautify, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("beautifyChanged", js_cocos2dx_extension_YouMeTalk_beautifyChanged, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        // JS_FN("stretchFace", js_cocos2dx_extension_YouMeTalk_stretchFace, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    static JSFunctionSpec st_funcs[] = {
        JS_FN("destroy", js_cocos2dx_extension_YouMeTalk_destroy, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getInstance", js_cocos2dx_extension_YouMeTalk_getInstance, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    jsb_YouMeTalk_prototype = JS_InitClass(
        cx, global,
        JS::NullPtr(),
        jsb_YouMeTalk_class,
        js_cocos2dx_extension_YouMeTalk_constructor, 0, // constructor
        properties,
        funcs,
        NULL, // no static properties
        st_funcs);

    // add the proto and JSClass to the type->js info hash table
    JS::RootedObject proto(cx, jsb_YouMeTalk_prototype);
    jsb_register_class<YouMeTalk>(cx, jsb_YouMeTalk_class, proto, JS::NullPtr());
}

void register_all_youmetalk(JSContext* cx, JS::HandleObject obj) {
    // Get the ns
    JS::RootedObject ns(cx);
    get_or_create_js_obj(cx, obj, "ym", &ns);

    js_register_cocos2dx_extension_YouMeTalk(cx, ns);
}

