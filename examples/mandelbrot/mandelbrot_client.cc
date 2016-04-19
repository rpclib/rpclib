#include <iostream>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "callme/client.h"
#include "mandelbrot.h"

int main() {
    const int width = 1024, height = 768;

    callme::client c("127.0.0.1", 8080);
    auto result = c.call("get_mandelbrot", width, height).as<pixel_data>();
    LOG_INFO("Got data");

    sf::Image image;
    image.create(width, height, sf::Color::Black);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            auto item = result[x * height + y];
            auto color = sf::Color(item.r, item.g, item.b, 255);
            image.setPixel(x, y, color);
        }
    }

    sf::RenderWindow window(sf::VideoMode(width, height), "callme mandelbrot client");

    sf::Texture texture;
    texture.loadFromImage(image, sf::IntRect(0, 0, width, height));
    sf::Sprite sprite;
    sprite.setTexture(texture, true);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.draw(sprite);
        window.display();
    }

    return 0;
}
