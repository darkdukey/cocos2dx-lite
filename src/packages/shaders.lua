
local shaders = {}

function shaders.roundNode( node,edge )

  local type = tolua.type(node)
  if type ~= 'cc.Sprite' then
    print('**WARNING**: [shaders.roundNode] node should be cc.Sprite, but', type)
    return
  end

  local strVertSource = [[
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    attribute vec4 a_color;

    #ifdef GL_ES
    varying lowp vec4 v_fragmentColor;
    varying mediump vec2 v_texCoord;
    #else
    varying vec4 v_fragmentColor;
    varying vec2 v_texCoord;
    #endif

    void main()
    {
        gl_Position = CC_PMatrix * a_position;
        v_fragmentColor = a_color;
        v_texCoord = a_texCoord;
    }
  ]]

  local strFragSource = [[
  #ifdef GL_ES
  varying lowp vec4 v_fragmentColor;
  varying mediump vec2 v_texCoord;
  #else
  varying vec4 v_fragmentColor;
  varying vec2 v_texCoord;
  #endif

  uniform float u_edge; // 0.1

  void main()
  {
      float edge = u_edge;
      float dis = 0.0;
      vec2 texCoord = v_texCoord;
      if ( texCoord.x < edge )
      {
          if ( texCoord.y < edge )
          {
              dis = distance( texCoord, vec2(edge, edge) );
          }
          if ( texCoord.y > (1.0 - edge) )
          {
              dis = distance( texCoord, vec2(edge, (1.0 - edge)) );
          }
      }
      else if ( texCoord.x > (1.0 - edge) )
      {
          if ( texCoord.y < edge )
          {
              dis = distance( texCoord, vec2((1.0 - edge), edge ) );
          }
          if ( texCoord.y > (1.0 - edge) )
          {
              dis = distance( texCoord, vec2((1.0 - edge), (1.0 - edge) ) );
          }
      }

      if(dis > 0.001)
      {
          float gap = edge * 0.1;
          if(dis <= edge - gap)
          {
              gl_FragColor = texture2D( CC_Texture0,texCoord);
          }
          else if(dis <= edge)
          {
              //gl_FragColor = texture2D( CC_Texture0,texCoord) * (gap - (dis - edge + gap))/gap;
              gl_FragColor = vec4(0,0,0,0);
          }
      }
      else
      {
          gl_FragColor = texture2D( CC_Texture0,texCoord);
      }
  }
  ]]

  local shaderKey = 'shaderRound'
  local glCache = cc.GLProgramCache:getInstance()
  local glProgram = glCache:getGLProgram(shaderKey)
  if not glProgram then
    glProgram = cc.GLProgram:createWithByteArrays(strVertSource, strFragSource)
    glProgram:link()
    glProgram:updateUniforms()
    glCache:addGLProgram(glProgram, shaderKey)
  end
  if not glProgram then print('glProgram is nil') return end

  local glProgramState = cc.GLProgramState:getOrCreateWithGLProgram(glProgram)
  if not glProgramState then print('glProgramState is nil') return end

  edge = edge or 0.1
  glProgramState:setUniformFloat('u_edge', 0.1)
  node:setGLProgramState(glProgramState)
end

function shaders.grayNode( node )
  if not node then return end

  -- 顶点shader
  local vertex = [[
      attribute vec4 a_position;
      attribute vec2 a_texCoord;
      attribute vec4 a_color;
      #ifdef GL_ES
      varying lowp vec4 v_fragmentColor;
      varying mediump vec2 v_texCoord;
      #else
      varying vec4 v_fragmentColor;
      varying vec2 v_texCoord;
      #endif
      void main()
      {
          gl_Position = CC_PMatrix * a_position;
          v_fragmentColor = a_color;
          v_texCoord = a_texCoord;
      }
  ]]

  -- 片段shader
  local fragment= [[
      #ifdef GL_ES
      precision mediump float;  // shader默认精度为double，openGL为了提升渲染效率将精度设为float
      #endif
      // varying变量为顶点shader经过光栅化阶段的线性插值后传给片段着色器
      varying vec4 v_fragmentColor;  // 颜色
      varying vec2 v_texCoord;       // 坐标
      void main(void)
      {
          // texture2D方法从采样器中进行纹理采样，得到当前片段的颜色值。CC_Texture0即为一个采样器
          vec4 c = texture2D(CC_Texture0, v_texCoord);
          // c.rgb即是像素点的三种颜色，dot为点乘，vec3为经验值，可以随意修改
          float gray = dot(c.rgb, vec3(0.299, 0.587, 0.114));
          // shader的内建变量，表示当前片段的颜色
          gl_FragColor.xyz = vec3(gray);
          // a为透明度
          gl_FragColor.a = c.a;
      }
  ]]
  local pProgram = cc.GLProgram:createWithByteArrays(vertex , fragment)
  -- img为sprite
  node:setGLProgram(pProgram)
end

return shaders
