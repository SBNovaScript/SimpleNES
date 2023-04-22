#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "Controller.h"
#include "Log.h"

namespace sn
{
    // Trim from both start and end (construct new string)
    inline std::string trim(const std::string &str)
    {
        std::string s(str);
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
        return s;
    }

    void parseControllerConf(const std::string& filepath,
                            std::vector<sf::Keyboard::Key>& p1,
                            std::vector<sf::Keyboard::Key>& p2)
    {
        const std::string buttonStrings[] = { "A",
                                            "B",
                                            "Select",
                                            "Start",
                                            "Up",
                                            "Down",
                                            "Left",
                                            "Right" };
        const std::string keys[] = {"A",
                                    "B",
                                    "C",
                                    "D",
                                    "E",
                                    "F",
                                    "G",
                                    "H",
                                    "I",
                                    "J",
                                    "K",
                                    "L",
                                    "M",
                                    "N",
                                    "O",
                                    "P",
                                    "Q",
                                    "R",
                                    "S",
                                    "T",
                                    "U",
                                    "V",
                                    "W",
                                    "X",
                                    "Y",
                                    "Z",
                                    "Num0",
                                    "Num1",
                                    "Num2",
                                    "Num3",
                                    "Num4",
                                    "Num5",
                                    "Num6",
                                    "Num7",
                                    "Num8",
                                    "Num9",
                                    "Escape",
                                    "LControl",
                                    "LShift",
                                    "LAlt",
                                    "LSystem",
                                    "RControl",
                                    "RShift",
                                    "RAlt",
                                    "RSystem",
                                    "Menu",
                                    "LBracket",
                                    "RBracket",
                                    "SemiColon",
                                    "Comma",
                                    "Period",
                                    "Quote",
                                    "Slash",
                                    "BackSlash",
                                    "Tilde",
                                    "Equal",
                                    "Dash",
                                    "Space",
                                    "Return",
                                    "BackSpace",
                                    "Tab",
                                    "PageUp",
                                    "PageDown",
                                    "End",
                                    "Home",
                                    "Insert",
                                    "Delete",
                                    "Add",
                                    "Subtract",
                                    "Multiply",
                                    "Divide",
                                    "Left",
                                    "Right",
                                    "Up",
                                    "Down",
                                    "Numpad0",
                                    "Numpad1",
                                    "Numpad2",
                                    "Numpad3",
                                    "Numpad4",
                                    "Numpad5",
                                    "Numpad6",
                                    "Numpad7",
                                    "Numpad8",
                                    "Numpad9",
                                    "F1",
                                    "F2",
                                    "F3",
                                    "F4",
                                    "F5",
                                    "F6",
                                    "F7",
                                    "F8",
                                    "F9",
                                    "F10",
                                    "F11",
                                    "F12",
                                    "F13",
                                    "F14",
                                    "F15",
                                    "Pause"
                                };
std::ifstream file(filepath);
        std::string line;
        enum { Player1, Player2, None } state = None;
        unsigned int line_no = 0;
        while (std::getline(file, line))
        {
            line = trim(line);
            if (line.empty() || line[0] == '#')
                continue;

            if (line == "[Player1]")
                state = Player1;
            else if (line == "[Player2]")
                state = Player2;
            else if (state == Player1 || state == Player2)
            {
                auto divider = line.find("=");
                auto button = trim(line.substr(0, divider));
                auto key = trim(line.substr(divider + 1));

                auto buttonIt = std::find(std::begin(buttonStrings), std::end(buttonStrings), button);
                auto keyIt = std::find(std::begin(keys), std::end(keys), key);

                if (buttonIt == std::end(buttonStrings) || keyIt == std::end(keys))
                {
                    LOG(Error) << "Invalid key in configuration file at Line " << line_no << std::endl;
                    continue;
                }

                auto i = std::distance(std::begin(buttonStrings), buttonIt);
                auto keyIndex = std::distance(std::begin(keys), keyIt);
                (state == Player1 ? p1 : p2)[i] = static_cast<sf::Keyboard::Key>(keyIndex);
            }
            else
                LOG(Error) << "Invalid line in key configuration at Line " << line_no << std::endl;

            ++line_no;
        }
    }
}
