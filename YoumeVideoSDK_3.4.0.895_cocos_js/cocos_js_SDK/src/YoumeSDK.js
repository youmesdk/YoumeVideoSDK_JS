/**
 * Created by Y.X. on 2017/6/21.
 */
var ym = ym || {};

var addTips = function (strTips, err) {
        var berr = err? err: false;
        var event = new cc.EventCustom(YMEvtTip);
        var p = {
            iserr: berr,
            str: strTips
        };
        event.setUserData( p );
        cc.eventManager.dispatchEvent( event );
}

ym.YouMe = function() {
    if(ym._instance == undefined) {
        ym._instance = new ym.YouMeTalk();
    }

    return ym._instance;
};

ym.YouMe().Auth = function(){
	addTips("开始初始化......");
    return ym.YouMe().init(YM_APP_KEY, YM_APP_SECRET, YM_APP_REGION, YM_APP_REGIONNAME);	
}

ym.YouMe().OnMemberChange = function (channel,jsretArr,bUpdate) {
	var len = 0;
	len = jsretArr.length;

	for(var i=0; i<len; ++i)
	{
		cc.log("memberChange %d  \n",i);
		cc.log("memberChange2 %s  \n",JSON.stringify(jsretArr));
		var userid = jsretArr[i].UserID;
		var evt = new cc.EventCustom(YMEvt.ymevt_memberchange);
        var p = {
              uid: userid
            };
        evt.setUserData(p);
        cc.eventManager.dispatchEvent( evt);
	    cc.log("memberChange3 %s  \n",userid);
	}
	return;
}

