#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <array>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace config {
const sf::Color BG_COLOR(40, 40, 40);
const sf::Color FG_COLOR(5, 100, 255);
const sf::Color OUTLINE_COLOR(0,0,0);
const float CELL_SIZE = 30.f;
const float GAP = CELL_SIZE / 5.f;
const float LINE_THICKNESS = 5.f;
const unsigned int GRID_WIDTH = 28;
const unsigned int GRID_HEIGHT = 6;
const unsigned int CLOCK_WIDTH = GRID_WIDTH * (CELL_SIZE + GAP);
const unsigned int CLOCK_HEIGHT = GRID_HEIGHT * (CELL_SIZE + GAP);
const int PADDING = 10.f;
} // namespace config

namespace pattern {
std::map<std::string, std::array<float, 2>> CHAR_TO_ANGLES = {
    {"┘", {180.f, 270.f}}, {"-", {0.f, 180.f}},  {"L", {0.f, 270.f}},
    {"|", {90.f, 270.f}},  {"┐", {90.f, 180.f}}, {"┌", {0.f, 90.f}},
    {".", {135.f, 135.f}}};

using PatternGrid4 = std::array<std::array<std::string, 4>, 6>;
using PatternGrid2 = std::array<std::array<std::string, 2>, 6>;

const std::array<PatternGrid4, 10> DIGITS = {{
    // 0
    {{{"┌", "-", "-", "┐"},
      {"|", "┌", "┐", "|"},
      {"|", "|", "|", "|"},
      {"|", "|", "|", "|"},
      {"|", "L", "┘", "|"},
      {"L", "-", "-", "┘"}}},
    // 1
    {{{"┌", "-", "┐", "."},
      {"L", "┐", "|", "."},
      {".", "|", "|", "."},
      {".", "|", "|", "."},
      {"┌", "┘", "L", "┐"},
      {"L", "-", "-", "┘"}}},
    // 2
    {{{"┌", "-", "-", "┐"},
      {"L", "-", "┐", "|"},
      {"┌", "-", "┘", "|"},
      {"|", "┌", "-", "┘"},
      {"|", "L", "-", "┐"},
      {"L", "-", "-", "┘"}}},
    // 3
    {{{"┌", "-", "-", "┐"},
      {"L", "-", "┐", "|"},
      {"┌", "-", "┘", "|"},
      {"L", "-", "┐", "|"},
      {"┌", "-", "┘", "|"},
      {"L", "-", "-", "┘"}}},
    // 4
    {{{"┌", "┐", "┌", "┐"},
      {"|", "|", "|", "|"},
      {"|", "L", "┘", "|"},
      {"L", "-", "┐", "|"},
      {".", ".", "|", "|"},
      {".", ".", "L", "┘"}}},
    // 5
    {{{"┌", "-", "-", "┐"},
      {"|", "┌", "-", "┘"},
      {"|", "L", "-", "┐"},
      {"L", "-", "┐", "|"},
      {"┌", "-", "┘", "|"},
      {"L", "-", "-", "┘"}}},
    // 6
    {{{"┌", "-", "-", "┐"},
      {"|", "┌", "-", "┘"},
      {"|", "L", "-", "┐"},
      {"|", "┌", "┐", "|"},
      {"|", "L", "┘", "|"},
      {"L", "-", "-", "┘"}}},
    // 7
    {{{"┌", "-", "-", "┐"},
      {"L", "-", "┐", "|"},
      {".", ".", "|", "|"},
      {".", ".", "|", "|"},
      {".", ".", "|", "|"},
      {".", ".", "L", "┘"}}},
    // 8
    {{{"┌", "-", "-", "┐"},
      {"|", "┌", "┐", "|"},
      {"|", "L", "┘", "|"},
      {"|", "┌", "┐", "|"},
      {"|", "L", "┘", "|"},
      {"L", "-", "-", "┘"}}},
    // 9
    {{{"┌", "-", "-", "┐"},
      {"|", "┌", "┐", "|"},
      {"|", "L", "┘", "|"},
      {"L", "-", "┐", "|"},
      {"┌", "-", "┘", "|"},
      {"L", "-", "-", "┘"}}},
}};

const PatternGrid2 COLON = {
    {{".", "."}, {"┌", "┐"}, {"L", "┘"}, {"┌", "┐"}, {"L", "┘"}, {".", "."}}};
} // namespace pattern

class Unit {
private:
  sf::RectangleShape box;
  sf::CircleShape c;
  sf::RectangleShape line1;
  sf::RectangleShape line2;

public:
  Unit() {
    box.setSize({config::CELL_SIZE, config::CELL_SIZE});
    box.setFillColor(config::BG_COLOR);
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color::Black);

    c.setRadius(config::CELL_SIZE / 2);
    c.setOutlineThickness(2.f);
    c.setOutlineColor(config::OUTLINE_COLOR);
    c.setFillColor(config::BG_COLOR);

    line1.setSize({config::CELL_SIZE / 2, config::LINE_THICKNESS});
    line1.setFillColor(config::FG_COLOR);
    line1.setOrigin({0.f, config::LINE_THICKNESS / 2});

