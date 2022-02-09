#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "car.h"
struct visual_car
{
  sf::Texture car_texture;
  sf::Texture car_body;
  sf::Texture car_rear_wheel;
  sf::Texture car_front_wheel;

  sf::Sprite car_body_s;
  sf::Sprite car_rwheel_s;
  sf::Sprite car_fwheel_s;

  double current_wheel_angle = 0.0;
  double x_scale = 1.0;
  double y_scale = 1.0;
  double x_pos = 40;
  double y_pos = 345;

  const car * physical_car = nullptr;
  
  double base_scale = 0.35;
  sf::Color color;
  sf::Color current_color;
  sf::Shader shader;

  sf::RenderWindow& window_;
  visual_car(sf::RenderWindow& window) :
    window_(window)
  {
    car_texture.loadFromFile("car\\car-full.png");
    car_body.loadFromFile("car\\car-body.png");
    car_rear_wheel.loadFromFile("car\\car-wheel.png");
    car_front_wheel.loadFromFile("car\\car-wheel.png");

    car_body_s.setTexture(car_body);
    car_rwheel_s.setTexture(car_rear_wheel);
    car_fwheel_s.setTexture(car_front_wheel);
  }

  void recolor()
  {
    sf::Glsl::Vec3 dest_color(current_color.r / 255.f, current_color.g / 255.f, current_color.b / 255.f);
    shader.loadFromFile("car_shader.frag", sf::Shader::Type::Fragment);
    shader.setUniform("dest_color", dest_color);
  }

  void rescale()
  {
    //  std::cout << "setpos " << x_pos << ", " << y_pos << "\n";
    car_body_s.setPosition(static_cast<float>(x_pos), static_cast<float>(y_pos));
    //  std::cout << "setscale " << x_scale * base_scale << ", " << y_scale * base_scale << "\n";

    car_body_s.setScale(static_cast<float>(x_scale * base_scale), static_cast<float>(y_scale * base_scale));

    int wheel_r = 60;

    double rwheel_x_offset = (12 + wheel_r / 2) * x_scale;
    double rwheel_y_offset = (38 + wheel_r / 2) * y_scale;

    car_rwheel_s.setPosition(static_cast<float>(x_pos + rwheel_x_offset), static_cast<float>(y_pos + rwheel_y_offset));
    car_rwheel_s.setScale(static_cast<float>(x_scale * base_scale), static_cast<float>(y_scale * base_scale));

    double fwheel_x_offset = (167 + wheel_r / 2) * x_scale;
    double fwheel_y_offset = (35 + wheel_r / 2) * y_scale;

    car_fwheel_s.setPosition(static_cast<float>(x_pos + fwheel_x_offset), static_cast<float>(y_pos + fwheel_y_offset));
    car_fwheel_s.setScale(static_cast<float>(x_scale * base_scale), static_cast<float>(y_scale * base_scale));

    car_fwheel_s.setOrigin(static_cast<float>(wheel_r), static_cast<float>(wheel_r));
    car_rwheel_s.setOrigin(static_cast<float>(wheel_r), static_cast<float>(wheel_r));

  //  car_body_s.setColor(color);
  }

  void redraw(double wheel_rotation_delta)
  {
    current_wheel_angle += wheel_rotation_delta;
    if (current_wheel_angle > 360.0)
      current_wheel_angle -= 360.0;

    car_fwheel_s.setRotation(static_cast<float>(current_wheel_angle));
    car_rwheel_s.setRotation(static_cast<float>(current_wheel_angle));

    sf::RenderStates states;

    states.shader = &shader;
    window_.draw(car_body_s, states);
    window_.draw(car_rwheel_s, states);
    window_.draw(car_fwheel_s, states);
  }

};