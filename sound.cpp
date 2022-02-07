#include <SFML/Audio.hpp>
#include <atomic>
std::atomic<bool> run_sound = true; 
extern std::atomic<int> rpm_g;

// play sound using default path and extension
void playsound(const std::string& name)
{
  sf::SoundBuffer f;
  f.loadFromFile("resources/" + name + ".wav");
  sf::Sound sound;
  sound.setBuffer(f);
  sound.play();
}


void sound_thread()
{
  sf::SoundBuffer buffer;
  // load something into the sound buffer...

  buffer.loadFromFile("resources\\truck_engine.wav");

  unsigned long long count = buffer.getSampleCount();
  const sf::Int16 * p = buffer.getSamples();

  std::vector<sf::Int16> samples;
  for (unsigned i = 0; i < count; ++i)
  {
    samples.push_back(p[i]);
  }

  // do the trimming
  int trimleft = static_cast<int>(5.5 * buffer.getSampleRate());
  int duration = static_cast<int>(1.0 * buffer.getSampleRate());
  sf::SoundBuffer buffer2;
  buffer2.loadFromSamples(samples.data() + trimleft, duration, buffer.getChannelCount(), buffer.getSampleRate());
  sf::Sound sound;
  sound.setBuffer(buffer2);
  while (run_sound)
  {
    if (rpm_g > 0)
    {
      sound.setPitch((rpm_g / 1500.f) / 3 + 1.25f);
      sound.setVolume(100 * rpm_g / 9000.f);
      sound.play();
      sf::sleep(sf::milliseconds(150));
    }
    else
    {
      sf::sleep(sf::milliseconds(10));
      break;
    }

  }
}
