#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>


sf::RenderWindow * window_handle = nullptr;

sf::RectangleShape rev_bar;

int r = 0;
int g = 100;
int b = 100;

int rpm = 0;
constexpr int rpm_max = 9000;
constexpr int rpm_min = 1000;

sf::Text rpm_text;
sf::Text fps_text;
constexpr double rev_base_size = 600;
constexpr int revs_per_sec = 16000;
sf::Clock sf_clock;

void mainloop()
{
  if (window_handle == nullptr)
    return;
  auto& window = *window_handle;

  sf::Event event;
  while (window.pollEvent(event))
  {
      // Window closed or escape key pressed: exit
      if ((event.type == sf::Event::Closed) ||
          ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
      {
          window.close();
          return;
      }

  }

  float delta = sf_clock.restart().asSeconds();
  int fps = (int)(1.0 / delta);
  fps_text.setString(std::to_string(fps));

  double rev_speed = delta * revs_per_sec;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
  {
    rpm += rev_speed;
    if (rpm > rpm_max)
      rpm = rpm_max;
  }
  else
  {
    rpm -= rev_speed/2;
    if (rpm < rpm_min)
      rpm = rpm_min;
  }

  double rev_size = rev_base_size * ((rpm - rpm_min) / double(rpm_max - rpm_min));
  // double rev_size = 300;
  rev_bar.setSize(sf::Vector2f(rev_size, 60));
  rpm_text.setString(std::to_string(rpm));
  window.clear(sf::Color(50, 50, 50));
  window.draw(rpm_text);
  window.draw(fps_text);
  window.draw(rev_bar);
  window.display();
}


int main_()
{
  sf::RenderWindow window(sf::VideoMode(800, 600, 32), "EngineRevTest",
    sf::Style::Titlebar | sf::Style::Close);

  window.setVerticalSyncEnabled(true);
  window_handle = &window;

  sf::Font font;
  if (!font.loadFromFile("resources/sansation.ttf"))
    return EXIT_FAILURE;

  rev_bar.setSize(sf::Vector2f(rev_base_size, 40));
  rev_bar.setOutlineThickness(3);
  rev_bar.setOutlineColor(sf::Color::White);

  rev_bar.setFillColor(sf::Color(r, g, b));
  rev_bar.setOrigin(0, 0);
  rev_bar.setPosition({ 100,400 });

  rpm_text.setFont(font);
  rpm_text.setCharacterSize(40);
  rpm_text.setPosition(170.f, 150.f);
  rpm_text.setFillColor(sf::Color::White);
  rpm_text.setString("0");

  fps_text.setFont(font);
  fps_text.setCharacterSize(32);
  fps_text.setPosition(50.f, 50.f);
  fps_text.setFillColor(sf::Color::Green);
  fps_text.setString("0");



  while (window.isOpen())
  {
    mainloop();
  }



}