ym.YouMe().OnEvent = function( event,  errorcode, channel, param ) {
	var ssTips = "OnEvent:event_"+event + ",error_" + errorcode + ",channel_" + channel + ",param_" + param;
	cc.log(ssTips);
	switch( event ){
		case 0: //YOUME_EVENT_INIT_OK:
			cc.log("Talk 初始化成功");
			addTips("初始化成功");
			var evt = new cc.EventCustom(YMEvt.ymevt_init);
			var p = {
				errcode: errorcode
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 1://YOUME_EVENT_INIT_FAILED:
			cc.log("Talk 初始化失败");
			addTips("初始化失败", true);
			var evt = new cc.EventCustom(YMEvt.ymevt_init);
			var p = {
				errcode: errorcode
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 2://YOUME_EVENT_JOIN_OK:
			cc.log("Talk 进入房间成功");
			addTips( "进入房间成功，ID：" + channel  );
			var evt = new cc.EventCustom(YMEvt.ymevt_join);
			var p = {
				errcode: errorcode,
				uid:param
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 3://YOUME_EVENT_JOIN_FAILED:
			cc.log("Talk 进入房间失败:");
			addTips( "进入房间失败", true );
			var evt = new cc.EventCustom(YMEvt.ymevt_join);
			var p = {
				errcode: errorcode,
				uid:param
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 4://YOUME_EVENT_LEAVED_ONE:
			cc.log("Talk 离开单个房间");
			var s = errorcode == 0? "退出房间成功": "退出房间失败";
			addTips(s, errorcode != 0);
			break;
		case 5://YOUME_EVENT_LEAVED_ALL:
			cc.log("Talk 离开所有房间");
			var s = errorcode == 0? "离开所有房间成功": "离开所有房间失败";
			addTips(s, errorcode != 0);
			var evt = new cc.EventCustom(YMEvt.ymevt_leave);
			var p = {
				errcode: errorcode,
				uid:param
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 10://YOUME_EVENT_RECONNECTING:///< 断网了，正在重连
			cc.log("Talk 正在重连");
			addTips( "正在重连" );
			break;
		case 11://YOUME_EVENT_RECONNECTED:///< 断网重连成功
			cc.log("Talk 重连成功");
			addTips( "重连成功" );
			break;
		case 12://YOUME_EVENT_REC_PERMISSION_STATUS:///< 通知录音权限状态，成功获取权限时错误码为YOUME_SUCCESS，获取失败为YOUME_ERROR_REC_NO_PERMISSION（此时不管麦克风mute状态如何，都没有声音输出）
		    cc.log("录音权限状态,%d", errorcode);
		    addTips( "录音权限状态,%d", errorcode );
			break;
		case 13://YOUME_EVENT_BGM_STOPPED:///< 通知背景音乐播放结束
			cc.log("背景音乐播放结束,%s",param);
			addTips( "背景音乐播放结束" );
			break;
		case 14://YOUME_EVENT_BGM_FAILED:///< 通知背景音乐播放失败
			cc.log("背景音乐播放失败,%s",param);
			addTips( "背景音乐播放失败", true );
			break;
		case 15://YOUME_EVENT_MEMBER_CHANGE:///< 频道成员变化
			cc.log("频道成员变化,%s",param);
			addTips( "频道成员变化:"+param );
			break;
		case 16://YOUME_EVENT_OTHERS_MIC_ON:///< 其他用户麦克风打开
			cc.log("其他用户麦克风打开,%s",param);
			addTips( "其他用户麦克风打开:"+param );
			break;
		case 17://YOUME_EVENT_OTHERS_MIC_OFF:///< 其他用户麦克风关闭
			cc.log("其他用户麦克风关闭,%s",param);
			addTips( "其他用户麦克风关闭:"+param );
			break;
		case 18://YOUME_EVENT_OTHERS_SPEAKER_ON:///< 其他用户扬声器打开
			cc.log("其他用户扬声器打开,%s",param);
			addTips( "其他用户扬声器打开:"+param );
			break;
		case 19://YOUME_EVENT_OTHERS_SPEAKER_OFF: ///< 其他用户扬声器关闭
			cc.log("其他用户扬声器关闭,%s",param);
			addTips( "其他用户扬声器关闭:"+param );
			break;
		case 20://YOUME_EVENT_OTHERS_VOICE_ON: ///<其他用户进入讲话状态
			cc.log("其他用户进入讲话状态,%s",param);
			addTips( "其他用户进入讲话状态:"+param );
			break;
		case 21://YOUME_EVENT_OTHERS_VOICE_OFF: ///<其他用户进入静默状态
			cc.log("其他用户进入静默状态,%s",param);
			addTips( "其他用户进入静默状态:"+param );
			break;
		case 200://YOUME_EVENT_OTHERS_VIDEO_ON: ///<收到其它用户的视频流
			cc.log("用户视频打开,%s",param);
			addTips( "用户视频打开:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_start
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 201://YOUME_EVENT_OTHERS_VIDEO_OFF: ///<其他用户视频流断开
			cc.log("用户视频关闭,%s",param);
			addTips( "用户视频关闭:"+param );
			break;
		case 202://YOUME_EVENT_OTHERS_CAMERA_PAUSE  = 24, ///< 其他用户摄像头暂停
			cc.log("其他用户摄像头暂停,%s",param);
			addTips( "其他用户摄像头暂停:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_pause
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 203://YOUME_EVENT_OTHERS_CAMERA_RESUME = 25, ///< 其他用户摄像头恢复
			cc.log("其他用户摄像头恢复,%s",param);
			addTips( "其他用户摄像头恢复:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_resume
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 204://YOUME_EVENT_MASK_VIDEO_BY_OTHER_USER   = 26, ///< 视频被其他用户屏蔽
			cc.log("视频被其他用户屏蔽,%s",param);
			addTips( "视频被其他用户屏蔽:"+param );
			break;
		case 205://YOUME_EVENT_RESUME_VIDEO_BY_OTHER_USER = 27, ///< 视频被其他用户恢复
			cc.log("视频被其他用户恢复,%s",param);
			addTips( "视频被其他用户恢复:"+param );
			break;
		case 206://YOUME_EVENT_MASK_VIDEO_FOR_USER        = 28, ///< 屏蔽了谁的视频
			cc.log("屏蔽了谁的视频,%s",param);
			addTips( "屏蔽了谁的视频:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_mask
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 207://YOUME_EVENT_RESUME_VIDEO_FOR_USER      = 29, ///< 恢复了谁的视频
			cc.log("恢复了谁的视频,%s",param);
			addTips( "恢复了谁的视频:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_unmask
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		case 208://YOUME_EVENT_OTHERS_VIDEO_SHUT_DOWN     = 30, ///< 其它用户的视频流断开（包含网络中断的情况）
			cc.log("视频断开,%s",param);
			addTips( "视频断开:"+param );
			var evt = new cc.EventCustom(YMEvt.ymevt_video);
			var p = {
				errcode: errorcode,
				uid:param,
				state: YMState.ymst_stop
			};
			evt.setUserData(p);
			cc.eventManager.dispatchEvent( evt);
			break;
		default:
			break;
	}
}