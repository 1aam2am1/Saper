//
// Created by MacBook on 2019-06-05.
//

#include "Saper.h"
#include <random>
#include <stack>
#include <functional>

std::string convertInt(int32_t number) {
    char str[12] = {0};
    std::sprintf(str, "%i", number);
    return str;
}

Saper::Ptr Saper::create(tgui::Layout2d size) {
    auto widget = std::make_shared<Saper>();
    widget->setSize(size);
    return widget;
}

Saper::Saper() {
    cell.loadFromFile("assets/Cell.png");
    empty_cell.loadFromFile("assets/EmptyCell.png");
    flag.loadFromFile("assets/flag.png");
    mine.loadFromFile("assets/mine.png");

    text.setFont(*m_fontCached.getFont());
    text.setFillColor(sf::Color::Black);

    small_text.setFont(*m_fontCached.getFont());
    small_text.setFillColor(sf::Color::Black);
}

Saper::~Saper() {

}

void Saper::run(int size, int bomb_count) {
    game_over = false;

    if (size > 60) {
        size = 60;
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, size - 1);

    map.clear();
    map.resize(size);
    for (int i = 0; i < size; ++i) {
        map[i].resize(size);
    }

    if (bomb_count >= size * size) {
        bomb_count = size * size / 4;
    }

    for (int i = 0; i < bomb_count;) {
        int x = dist(mt);
        int y = dist(mt);
        if (!map[x][y].bomb) {
            map[x][y].bomb = true;
            map[x][y].bomb_count = 0;
            ++i;

            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 3; ++k) {
                    int next_x = j + x - 1;
                    int next_y = k + y - 1;
                    if (next_x >= 0 && next_y >= 0 && next_x < map.size() && next_y < map.size()) {
                        if (!map[next_x][next_y].bomb) {
                            map[next_x][next_y].bomb_count += 1;
                        }
                    }
                }
            }
        }
    }

}

void Saper::leftMousePressed(tgui::Vector2f pos) {
    double width = this->getSize().x;
    double cell_width = width / static_cast<double>(map.size());

    clicked_cell_x = pos.x / cell_width;
    clicked_cell_y = pos.y / cell_width;


    ClickableWidget::leftMousePressed(pos);
}

void Saper::leftMouseReleased(tgui::Vector2f pos) {
    if (game_over) { return; }
    double width = this->getSize().x;
    double cell_width = width / static_cast<double>(map.size());

    int x = pos.x / cell_width;
    int y = pos.y / cell_width;

    if (clicked_cell_x == x && clicked_cell_y == y) {
        leftMouseCell(x, y);
    }

    ClickableWidget::leftMouseReleased(pos);
}

void Saper::leftMouseCell(int x, int y) {
    if (map.size() > x && map.size() > y) {
        if (map[x][y].marked_as_bomb) { return; }
        if (map[x][y].bomb) {
            onGameOver.emit(this, "Game Over (LOSER!!!)");
            game_over = true;
        } else {
            /**Show adll not shown tiles*/
            std::stack<std::pair<int, int>> to_shown;

            to_shown.emplace(x, y);

            while (!to_shown.empty()) {
                auto tile = to_shown.top();
                to_shown.pop();

                map[tile.first][tile.second].shown = true;
                if (map[tile.first][tile.second].bomb_count != 0) { continue; }

                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        int next_x = i + tile.first - 1;
                        int next_y = j + tile.second - 1;
                        if (next_x >= 0 && next_y >= 0 && next_x < map.size() && next_y < map.size()) {
                            if (!map[next_x][next_y].shown) {
                                to_shown.emplace(next_x, next_y);
                            }
                        }
                    }
                }
            }

            /**Check if we have win*/
            bool win = true;
            for (int i = 0; i < map.size(); ++i) {
                for (int j = 0; j < map.size(); ++j) {
                    if (!(map[i][j].shown | map[i][j].bomb)) {
                        win = false;
                    }
                }
            }
            if (win) {
                game_over = true;
                onGameOver.emit(this, "Game Over (WINNER!!!)");
            }
        }

        this->updatehints();
    }
}

