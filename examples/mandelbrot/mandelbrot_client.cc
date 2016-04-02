#include <iostream>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "callme/client.h"
#include "mandelbrot.h"

int main() {
    const int width = 1024, height = 768;

    callme::client c("127.0.0.1", 8080);
    auto result = c.call("get_mandelbrot", width, height).as<pixel_data>();

    sf::Image image;
    image.create(width, height, sf::Color::Black);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            auto item = result[y * height + width];
            auto color = sf::Color(item.r, item.g, item.b, 255);
            image.setPixel(x, y, color);
        }
    }

    sf::Window window(sf::VideoMode(width, height), "callme mandelbrot client");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
