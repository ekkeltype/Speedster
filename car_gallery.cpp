#include "visual_car.h"
#include "visual_bg.h"
#include <map>
#include <string>
#include <sstream>

#include "car.h"
extern std::map<std::string, car_model> model_data;
void populate_car_model_data();

sf::RectangleShape * get_rating_bar(double value, double best, double medium, 
  unsigned full_length = 400, bool greenIsGood = true)
{

  double rating = 0.01;
  double worst = medium + (medium - best);

  bool highIsBetter = (worst < medium);

  if (highIsBetter && value < worst)
    rating = 0.01;
  else if (!highIsBetter && value > worst)
    rating = 0.01;
  else
    rating = (value - worst) / (best - worst);

  if (rating > 1)
    rating = 1;


  sf::RectangleShape * rating_bar = new sf::RectangleShape();

  rating_bar->setSize(sf::Vector2f(full_length * rating, 28.f));

  rating_bar->setOutlineThickness(4);
  rating_bar->setOutlineColor(sf::Color(32,32,32));
  if(greenIsGood)
    rating_bar->setFillColor(sf::Color(255 - 255 * rating, 255 * rating, 127 - 127 * rating));
  else
    rating_bar->setFillColor(sf::Color(255 * rating, 255 - 255 * rating, 127 - 127 * rating));

  return rating_bar;
}

void print_mouse_move(const sf::Event& event)
{
  if (event.type != sf::Event::MouseMoved)
    return;

  std::cout << event.mouseMove.x << ", " << event.mouseMove.y << "\n";

}


void car_gallery(sf::RenderWindow& window)
{
  if (model_data.empty())
    populate_car_model_data();

  std::vector<car_model> models;
  for (auto& c : model_data)
  {
    models.push_back(c.second);
  }

  std::sort(models.rbegin(), models.rend(), [](car_model& a, car_model& b) { return b.price < a.price; });

  sf::Font font;
  if (!font.loadFromFile("C:\\windows\\fonts\\terminal.ttf"))
  {
    if (!font.loadFromFile("C:\\windows\\fonts\\arial.ttf"))
    {
      if (!font.loadFromFile("resources/sansation.ttf"))
        throw(std::runtime_error("font not found"));
    }
  }

  sf::Color bg_color = sf::Color::Green;
  window.clear(bg_color);
  visual_bg bg(window, "country", 3);
  visual_car car(window);
  const double global_scale_factor = 2.0;
  car.x_pos = (40 + 0 * 10) * global_scale_factor;
  car.y_pos = (365 + 0 * 10) * global_scale_factor;

  //car.base_scale = global_scale_factor;

  bg.redraw(0.0);
  car.rescale();
  car.redraw(0.);
  window.display();
  bool finished = false;

 // double car_pixels_per_frame = 10;

  sf::RectangleShape rf;
  rf.setSize({ window.getSize().x * 1.f, window.getSize().y / 3.f });
  rf.setPosition(495,88 );
  rf.setSize({ 1197 - 495, 416 - 88 });
  rf.setFillColor({ 215,215,215, 200 });
  rf.setOutlineColor({ 127,127,127 });
  rf.setOutlineThickness(6);

  auto it = models.cbegin();

  sf::Text car_name_text; 

  car_name_text.setFont(font);
  car_name_text.setCharacterSize(52);
  car_name_text.setPosition(window.getSize().x / 2.0 - 280, 90);
  car_name_text.setFillColor(sf::Color(234, 191, 21));
  car_name_text.setOutlineColor(sf::Color(16, 16, 16));
  car_name_text.setOutlineThickness(6);

  const int description_font_size = 36;

  sf::Text description_text;
  description_text.setFont(font);
  description_text.setCharacterSize(description_font_size);
  description_text.setPosition(window.getSize().x / 2.0 - 280, 190);
  description_text.setFillColor(sf::Color(220, 220, 220));
  description_text.setOutlineColor(sf::Color(16, 16, 16));
  description_text.setOutlineThickness(6);

  sf::Text index_text;
  index_text.setFont(font);
  index_text.setCharacterSize(description_font_size);
  index_text.setPosition(495, 430);
  index_text.setFillColor(sf::Color(234, 191, 21));
  index_text.setOutlineColor(sf::Color(16, 16, 16));
  index_text.setOutlineThickness(4);

  int car_index = 0;

  while (!finished)
  {
    const car_model& current_model = *it;
    sf::Event event;
    while (window.pollEvent(event))
    {
      print_mouse_move(event);
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
          if (key_code == sf::Keyboard::Right)
          {
            ++it;
            if (it == models.cend())
              it = models.cbegin();
          }
          if (key_code == sf::Keyboard::Left)
          {
            if (it != models.cbegin())
             --it;
          }
          car.current_color = { sf::Uint8(rand() % 255u), sf::Uint8(rand() % 255u), sf::Uint8(rand() % 255u) };
          car.recolor();
        }
    }
    
    car_index = std::distance(models.cbegin(), it) + 1;
    index_text.setString("Car " + std::to_string(car_index) + " of " + std::to_string(models.size()));

    std::ostringstream oss;
    oss << "Acceleration: \n";
    oss << "Top speed:    \n";
    oss << "Power:        \n\n";
    oss << "Price: $" << current_model.price;
    auto oss_str = oss.str();
    description_text.setString(oss_str);
    car_name_text.setString(current_model.name());

    double hpw = current_model.engine_bhp / current_model.weight;
    std::unique_ptr<sf::RectangleShape> accel_bar(get_rating_bar(hpw, 0.45, 0.22));
    accel_bar->setPosition(description_text.getPosition().x + 225, description_text.getPosition().y + 13);
    
    std::unique_ptr<sf::RectangleShape> speed_bar(get_rating_bar(current_model.top_speed, 350, 250));
    speed_bar->setPosition(description_text.getPosition().x + 225, description_text.getPosition().y + 13 + description_font_size *1.1);
    
    std::unique_ptr<sf::RectangleShape> power_bar(get_rating_bar(current_model.engine_bhp, 700, 350));
    power_bar->setPosition(description_text.getPosition().x + 225, description_text.getPosition().y + 13 + description_font_size *2.2);

    //car.x_pos += car_pixels_per_frame;
    //if (car.x_pos > window.getSize().x)
    //  car.x_pos = 0;
    sf::sleep(sf::milliseconds(10));
    window.clear(bg_color);
    bg.redraw(0.1);
   // window.draw(rf);

    car.rescale();
    car.redraw(1.0);
    window.draw(rf);
    window.draw(car_name_text);
    window.draw(description_text);
    window.draw(index_text);
    window.draw(*accel_bar);
    window.draw(*speed_bar);
    window.draw(*power_bar);
    window.display();
  }
}
