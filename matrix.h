//
// Created by tom on 12/28/19.
//

#ifndef PROTOLINK_MATRIX_H
#define PROTOLINK_MATRIX_H

#include <SFML/Graphics.hpp>
#include <iostream>

class Matrix {
private:
    std::vector<std::vector<bool>> fb;
    //bool fb[w][h];

    // position to start drawing
    const float x;
    const float y;

    bool mirror;

    void drawCircle(sf::RenderWindow& window, int i, int p) {
        sf::CircleShape circle(5.f);
        circle.setPosition(x + (float)i * 15.f, y + (float)p * 15.f);

        if (fb.at(i).at(p)) {
            circle.setFillColor(sf::Color(255, 255, 255));
        } else {
            circle.setFillColor(sf::Color(40, 40, 40));
        }

        window.draw(circle);
    }

public:
    const int w;
    const int h;

    Matrix(int w, int h, float pos_x, float pos_y, bool mirrored)
    : x(pos_x), y(pos_y), w(w), h(h){
        mirror = mirrored;

        for (int i = 0; i < w; i++) {
            std::vector<bool> row;
            for (int p = 0; p < h; p++) {
                row.push_back(false);
            }
            fb.push_back(row);
        }
    }

    void render(sf::RenderWindow& window) {
        for (int p = 0; p < h; p++) {
            if (mirror) {
                for (int i = w - 1; i >= 0; i--) {
                    drawCircle(window, i, p);
                }
            } else {
                for (int i = 0; i < w; i++) {
                    drawCircle(window, i, p);
                }
            }
        }
    }

    void drawPixel(int x, int y, bool state) {
        fb.at(x).at(y) = state;
    }
};


#endif //PROTOLINK_MATRIX_H
