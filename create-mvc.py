#!/usr/bin/env python
#coding=utf-8

import os,sys,io

files = [
# ccs =
['raw/__NAME__View.ccs',
'''
<Solution>
  <PropertyGroup Name="__NAME__View" Version="3.10.0.0" Type="CocosStudio" />
  <SolutionFolder>
    <Group ctype="ResourceGroup">
      <RootFolder Name=".">
        <Folder Name="views">
          <Folder Name="public">
            <Image Name="close.png" />
          </Folder>
          <Project Name="BBView.csd" Type="Node" />
        </Folder>
      </RootFolder>
    </Group>
  </SolutionFolder>
</Solution>
'''
]

# cfg =
,
['raw/__NAME__View.cfg',
'''
<SolutionConfig Version="3.10.0.0">
  <PublishDirectory Value="../res/" />
  <PackageDirectory Value="package/" />
  <PublishType Value="Reference" />
  <SolutionSize Value="960 * 640" />
  <ResolutionName Value="iPhone 4/4S" />
  <DefaultSerializer Value="Serializer_FlatBuffers" />
  <CustomSerializer Value="Serializer_FlatBuffers" />
  <IsNameStandardized Value="False" />
  <CustomProperties>
    <Item Key="CCS_CocosPropertis">
      <Value ctype="CocosProperties">
        <SolutionCodeType Value="Resource" />
        <ProgramLanguage Value="none" />
        <CreateFrameworkVersion Value="" />
        <CurrentFrameworkVersion Value="" />
        <EngineType Value="Prebuilt" />
      </Value>
    </Item>
  </CustomProperties>
</SolutionConfig>
'''
]

# udf =
,
['raw/__NAME__View.udf',
'''
<UserData Version="3.10.0.0">
  <Properties>
    <Item Key="CocosRecentOperation">
      <Value ctype="CocosRecentOperation">
        <LastPublishType Value="Resource" />
        <IsLastPublish Value="True" />
        <LastRunType Value="Mac" />
      </Value>
    </Item>
    <Item Key="PackageParamsKey">
      <Value ctype="PackageParams">
        <LuaIsEncrypt Value="False" />
        <LuaEncryptKey Value="" />
        <LuaEncryptSign Value="" />
        <Android_PackageName Value="org.cocos.MsgBoxView" />
        <AndroidkeyStore Value="" />
        <AndroidVersion Value="" />
        <iOS_Target Value="MsgBoxView iOS" />
        <iOS_BundleID Value="" />
        <Platform Value="None" />
        <RunPlatform Value="None" />
        <FrameworkVersion Value="" />
        <IsDebugKeystore Value="True" />
        <EnableSourceMap Value="False" />
        <EnableHTML5Advanced Value="False" />
      </Value>
    </Item>
    <Item Key="TabsParamsKey">
      <Value ctype="TabsInfo">
        <OpenedDocuments>
          <FilePathData Path="views/__NAME__View.csd" />
        </OpenedDocuments>
        <ActiveDocument Path="views/__NAME__View.csd" />
      </Value>
    </Item>
  </Properties>
</UserData>
'''
]

# views_csd =
,
['raw/cocosstudio/views/__NAME__View.csd',
'''
<GameFile>
  <PropertyGroup Name="__NAME__View" Type="Node" ID="18d60a97-de13-413d-af97-214edf7e8039" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Node" Tag="378" ctype="GameNodeObjectData">
        <Size X="0.0000" Y="0.0000" />
        <Children>
          <AbstractNodeData Name="MainPanel" ActionTag="1240310140" Tag="379" IconVisible="False" PositionPercentXEnabled="True" PositionPercentYEnabled="True" LeftMargin="-568.0000" RightMargin="-568.0000" TopMargin="-320.0000" BottomMargin="-320.0000" TouchEnable="True" ClipAble="False" BackColorAlpha="153" ComboBoxIndex="1" ColorAngle="90.0000" ctype="PanelObjectData">
            <Size X="1136.0000" Y="640.0000" />
            <Children>
              <AbstractNodeData Name="panel" ActionTag="811148742" Tag="380" IconVisible="False" PositionPercentXEnabled="True" PositionPercentYEnabled="True" LeftMargin="468.0000" RightMargin="468.0000" TopMargin="220.0000" BottomMargin="220.0000" TouchEnable="True" ClipAble="False" BackColorAlpha="102" ComboBoxIndex="1" ColorAngle="90.0000" ctype="PanelObjectData">
                <Size X="200.0000" Y="200.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="568.0000" Y="320.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.5000" Y="0.5000" />
                <PreSize X="0.1761" Y="0.3125" />
                <SingleColor A="255" R="150" G="200" B="255" />
                <FirstColor A="255" R="150" G="200" B="255" />
                <EndColor A="255" R="255" G="255" B="255" />
                <ColorVector ScaleY="1.0000" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition />
            <PreSize X="0.0000" Y="0.0000" />
            <SingleColor A="255" R="0" G="0" B="0" />
            <FirstColor A="255" R="150" G="200" B="255" />
            <EndColor A="255" R="255" G="255" B="255" />
            <ColorVector ScaleY="1.0000" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>
'''
]

# views_udf =
,
['raw/cocosstudio/views/__NAME__View.udf',
'''
<UserData Version="3.10.0.0">
  <Properties>
    <Item Key="GuidesList">
      <Value ctype="GuidesData" />
    </Item>
  </Properties>
</UserData>
'''
]

# code_view =
,
['src/app/views/__NAME__View.lua',
'''
local __NAME__View = {}

function __NAME__View:ctor()
  self.ui:setPosition(display.cx, display.cy)
end

function __NAME__View:layout()
end


return __NAME__View

'''
]

# code_ctrl =
,
['src/app/controllers/__NAME__Controller.lua',
'''
local Controller = require('mvc.Controller')
local __NAME__Controller = class("__NAME__Controller", Controller)

function __NAME__Controller:ctor()
    __NAME__Controller.super.ctor(self)
    cc.load('event'):bind(self)
end

function __NAME__Controller:viewDidLoad()
    self.view:layout()
end

function __NAME__Controller:clickClose(  )
    self.emitter:emit('back')
end

return __NAME__Controller
'''
]
]

def warn(msg):
  print('\x1b[0;31;40m' + msg + '\x1b[0m')

def log(msg):
  print('\x1b[6;30;42m' + msg + '\x1b[0m')

def usage():
  print('''%s Lobby''' %(__file__))
  exit(1)

if len(sys.argv) != 2 or sys.argv[1] in ['-h', '--help', '-help']:
  usage()

name = sys.argv[1]

currdir = os.path.dirname(os.path.realpath(__file__))
fileexist = False
for item in files:
  path = os.path.join(item[0].replace('__NAME__', name))
  if os.path.exists(path):
    fileexist = True
    warn(path + ' exist, remove first.')

if fileexist:
  exit(1)

print('Create files ...\n')

for item in files:
  path = os.path.join(item[0].replace('__NAME__', name))
  c = item[1].replace('__NAME__', name)
  log('>> creating ' + path)
  with open(path, 'w') as file:
    file.write(c)

print('\nDone.')
