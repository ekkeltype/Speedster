#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>

#include <vector>
#include <filesystem>

struct layer
{
  sf::Texture texture;
  float current_scroll_pos = 0.f;
  float scroll_rate = 1.f;
  sf::Sprite sprite;
  float x_scale = 1.f;
  float y_scale = 1.f;

  layer(const std::string& file)
  {
    texture.loadFromFile(file);
    texture.setRepeated(true);
  }
};

struct visual_bg
{
  sf::RenderWindow& window_;

  std::vector<layer*> layers;

  float bg_scroll_pos = 0.0f;
  float mg_scroll_pos = 0.0f;
  float fg_scroll_pos = 0.0f;

  float base_scale = 2.0;
  float x_scale = 1.0;
  float y_scale = 1.0;

  visual_bg(sf::RenderWindow& window, const std::string& bgname, int num_layers) : window_(window)
  {
    //const int use_layers[] = { 0,1,2 };
   // const int num_layers = sizeof(use_layers) / sizeof(int);
    int idx = 0;
    for (int i = 0; i < num_layers; ++i)
    {
      std::string filename = bgname + "\\" + std::to_string(i) + ".png";
      layer * p_lay = new layer(filename);
      layer& lay = *p_lay;
      //	layer lay("forest\\" + std::to_string(i) + ".png");
      lay.scroll_rate = (idx + 1) / float(num_layers);

      float calculated_scale = static_cast<float>(window.getSize().y) / lay.texture.getSize().y;
      lay.x_scale = calculated_scale;
      lay.y_scale = calculated_scale;

      sf::Sprite sprite(lay.texture);

      //sprite.setTexture(texture);
      sprite.setTextureRect({ 0,0,5000,(int)window_.getSize().y });
      sprite.setScale({ lay.x_scale, lay.y_scale });
      lay.sprite = sprite;
      layers.push_back(p_lay);
      ++idx;
    }

  }

  void redraw(double moved_dist)
  {
    const double pixel_len = 0.08;
    int i = 0;
    for (auto p_lay : layers)
    {
      layer& lay = *p_lay;
      lay.current_scroll_pos -= static_cast<float>(lay.scroll_rate * moved_dist / pixel_len);
      if (fabs(lay.current_scroll_pos) > lay.texture.getSize().x * lay.x_scale)
        lay.current_scroll_pos = 0;

      lay.sprite.setPosition(lay.current_scroll_pos, static_cast<float>(i * 10));
      ++i;

      window_.draw(lay.sprite);
    }

  }
};