void Saper::rightMousePressed(tgui::Vector2f pos) {
    double width = this->getSize().x;
    double cell_width = width / static_cast<double>(map.size());

    clicked_cell_x = pos.x / cell_width;
    clicked_cell_y = pos.y / cell_width;


    Widget::rightMousePressed(pos);
}

void Saper::rightMouseReleased(tgui::Vector2f pos) {
    if (game_over) { return; }
    double width = this->getSize().x;
    double cell_width = width / static_cast<double>(map.size());

    int x = pos.x / cell_width;
    int y = pos.y / cell_width;

    if (clicked_cell_x == x && clicked_cell_y == y) {
        rightMouseCell(x, y);
    }

    Widget::rightMouseReleased(pos);

}

void Saper::rightMouseCell(int x, int y) {

    if (map.size() > x && map.size() > y) {
        map[x][y].marked_as_bomb = !map[x][y].marked_as_bomb;
    }

}


void Saper::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (map.empty()) { return; }

    double width = this->getSize().x;
    double cell_width = width / static_cast<double>(map.size());
    text.setCharacterSize(cell_width - 1);
    small_text.setCharacterSize(cell_width / 6);

    {
        sf::Sprite cell_s(cell);
        cell_s.scale(cell_width / cell.getSize().x, cell_width / cell.getSize().y);

        sf::Sprite em_cell_s(empty_cell);
        em_cell_s.scale(cell_width / empty_cell.getSize().x, cell_width / empty_cell.getSize().y);

        sf::Sprite flag_s(flag);
        flag_s.scale(cell_width / flag.getSize().x, cell_width / flag.getSize().y);

        sf::Sprite mine_s(mine);
        mine_s.scale(cell_width / mine.getSize().x, cell_width / mine.getSize().y);

        for (int i = 0; i < map.size(); ++i) {
            for (int j = 0; j < map[i].size(); ++j) {
                if (!map[i][j].shown || game_over) { ///not shown or game over
                    cell_s.setPosition(i * cell_width, j * cell_width);

                    target.draw(cell_s, states);

                    if (map[i][j].marked_as_bomb) {
                        flag_s.setPosition(i * cell_width, j * cell_width);

                        target.draw(flag_s, states);
                    }
                    if ((game_over || cheat) && map[i][j].bomb) {
                        mine_s.setPosition(i * cell_width, j * cell_width);

                        target.draw(mine_s, states);
                    }
                } else {  ///shown
                    em_cell_s.setPosition(i * cell_width, j * cell_width);

                    target.draw(em_cell_s, states);

                    if (map[i][j].bomb_count != 0) {
                        text.setString(convertInt(map[i][j].bomb_count));

                        text.setPosition(i * cell_width, j * cell_width);

                        target.draw(text, states);
                    }
                }

                if (hints && map[i][j].have_hints) {
                    small_text.setString(convertInt(map[i][j].bomb_probability));

                    small_text.setPosition(
                            i * cell_width + cell_width - cell_width / 10 - small_text.getLocalBounds().width,
                            j * cell_width);

                    target.draw(small_text, states);
                }
            }
        }
    }
}

tgui::Signal &Saper::getSignal(std::string signalName) {
    if (signalName == tgui::toLower(onGameOver.getName()))
        return onGameOver;
    else
        return ClickableWidget::getSignal(signalName);
}

void Saper::enable_hints(bool en) {
    hints = en;
}

void Saper::enable_bombs(bool en) {
    cheat = en;
}

