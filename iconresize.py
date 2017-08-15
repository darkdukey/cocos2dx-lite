#!/usr/bin/env python
#coding=utf-8

'''
sudo pip install Pillow

使用方法：同级目录放一个 icon.png     (512x512) 或者 (1024x1024)
'''

import sys
import os
import shutil
from PIL import Image

#自动生成android,ios 需要的图标
#python icon.py
def generate():
    iPath = 'icon.png'
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

    android_dir = 'frameworks/runtime-src/proj.android/res/'
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

    ios_dir = 'frameworks/runtime-src/proj.ios_mac/ios/'
    if not os.path.exists(ios_dir):
        os.makedirs(ios_dir)
    for size in sizes:
        img = icon.resize((size,size), Image.ANTIALIAS)
        oPath = ios_dir+'Icon-'+str(size)+'.png'
        img.save(oPath, icon.format)
        print(oPath)

generate()
