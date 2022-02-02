/*******************************************************************************************
* Based on the raylib parallax scrolling example by Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>

#include "car.h"
#include "visual_car.h"
#include "visual_bg.h"

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

sf::RenderWindow * window_handle = nullptr;

std::atomic<int> rpm_g = 0;

void sound_thread()
{
  sf::SoundBuffer buffer;
  // load something into the sound buffer...

  buffer.loadFromFile("resources\\truck_engine.wav");

  int count = buffer.getSampleCount();
   const sf::Int16 * p = buffer.getSamples();

   std::vector<sf::Int16> samples;
   for (unsigned i = 0; i < count; ++i)
   {
     samples.push_back(p[i]);
   }

  // do the trimming
   int trimleft = 5.5 * buffer.getSampleRate();
   int duration = 1.0 * buffer.getSampleRate();
   sf::SoundBuffer buffer2;
   buffer2.loadFromSamples(samples.data()+ trimleft, duration, buffer.getChannelCount(), buffer.getSampleRate());
  sf::Sound sound;
  sound.setBuffer(buffer2);
  for (;;)
  {
    if(rpm_g != 0)
    {
      sound.setPitch((rpm_g / 1500.0) / 3 + 1.25);
      sound.setVolume(100 * rpm_g / 9000.0);
      sound.play();
      sf::sleep(sf::milliseconds(150));
    }
    if (rpm_g == -1)
      break;
  }
}


int main(void)
{
  sf::Font font;
  if (!font.loadFromFile("resources/sansation.ttf"))
    return EXIT_FAILURE;

  std::thread sound_t(sound_thread);

  const double global_scale_factor = 2.0;

  const int screenWidth = 800 * global_scale_factor;
  const int screenHeight = 450 * global_scale_factor;

  sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight, 32), "Speedster: Redline",
    sf::Style::Titlebar | sf::Style::Close);

  window.setVerticalSyncEnabled(true);
    
  sf::Text rpm_text;
  sf::Text speed_text;
  sf::Text gear_text;

  sf::RectangleShape rev_bar;

  double rpm = 0.0;
  constexpr int rpm_max = 9000;
  constexpr int rpm_min = 1000;

  constexpr double rev_base_size = 400;
  constexpr int revs_per_sec = 5000;
  sf::Clock sf_clock;

  car_model current_car_model;
  car current_car(current_car_model);

  double angle = 0.0;
  bool gear_ready = true;
  bool finished = false;

  visual_bg background(window);

  std::vector<visual_car*> vcars;

  const int num_cars = 3;

  const sf::Color colors[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };

  for (int i = 0; i < num_cars; ++i)
  {
      auto p_vcar = new visual_car(window);
      auto& vcar = *p_vcar;
      vcar.x_scale = 1.0 * global_scale_factor;
      vcar.y_scale = 1.0 * global_scale_factor;
      vcar.x_pos = (40 + i*10) * global_scale_factor;
      vcar.y_pos = (345 + i*10) * global_scale_factor;
      vcar.color = colors[i];

      vcar.rescale();
      vcars.push_back(p_vcar);
  }
  
  while (!finished) // main loop
  {
    sf::Event event;
    rpm_g = rpm;
    while (window.pollEvent(event))
    {
      // Window closed or escape key pressed: exit
      if ((event.type == sf::Event::Closed) ||
        ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
      {
        rpm_g = -1;
        window.close();
        finished = true;
        break;
      }

    }
    float frame_time = sf_clock.restart().asSeconds();
    int fps = (int)(1.0 / frame_time);
    double rev_speed = frame_time * revs_per_sec;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
      current_car.throttle = 100;
    }
    else
    {
      current_car.throttle = 0;
      if (current_car.rpm < 6000)
      {
          current_car.gear_shift_down();
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) )
    {
      if (gear_ready)
      {
        current_car.gear_shift_up();
        gear_ready = false;
      }
    }
    else
    {
      gear_ready = true;
    }

    current_car.update_acceleration();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) // braking
    {
      current_car.brake(frame_time);
    }
    current_car.update_rpm(frame_time);

    current_car.update_speed(frame_time);

    if (current_car.rpm > current_car.model.rpm_max)
      current_car.rpm = current_car.model.rpm_max;
    if (current_car.rpm <current_car.model.rpm_min)
      current_car.rpm = current_car.model.rpm_min;
    
    rpm = current_car.rpm;

    double car_moved_dist = current_car.speed * frame_time;

    window.clear(sf::Color(50, 50, 50));

    double speedms = current_car.speed / 3.6;
    double wheel_rpm = (current_car_model.wheel_radius * 2 * 3.14) * speedms;

    double wheel_rotation_delta = 360.0 * frame_time * wheel_rpm * 0.3;
    

    double rpm_factor = ((rpm - rpm_min) / double(rpm_max - rpm_min));

    double rev_size = rev_base_size * rpm_factor * global_scale_factor;
    // double rev_size = 300;
    rev_bar.setSize(sf::Vector2f(rev_size, 23 * global_scale_factor));

    rev_bar.setOutlineThickness(3);
    rev_bar.setOutlineColor(sf::Color::White);

    rev_bar.setFillColor(sf::Color(255*rpm_factor, 255-255*rpm_factor, 127-127*rpm_factor));
    rev_bar.setOrigin(0, 0);
    rev_bar.setPosition({ 60*global_scale_factor, 50 *global_scale_factor });

    rpm_text.setFont(font);
    rpm_text.setCharacterSize(18 * global_scale_factor);
    rpm_text.setPosition(60. * global_scale_factor, 46 * global_scale_factor);
    rpm_text.setFillColor(sf::Color::White);
    rpm_text.setString(std::to_string(current_car.rpm));

    gear_text.setFont(font);
    gear_text.setCharacterSize(32 * global_scale_factor);
    gear_text.setPosition(32 * global_scale_factor, 40 * global_scale_factor);
    gear_text.setFillColor(sf::Color::White);
    gear_text.setString(std::to_string(current_car.gear));

    speed_text.setFont(font);
    speed_text.setCharacterSize(32 * global_scale_factor);
    speed_text.setPosition(rev_bar.getPoint(0).x + (rev_base_size + 80)*global_scale_factor, 40 * global_scale_factor);
    speed_text.setFillColor(sf::Color::White);
    speed_text.setString(std::to_string(int(current_car.speed)));
    static double total_dist = 0;
  //  rev_bar.setPosition({ 100,400 });

    background.x_scale = global_scale_factor;
    background.y_scale = global_scale_factor;

    background.redraw(car_moved_dist);
    total_dist += car_moved_dist;

   // std::cout << (int)total_dist << "\n";
    for (auto p_vcar : vcars)
    {
       // p_vcar->rescale();
        double relative_pos = rand() % 15 - 8;
        if (relative_pos > 0 && p_vcar != vcars[0])
        {
            p_vcar->x_pos += relative_pos;
            p_vcar->rescale();

            if (p_vcar->x_pos > window.getSize().x)
            {
                sf::Text you_lose;
                you_lose.setFont(font);
                you_lose.setCharacterSize(128 * global_scale_factor);
                you_lose.setPosition(170, window.getSize().y / 2 - 200);
                you_lose.setFillColor(sf::Color::Red);
                you_lose.setOutlineColor(sf::Color(220,220,220));
                you_lose.setOutlineThickness(6);
                you_lose.setString("YOU LOSE!");
                window.draw(you_lose);
                window.display();
                sf::sleep(sf::milliseconds(5000));
            }

        }
        p_vcar->redraw(wheel_rotation_delta);
    }

    window.draw(rev_bar);
    
    window.draw(gear_text);
    window.draw(rpm_text);
    window.draw(speed_text);

    window.display();
  }
  sound_t.join();
  return 0;
}