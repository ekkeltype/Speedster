uniform sampler2D texture;
uniform vec3 dest_color;

bool isWhite(float r, float g, float b)
{
    return (r > 0.9) && (g > 0.9) && (b > 0.9);
}

void main()
{
  vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

  if(!isWhite(texColor.x, texColor.y, texColor.z) && texColor.x > 0.3)
  //{
    //// float r = 
    float rgb_sum = (texColor.x + texColor.y + texColor.z);
    float rgb_dest_sum = (dest_color.x + dest_color.y + dest_color.z);

    float r_ratio = dest_color.x / rgb_dest_sum;
    float g_ratio = dest_color.y / rgb_dest_sum;
    float b_ratio = dest_color.z / rgb_dest_sum;

   // texColor.x = rgb_sum / r_ratio;
   // texColor.y = rgb_sum / g_ratio;
    //texColor.z = rgb_sum / b_ratio;

    //texColor.x = 0.5* rgb_sum / r_ratio;
    //texColor.y = 0.5*rgb_sum / g_ratio;
    //texColor.z = 0.5*rgb_sum / b_ratio;

     //texColor = dest_color;
    // texColor.xyz *= dest_color.xyz;
    texColor.x *= dest_color.x;
    texColor.y *= dest_color.y;
    texColor.z *= dest_color.z;
  //}

  gl_FragColor = texColor;  // vec4(1.0, 0.0, 0.0, 0.5);
}