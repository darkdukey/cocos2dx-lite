/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.lua;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.ArrayList;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.ActivityInfo;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.provider.Settings;
import android.text.format.Formatter;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Toast;
import android.net.Uri;
import java.util.Map;

import cn.magicwindow.MLink;
import cn.magicwindow.MLinkAPIFactory;
import cn.magicwindow.MWConfiguration;
import cn.magicwindow.MagicWindowSDK;
import cn.magicwindow.mlink.MLinkCallback;
import cn.magicwindow.mlink.MLinkIntentBuilder;
import cn.magicwindow.Session;
import cn.magicwindow.mlink.annotation.MLinkDefaultRouter;

@MLinkDefaultRouter
public class AppActivity extends Cocos2dxActivity{

    static String hostIPAdress = "0.0.0.0";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // 魔窗
        MWConfiguration config = new MWConfiguration(this);
        config.setLogEnable(true);      //打开魔窗Log信息
        MagicWindowSDK.initSDK(config);

        MLinkAPIFactory.createAPI(this).registerWithAnnotation(this);
        MLinkAPIFactory.createAPI(this).deferredRouter();

        MLink.getInstance(this).register("LitePlayerKey",new MLinkCallback()
        {
            public void execute(Map<String, String>paramMap, Uri uri,Context context){
                String roomid = "";
                if (paramMap != null) {
                    roomid = paramMap.get("roomid");
                } else if(uri!=null) {
                    roomid = uri.getQueryParameter("roomid");
                }
                Log.e("$$$$$$$$$$$$", "######### roomid="+roomid);
            }
        });

        Uri mLink = getIntent().getData();
        //如果从浏览器传来 则进行路由操作
        if(mLink != null){
            MLink.getInstance(this).router(this, mLink);
        }
    }

    @Override
    protected void onPause() {
        Session.onPause(this);
        super.onPause();
    }
    @Override
    protected void onResume() {
        Session.onResume(this);
        super.onResume();
    }
}
