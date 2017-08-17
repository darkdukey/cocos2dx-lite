#!/usr/bin/env python
#coding=utf-8

'''
sudo pip install Pillow

使用方法：同级目录放一个 icon.png     (512x512) 或者 (1024x1024)
'''

import sys
import os
import shutil

def warn(msg):
  print('\x1b[0;31;40m' + msg + '\x1b[0m')

import imp
try:
    imp.find_module('PIL')
    from PIL import Image
except ImportError:
    warn('运行\nsudo pip install Pillow\n安装 PIL 模块')
    exit (0)

path        = os.path.split(os.path.realpath(__file__))[0]
android_dir = os.path.join(path, 'frameworks/runtime-src/proj.android/res/')
ios_dir     = os.path.join(path, 'frameworks/runtime-src/proj.ios_mac/ios/')

#自动生成android,ios 需要的图标
#python icon.py
def generate():
    iPath = os.path.join(path, 'icon.png')
    if not os.path.exists(iPath):
        print('> 文件不存在：', iPath)
        exit (0)
    icon = Image.open(iPath)

    #android
    sizeFolders = [
        ('drawable',512),
        ('drawable-hdpi',72),
        ('drawable-ldpi',36),
        ('drawable-mdpi',48),
        ('drawable-xhdpi',96),
        #('drawable-xxhdpi',144),
        #('drawable-xxxhdpi',192),
    ]
    # names = ['icon','push']
    names = ['icon']

    for s in sizeFolders:
        folder,size = s
        img = icon.resize((size,size),Image.ANTIALIAS)

        oFolder = android_dir+folder
        if not os.path.exists(oFolder):
            os.makedirs(oFolder)
        for name in names:
            oPath = oFolder+'/'+name+'.png'
            img.save(oPath, icon.format)
            print(oPath)

    # ios
    sizes = [
        29,
        40,
        #48,
        50,
        57,
        58,
        72,
        76,
        80,
        #96,
        100,
        114,
        120,
        144,
        152,
    ]

    if not os.path.exists(ios_dir):
        os.makedirs(ios_dir)
    for size in sizes:
        img = icon.resize((size,size), Image.ANTIALIAS)
        oPath = ios_dir+'Icon-'+str(size)+'.png'
        img.save(oPath, icon.format)
        print(oPath)

generate()
