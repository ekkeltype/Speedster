/*******************************************************************************************
* Very loosely based on the raylib parallax scrolling example by Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "car.h"
#include "visual_car.h"
#include "visual_bg.h"
#include "menu.h"

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

sf::RenderWindow * window_handle = nullptr;

std::atomic<int> rpm_g = 0;

// signal to terminate the sound thread. initialized to true
extern std::atomic<bool> run_sound;
void sound_thread();
void game_over(sf::Font &font, const double &global_scale_factor, sf::RenderWindow &window, bool win);

void main_menu(sf::RenderWindow& window);

namespace screen_resolution
{
  int width = 0;
  int height = 0;
}

int main(void)
{
  sf::Font font;
  if (!font.loadFromFile("C:\\windows\\fonts\\arial.ttf"))
    return EXIT_FAILURE;

  std::thread sound_t(sound_thread);

  screen_resolution::width = sf::VideoMode::getDesktopMode().width;
  screen_resolution::height = sf::VideoMode::getDesktopMode().height;

  const double base_render_width = 800;
  const double base_render_height = 450;

  double wscale = screen_resolution::width / base_render_width;
  double hscale = screen_resolution::height / base_render_height;
   double global_scale_factor = std::min(wscale, hscale) - 1;

  global_scale_factor = 2;
  std::cout << "global scale factor is " << global_scale_factor << "\n";

  const int screenWidth = static_cast<int>(800 * global_scale_factor);
  const int screenHeight = static_cast<int>(450 * global_scale_factor);

  sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight, 32), "Speedster: Redline",
    sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

  window.setVerticalSyncEnabled(true);
  main_menu(window);
  run_sound = false;
  sound_t.join();
  return 0;

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

  const car_model current_car_model;

  double angle = 0.0;
  bool gear_ready = true;
  bool finished = false;

  std::vector<car*> cars;
  std::vector<visual_car*> vcars;
  std::vector<car*> ai_cars;

  const int num_cars = 8;
  int ai_skill[num_cars - 1];
  sf::Color colors[num_cars] = { };
  visual_bg background(window, "cyberpunk", 3);
  for (int i = 0; i < num_cars; ++i)
  {
    // randomize color
    sf::Color rcolor = sf::Color(rand() % 255, rand() % 255, rand() % 255);
    colors[i] = rcolor;
    car * new_car = new car(current_car_model);
    cars.push_back(new_car);
    if (i > 0)
    {
      ai_cars.push_back(new_car);
      ai_skill[i-1] = rand() % 100;
      new_car->rpm = 1500 + ai_skill[i - 1] * 10;
    }

    auto p_vcar = new visual_car(window);
    auto& vcar = *p_vcar;
    vcar.x_scale = 1.0 * global_scale_factor;
    vcar.y_scale = 1.0 * global_scale_factor;
    vcar.x_pos = (40 + i * 10) * global_scale_factor;
    vcar.y_pos = (345 + i * 10) * global_scale_factor;
    vcar.color = colors[i];
    vcar.current_color = colors[i];
    vcar.physical_car = new_car;
    vcar.recolor();
    vcar.rescale();
    vcars.push_back(p_vcar);
  }
  car& current_car = *cars[0];

  while (!finished) // main loop
  {
    sf::Event event;
    rpm_g = static_cast<int>(rpm);
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
    if(rand() % 10 == 1)
      std::cout << fps << "\n";
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
    for (unsigned i = 0; i < ai_cars.size(); ++i) // ais never slow down
    {
      auto& ai_car = *ai_cars[i];
      ai_car.throttle = 100;

      int index = 1 + i;
      int skill = ai_skill[index];
      int shift_timing = ai_car.model.rpm_autoshift_up - (100 - skill) * 25;

      if (ai_car.rpm > shift_timing) //p->model.rpm_autoshift_up)
        ai_car.gear_shift_up();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) // braking
    {
      current_car.brake(frame_time);
    }

    for (auto p : cars)
    {
      car& c = *p;
      
      c.update_acceleration();
      c.update_rpm(frame_time);
      c.update_speed(frame_time);

      if (c.rpm > c.model.rpm_max)
        c.rpm = c.model.rpm_max;
      if (c.rpm < c.model.rpm_min)
        c.rpm = c.model.rpm_min;
      double car_moved_dist = c.speed * frame_time;
      c.travel_distance += car_moved_dist;
    }

    rpm = current_car.rpm;
    window.clear(sf::Color(50, 50, 50));

    double rpm_factor = ((rpm - rpm_min) / double(rpm_max - rpm_min));

    double rev_size = rev_base_size * rpm_factor * global_scale_factor;
    // double rev_size = 300;
    rev_bar.setSize(sf::Vector2f(static_cast<float>(rev_size), static_cast<float>(23 * global_scale_factor)));

    rev_bar.setOutlineThickness(3);
    rev_bar.setOutlineColor(sf::Color::White);

    rev_bar.setFillColor(sf::Color(
      sf::Uint8(255 * rpm_factor), 
      sf::Uint8(255 - 255 * rpm_factor), 
      sf::Uint8(127 - 127 * rpm_factor)));

    rev_bar.setOrigin(0, 0);
    rev_bar.setPosition({ static_cast<float>(60 * global_scale_factor), static_cast<float>(50 * global_scale_factor) });

    rpm_text.setFont(font);
    rpm_text.setCharacterSize(static_cast<unsigned>(18 * global_scale_factor));
    rpm_text.setPosition(static_cast<float>(60 * global_scale_factor), static_cast<float>(46 * global_scale_factor));
    rpm_text.setFillColor(sf::Color::White);
    rpm_text.setString(std::to_string(current_car.rpm));

    gear_text.setFont(font);
    gear_text.setCharacterSize(static_cast<unsigned>(32 * global_scale_factor));
    gear_text.setPosition(static_cast<float>(32 * global_scale_factor), static_cast<float>(40 * global_scale_factor));
    gear_text.setFillColor(sf::Color::White);
    gear_text.setString(std::to_string(current_car.gear));

    speed_text.setFont(font);
    speed_text.setCharacterSize(static_cast<unsigned>(32 * global_scale_factor));
    speed_text.setPosition(static_cast<float>(rev_bar.getPoint(0).x + (rev_base_size + 80)*global_scale_factor), 
      static_cast<float>(40 * global_scale_factor));
    speed_text.setFillColor(sf::Color::White);
    speed_text.setString(std::to_string(int(current_car.speed)));


    sf::Text player_arrow;
    player_arrow.setFont(font);
    player_arrow.setCharacterSize(static_cast<unsigned>(36 * global_scale_factor));
    player_arrow.setPosition(static_cast<float>(vcars[0]->x_pos + 112 * global_scale_factor), static_cast<float>(280 * global_scale_factor));
    player_arrow.setFillColor(sf::Color::White);
    player_arrow.setOutlineColor(sf::Color::Black);
    player_arrow.setOutlineThickness(static_cast<float>(6 * global_scale_factor));
    player_arrow.setString(L'\u25BC');/// (L'\u2193');

    static double total_dist = 0;
    //  rev_bar.setPosition({ 100,400 });

    background.x_scale = static_cast<float>(global_scale_factor);
    background.y_scale = static_cast<float>(global_scale_factor);

    background.redraw(current_car.speed * frame_time);
   // total_dist += car_moved_dist;

    //vcars[0]->current_color = rev_bar.getFillColor();

    // std::cout << (int)total_dist << "\n";

    sf::Color car_colors[] = { sf::Color::Red, sf::Color::Blue, sf::Color::Green };
    int cars_pwned = 0;
    int current_position = 1;
    for (int i = 0; i < num_cars; ++i)
    {
      auto p_vcar = vcars[i];
      const car& physical_car = *(p_vcar->physical_car);
      // p_vcar->rescale();
      double speedms = physical_car.speed / 3.6;
      double wheel_rpm = physical_car.model.wheel_radius * 2 * 3.14 * speedms;
      double wheel_rotation_delta = 360.0 * frame_time * wheel_rpm * 0.3;

      double relative_pos = physical_car.travel_distance - current_car.travel_distance;

      if (i > 0)
      {
        if (relative_pos > 0)
          ++current_position;

        if (relative_pos < -100)
          ++cars_pwned;
       // std::cout << "car " << i << ": " << "rpm(" << physical_car.rpm << ") " << relative_pos << "m ahead!\n";
        const double pixel_dist_ratio = 5.5;
        p_vcar->x_pos = vcars[0]->x_pos + relative_pos * pixel_dist_ratio;
        p_vcar->rescale();

        if (p_vcar->x_pos > window.getSize().x)
        {
          game_over(font, global_scale_factor, window, false);
          finished = true;
        }
        if (cars_pwned == num_cars - 1)
        {
          game_over(font, global_scale_factor, window, true);
          finished = true;

        }
      }
      p_vcar->redraw(wheel_rotation_delta);
    }

    if (current_car.travel_distance >= 1609)
    {
      game_over(font, global_scale_factor, window, current_position == 1);
      finished = true;
    }

    window.draw(rev_bar);

    window.draw(gear_text);
    window.draw(rpm_text);
    window.draw(speed_text);
    window.draw(player_arrow);
    window.display();
  }
  sound_t.join();
  return 0;
}

void game_over(sf::Font &font, const double &global_scale_factor, sf::RenderWindow &window, bool win)
{
  sf::Text end_text;
  end_text.setFont(font);
  end_text.setCharacterSize(static_cast<unsigned>(128 * global_scale_factor));
  end_text.setPosition(170.f, static_cast<float>(window.getSize().y / 2 - 200));
  end_text.setFillColor(win ? sf::Color::Green : sf::Color::Red);
  //end_text.setScale({ 1 / 4.f, 1/4.f});
  end_text.setOutlineColor(sf::Color(220, 220, 220));
  end_text.setOutlineThickness(6);
  end_text.setString(win ? "YOU WIN!" : "YOU LOSE!");
  window.draw(end_text);
  window.display();
  rpm_g = -1;
  // sleep and handle events

  sf::Clock clocks_;

  for (;;)
  {

    sf::Event event;
    while (window.pollEvent(event))
    {
      // Window closed or escape key pressed: exit
      if ((event.type == sf::Event::Closed) ||
        ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
      {
        window.close();
        break;
      }
    }
    sf::sleep(sf::milliseconds(50));
    if (clocks_.getElapsedTime().asMilliseconds() > 60000)
    {
      break;
    }
  }
}
