//
// Created by MacBook on 2019-06-05.
//

#ifndef SAPER_SAPER_H
#define SAPER_SAPER_H

#include <TGUI/TGUI.hpp>
#include <vector>


class Saper : public tgui::ClickableWidget {
public:
    typedef std::shared_ptr<Saper> Ptr; ///< Shared widget pointer
    typedef std::shared_ptr<const Saper> ConstPtr; ///< Shared constant widget pointer

    static Saper::Ptr create(tgui::Layout2d size = {"100%", "100%"});

    Saper();

    virtual ~Saper();

    void run(int size, int bomb_count);

    void enable_hints(bool en = false);

    void enable_bombs(bool en = false);

    void boot_turn();

    void leftMousePressed(tgui::Vector2f pos) override;

    void leftMouseReleased(tgui::Vector2f pos) override;

    void leftMouseCell(int x, int y);

    void rightMousePressed(tgui::Vector2f pos) override;

    void rightMouseReleased(tgui::Vector2f pos) override;

    void rightMouseCell(int x, int y);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

protected:
    tgui::Signal &getSignal(std::string signalName) override;

public:

    tgui::SignalString onGameOver = {"onGameOver"};

private:
    struct Tile {
        bool bomb = false;
        int bomb_count = 0;

        int marked_as_bomb = false;
        bool shown = false;

        int bomb_probability = 0;
        bool have_hints = false;
    };

    std::vector<std::vector<Tile>> map;

    mutable sf::Text text;
    mutable sf::Text small_text;
    sf::Texture cell;
    sf::Texture empty_cell;
    sf::Texture flag;
    sf::Texture mine;

    bool game_over = false;
    bool hints = false;
    bool cheat = false;
    int clicked_cell_x;
    int clicked_cell_y;

    void updatehints();

};


#endif //SAPER_SAPER_H
