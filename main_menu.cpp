#include <SFML/Audio.hpp>

#include "menu.h"
#include "visual_bg.h"
#include <iostream>

void playsound(const std::string& name);
void car_gallery(sf::RenderWindow& window);
void show_menu(game_menu& menu, sf::RenderWindow& window, visual_bg& bg);
void main_menu(sf::RenderWindow& window)
{
  visual_bg background(window, "cyberpunk", 3);
  background.redraw(0.0);

  game_menu menu("Speedster: Redline");
  menu.add_item("New game");
  menu.add_item("Load game");
  menu.add_item("Exit");

  game_menu new_game_menu("Main menu");
  new_game_menu.add_item("Buy car");
  new_game_menu.add_item("Sell car");
  new_game_menu.add_item("Workshop");
  new_game_menu.add_item("Time trial");
  new_game_menu.add_item("Race");
  new_game_menu.add_item("Finances");

  menu.add_submenu("New game", new_game_menu);

  show_menu(menu, window, background);
}

void handle_choice(std::tuple<std::string, int, game_menu> choice, sf::RenderWindow& window, visual_bg& bg)
{
  std::string menu_choice = std::get<0>(choice);
  auto submenu = std::get<2>(choice);
  if (!submenu.empty())
  {
    playsound("camera");
    show_menu(submenu, window, bg);
    return;
  }

  if(menu_choice == "Buy car")
  {
    car_gallery(window);
  }
  if (menu_choice == "Exit")
  {
    exit(0);
  }
}

void show_menu(game_menu& menu, sf::RenderWindow& window, visual_bg& bg)
{
  bool finished = false;

  menu.draw(window);
  window.display();

  int old_y = 0;
  sf::Clock clk;
  const float bg_scroll_speed = 0.1f;

  while (!finished) // main menu loop
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::MouseMoved && event.mouseMove.y != old_y)
      {
        bool redraw = menu.select_from_mouse_coord(event.mouseMove.x, event.mouseMove.y);
        if (redraw)
        {
          std::cout << "redraw (mouse move)\n";
        }
        old_y = event.mouseMove.y;
      }
      else if (event.type == sf::Event::MouseButtonReleased)
      {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
          playsound("arrow");
          return;
        }
        else if (event.mouseButton.button == sf::Mouse::Left)
        {
          auto choice = menu.select();
          handle_choice(choice, window, bg);
        }
      }

      // Window closed or escape key pressed: exit
      if (event.type == sf::Event::Closed)
      {
        window.close();
        finished = true;
        return;
      }
      else if (event.type == sf::Event::KeyPressed)
      {
        auto key_code = event.key.code;
        if (key_code == sf::Keyboard::Up)
        {
          menu.move_up();
        }
        if (key_code == sf::Keyboard::Down)
        {
          menu.move_down();
        }
        if (key_code == sf::Keyboard::Enter)
        {
          auto choice = menu.select();
          handle_choice(choice, window, bg);
        }
        if (key_code == sf::Keyboard::Escape)
        {
          playsound("arrow");
          return;
        }

        std::cout << "redraw (keypressed)\n";
      }
    }
    bg.redraw(bg_scroll_speed);
    menu.draw(window);
    window.display();
    sf::sleep(sf::milliseconds(20));
  }
}