    line2.setSize({config::CELL_SIZE / 2, config::LINE_THICKNESS});
    line2.setFillColor(config::FG_COLOR);
    line2.setOrigin({0.f, config::LINE_THICKNESS / 2});
  }

  void setPosition(const sf::Vector2f &pos) {
    box.setPosition(pos);
    c.setPosition(pos);
    sf::Vector2f center =
        pos + sf::Vector2f({config::CELL_SIZE / 2, config::CELL_SIZE / 2});
    line1.setPosition(center);
    line2.setPosition(center);
  }

  void setPattern(std::string patternChar) {
    auto &angle = pattern::CHAR_TO_ANGLES.at(patternChar);
    line1.setRotation(sf::degrees(angle[0]));
    line2.setRotation(sf::degrees(angle[1]));
  }

  void draw(sf::RenderTarget &target) const {
    target.draw(box);
    // target.draw(c);
    target.draw(line1);
    target.draw(line2);
  }
};

class Separator {
private:
  std::array<std::array<Unit, 2>, 6> units;

public:
  Separator(const sf::Vector2f &origin) {
    for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 2; col++) {
        float boxPosX = col * (config::CELL_SIZE + config::GAP) + origin.x;
        float boxPosY = row * (config::CELL_SIZE + config::GAP) + origin.y;

        units[row][col].setPosition({boxPosX, boxPosY});
        units[row][col].setPattern(pattern::COLON[row][col]);
      }
    }
  }

  void draw(sf::RenderTarget &target) const {
    for (const auto &row : units) {
      for (const auto &unit : row) {
        unit.draw(target);
      }
    }
  }
};

class Digit {
private:
  std::array<std::array<Unit, 4>, 6> units;

public:
  Digit(const sf::Vector2f &origin) {
    for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 4; col++) {
        float boxPosX = col * (config::CELL_SIZE + config::GAP) + origin.x;
        float boxPosY = row * (config::CELL_SIZE + config::GAP) + origin.y;

        units[row][col].setPosition({boxPosX, boxPosY});
      }
    }
  }

  void setNumber(int number) {
    const auto &pattern = pattern::DIGITS[number];
    for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 4; col++) {
        units[row][col].setPattern(pattern[row][col]);
      }
    }
  }

  void draw(sf::RenderTarget &target) const {
    for (const auto &row : units) {
      for (const auto &unit : row) {
        unit.draw(target);
      }
    }
  }
};

class Clock {
private:
  Digit hh;
  Digit h;
  Separator s1;
  Digit mm;
  Digit m;
  Separator s2;
  Digit ss;
  Digit s;

  std::time_t now;
  struct std::tm *tm_now;
  int hour, min, sec;

public:
  Clock(const sf::Vector2f &origin)
      : hh({origin.x, origin.y}),
        h({origin.x + 4 * (config::CELL_SIZE + config::GAP), origin.y}),
        s1({origin.x + 8 * (config::CELL_SIZE + config::GAP), origin.y}),
        mm({origin.x + 10 * (config::CELL_SIZE + config::GAP), origin.y}),
        m({origin.x + 14 * (config::CELL_SIZE + config::GAP), origin.y}),
        s2({origin.x + 18 * (config::CELL_SIZE + config::GAP), origin.y}),
        ss({origin.x + 20 * (config::CELL_SIZE + config::GAP), origin.y}),
        s({origin.x + 24 * (config::CELL_SIZE + config::GAP), origin.y}) {}

  void update() {
    now = time(NULL);
    tm_now = localtime(&now);
    hour = tm_now->tm_hour % 12;
    min = tm_now->tm_min;
    sec = tm_now->tm_sec;

    h.setNumber(hour % 10);
    hh.setNumber(hour / 10);
    m.setNumber(min % 10);
    mm.setNumber(min / 10);
    s.setNumber(sec % 10);
    ss.setNumber(sec / 10);
  }

  void draw(sf::RenderTarget &target) const {
    h.draw(target);
    hh.draw(target);
    s1.draw(target);
    m.draw(target);
    mm.draw(target);
    s2.draw(target);
    s.draw(target);
    ss.draw(target);
  }
};

int main() {
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;

  unsigned int windowHeight = config::CLOCK_HEIGHT + 2 * config::PADDING;
  unsigned int windowWidth = config::CLOCK_WIDTH + 2 * config::PADDING;

  sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Clock",
                          sf::Style::Resize, sf::State::Windowed, settings);
  window.setFramerateLimit(30);
  window.setMinimumSize(window.getSize());



  // Game Loop
  while (window.isOpen()) {
  Clock clock({(window.getSize().x - config::CLOCK_WIDTH)/ 2.f, (window.getSize().y - config::CLOCK_HEIGHT) / 2.f});

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (const auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keypressed->scancode == sf::Keyboard::Scan::Escape) {
          window.close();
        }
      }

      if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        sf::FloatRect visible({0, 0}, {(float)resized->size.x, (float)resized->size.y});
        window.setView(sf::View(visible));
      }
    }


    window.clear(config::BG_COLOR);
    clock.update();
    clock.draw(window);
    window.display();
  }

  return 0;
}
