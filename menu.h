#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics.hpp>

class game_menu
{


public:
  game_menu() : game_menu(std::string())
  {
  }
  game_menu(const std::string& headline) : headline(headline)
  {

    if (!font.loadFromFile("C:\\windows\\fonts\\terminal.ttf"))
    {
      if (!font.loadFromFile("C:\\windows\\fonts\\arial.ttf"))
      {

        if (!font.loadFromFile("resources/sansation.ttf"))
          throw(std::runtime_error("font not found"));
      }
    }
  }

  void add_item(const std::string& item)
  {
    menu_items.push_back(item);
  }
  void add_submenu(const std::string& item, const game_menu& submenu)
  {
    sub_menus[item] = submenu;
  }

  void draw(sf::RenderTarget& window)
  {
    int idx = 0;
    auto window_dimensions = window.getSize();
    // est. space per menu item
    float initial_y = (window_dimensions.y / 2.f) - (menu_items.size() / 2.f) * font_size;
    float initial_x = window_dimensions.x / 2.f - 100;
    menu_item_coord.clear();

    // draw headline
    if (headline_text == nullptr)
    {
      headline_text = new sf::Text();
      headline_text->setFont(font);
      headline_text->setCharacterSize(static_cast<int>(font_size*1.25));
      //    item_text.setPosition(170, window.getSize().y / 2 - 200);
      headline_text->setFillColor(sf::Color(220, 220, 220));
      //end_text.setScale({ 1 / 4.f, 1/4.f});
      headline_text->setOutlineColor(sf::Color(16, 16, 16));
      headline_text->setOutlineThickness(6);
      headline_text->setString(headline);
    }
    int y = static_cast<int>(initial_y - (1 * font_size * spacing));
    headline_text->setPosition(initial_x, static_cast<float>(y));
    window.draw(*headline_text);
    for (auto item : menu_items)
    {
      sf::Text& text = get_item_text(item, idx);  
      if (idx == current_item) // selected item
      {
        text.setFillColor(sf::Color::Yellow);
      }
      else
      {
        text.setFillColor(sf::Color::White);
      }
      ++idx;
      float y = initial_y + static_cast<float>(idx * font_size * spacing);
      text.setPosition( initial_x, y);
      menu_item_coord.push_back(static_cast<int>(y));
      window.draw(text);
    }

  }
  // select menu index based on mouse coord (return true if changed)
  bool select_from_mouse_coord(int x, int y)
  {
    int old_current_item = current_item;

    for (unsigned idx = 0; idx < menu_item_coord.size(); ++idx)
    {
      double dist = std::fabs(y - menu_item_coord[idx]);
      if (dist < font_size) // y-coord found, check x
      {
        sf::Text * cand = menu_item_text[menu_items[idx]];
        sf::FloatRect text_rect = cand->getGlobalBounds();
        if (text_rect.contains({ float(x),float(y) }))
        {
          current_item = idx;
          break;
        }
      }
    }
    return current_item != old_current_item;
  }

  void move_up()
  {
    --current_item;
    if (current_item < 0)
      current_item += menu_items.size();
  }

  void move_down()
  {
    ++current_item;
    if (current_item >= static_cast<int>(menu_items.size()))
      current_item = 0;
  }
  // returns string, index, and submenu (if any) of the currently selected menu item
  std::tuple<std::string, int, game_menu> select() const
  {
    if (current_item < 0 || current_item >= static_cast<int>(menu_items.size()))
    {
      return {};
    }
    std::string itemname = menu_items[current_item];
    game_menu submenu;
    auto find = sub_menus.find(itemname);
    if (find != sub_menus.end())
    {
      submenu = find->second;
    }

    return { itemname, current_item, submenu };
  }

  int size() const
  {
    return menu_items.size();
  }

  bool empty() const
  {
    return size() == 0;
  }

private:
  std::string headline;
  int font_size = 48;
  double spacing = 1.2;
  sf::Font font;

  sf::Text& get_item_text(const std::string& str, int pos)
  {
    if (menu_item_text.count(str) > 0)
      return *menu_item_text[str];

    sf::Text& item_text = *(new sf::Text());
    item_text.setFont(font);
    item_text.setCharacterSize(font_size);
//    item_text.setPosition(170, window.getSize().y / 2 - 200);
    item_text.setFillColor(sf::Color(220, 220, 220));
    //end_text.setScale({ 1 / 4.f, 1/4.f});
    item_text.setOutlineColor(sf::Color(16, 16, 16));
    item_text.setOutlineThickness(6);
    item_text.setString(str);
    menu_item_text[str] = &item_text;
    return item_text;
  }

  sf::Text * headline_text;

  std::vector<std::string> menu_items;
  int current_item = 0;
  std::map<std::string, sf::Text*> menu_item_text;
  std::map<std::string, game_menu> sub_menus;
  std::vector<int> menu_item_coord;

};