void Saper::updatehints() {
    std::vector<std::vector<int>> copy_of_bombs;
    copy_of_bombs.resize(map.size());
    for (int i = 0; i < map.size(); ++i) {
        copy_of_bombs[i].resize(map.size());
    }


    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            map[i][j].have_hints = false;
            map[i][j].bomb_probability = 0;

            if (map[i][j].shown) { copy_of_bombs[i][j] = map[i][j].bomb_count; } else { copy_of_bombs[i][j] = 0; }
        }
    }

    auto check_how_many = [](int x, int y, const std::vector<std::vector<Tile>> &v) -> int {
        int result = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int next_x = i + x - 1;
                int next_y = j + y - 1;

                if (next_x >= 0 && next_y >= 0 && next_x < v.size() && next_y < v.size()) {
                    if ((next_x != x || next_y != y) && !v[next_x][next_y].shown && !v[next_x][next_y].have_hints) {
                        result += 1;
                        ///Niejestem soba
                        ///Jestem zakryty
                        ///Niemam podpowiedzi
                    }
                }
            }
        }
        return result;
    };

    auto get_not_shown = [](int x, int y, const std::vector<std::vector<Tile>> &v) -> std::pair<int, int> {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int next_x = i + x - 1;
                int next_y = j + y - 1;

                if (next_x >= 0 && next_y >= 0 && next_x < v.size() && next_y < v.size()) {
                    if (!v[next_x][next_y].shown && !v[next_x][next_y].have_hints) {
                        return {next_x, next_y};
                    }
                }
            }
        }
    };

    auto set_zero_propabibilty_around = [](int x, int y, std::vector<std::vector<Tile>> &v) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int next_x = i + x - 1;
                int next_y = j + y - 1;

                if (next_x >= 0 && next_y >= 0 && next_x < v.size() && next_y < v.size()) {
                    if (!v[next_x][next_y].have_hints && !v[next_x][next_y].shown) {///ukryte kafelki bez podpowiedzi
                        v[next_x][next_y].bomb_probability = 0;
                        v[next_x][next_y].have_hints = true;
                    }
                }
            }
        }
    };

    ///-1 do kafalkow naokolo
    auto remove_around = [&](int x, int y, std::vector<std::vector<int>> &v, std::vector<std::vector<Tile>> &v2) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int next_x = i + x - 1;
                int next_y = j + y - 1;

                if (next_x >= 0 && next_y >= 0 && next_x < v.size() && next_y < v.size()) {
                    if (v2[next_x][next_y].shown) { v[next_x][next_y] -= 1; }///-1 do ilosci bomb do okola
                    if (v[next_x][next_y] == 0 && v2[next_x][next_y].shown) { ///jezeli ustalilem ze niema bomb
                        set_zero_propabibilty_around(next_x, next_y, v2);///ustaw do okola 0%
                    }
                }
            }
        }
    };


    bool change = true;
    while (change) {
        change = false;
        for (int i = 0; i < map.size(); ++i) {
            for (int j = 0; j < map[i].size(); ++j) {
                if (copy_of_bombs[i][j] == 1) {///sprawdzam czy tylko jedna bomba do okola
                    if (check_how_many(i, j, map) == 1) {///sprawdzam ile niewidocznych kafelkow
                        auto punkt = get_not_shown(i, j, map);///pobieram adres kafelka

                        map[punkt.first][punkt.second].bomb_probability = 100;///ustawiam ze jest tam bomba
                        map[punkt.first][punkt.second].have_hints = true;    ///jest to podpowiedz

                        remove_around(punkt.first, punkt.second, copy_of_bombs,
                                      map);///-1 do ilosci bomb dookola naszj bomby
                        change = true;
                    }
                }
            }
        }
    }

}

void Saper::boot_turn() {
    if (game_over) { return; }
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, map.size() - 1);

    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            if (!map[i][j].shown && map[i][j].have_hints && map[i][j].bomb_probability == 0) {
                leftMouseCell(i, j);
                return;
            } else if (!map[i][j].shown && map[i][j].have_hints && map[i][j].bomb_probability == 100 &&
                       !map[i][j].marked_as_bomb) {
                rightMouseCell(i, j);
                return;
            }
        }
    }

    while (true) {
        int i = dist(mt);
        int j = dist(mt);

        if (!map[i][j].shown) {
            leftMouseCell(i, j);
            return;
        }
    }

}