#include <iostream>
#include <TGUI/TGUI.hpp>
#include "Saper.h"

void load(tgui::Gui &);

int32_t convertString(std::string str) {
    return atoi(str.c_str());
}

int main() {
    sf::RenderWindow window{{800, 600}, "Saper"};
    tgui::Gui gui{window}; // Create the gui and attach it to the window


    load(gui);


    while (window.isOpen()) {

        sf::Event event;
        sf::Clock clock;
        while (clock.getElapsedTime() < sf::seconds(1)) {
            bool update = false;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                gui.handleEvent(event); // Pass the event to the widgets
                update = true;
            }

            if (update) { break; }
            sf::sleep(sf::milliseconds(20));
        }

        window.clear(sf::Color::White);

        gui.draw(); // Draw all widgets
        window.display();
    }
}

void load(tgui::Gui &gui) {
    tgui::EditBox::Ptr bomb_count;
    tgui::EditBox::Ptr size;

    try {

        auto game = Saper::create({"70%", "100%"});
        gui.add(game, "game");

        game->connect("onGameOver", [&gui](std::string message) {
            auto box = tgui::MessageBox::create();

            box->setText(message);
            box->addButton("Ok");
            box->setPosition({"(&.width - width)/2", "(&.height - height)/2"});
            box->connect("ButtonPressed",
                         [box]() -> void { box->getParent()->remove(box); });

            gui.add(box);
        });

        auto p1 = tgui::Panel::create({"30%", "100%"});
        p1->getRenderer()->setBackgroundColor(sf::Color::Magenta);
        p1->setPosition({"game.right", "0"});
        gui.add(p1);

        {
            auto panel = tgui::Panel::create();

            auto text = tgui::Label::create("Ilosc bomb");
            panel->add(text);

            auto t1 = tgui::EditBox::create();
            t1->setSize(50, 20);
            t1->setPosition({"&.right - 60", bindTop(text)});
            t1->setInputValidator(tgui::EditBox::Validator::UInt);
            panel->add(t1);

            bomb_count = t1;

            p1->add(panel);
        }
        {
            auto panel = tgui::Panel::create();
            panel->setPosition("0", "25");

            auto text = tgui::Label::create("Pole gry (x, x)");
            panel->add(text);

            auto t1 = tgui::EditBox::create();
            t1->setSize(50, 20);
            t1->setPosition({"&.right - 60", bindTop(text)});
            t1->setInputValidator(tgui::EditBox::Validator::UInt);
            panel->add(t1);

            size = t1;

            p1->add(panel);
        }

        {
            auto button = tgui::Button::create("Start");
            button->setPosition("(&.width - width)/2", 50);
            button->connect("Pressed", [=, &gui]() {
                game->run(convertString(size->getText()), convertString(bomb_count->getText()));
            });

            p1->add(button);
        }

        {
            auto checkbox = tgui::CheckBox::create("Wlacz podopwiedzi");
            checkbox->setPosition(0, 75);
            checkbox->connect("Changed", [=]() {
                game->enable_hints(checkbox->isChecked());
            });

            p1->add(checkbox);
        }
        {
            auto checkbox = tgui::CheckBox::create("Cheat");
            checkbox->setPosition(0, 100);
            checkbox->connect("Changed", [=]() {
                game->enable_bombs(checkbox->isChecked());
            });

            p1->add(checkbox);
        }
        {
            auto button = tgui::Button::create("Computer turn");
            button->setPosition("(&.width - width)/2", 125);
            button->connect("Pressed", [=, &gui]() {
                game->boot_turn();
            });

            p1->add(button);
        }

    }
    catch (const tgui::Exception &e) {
        std::cerr << "TGUI Exception: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

}