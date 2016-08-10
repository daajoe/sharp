/*
 * File:   JsonOutput.cpp
 *
 * Author: FICHTE Johannes (fichte@dbai.tuwien.ac.at)
 *
 * Copyright 2016, Johannes K. Fichte
 *    E-Mail: <fichte@dbai.tuwien.ac.at>
 *
 * This file is part of htd.
 *
 * htd is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * htd is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

//typedef std::map<std::string, std::map<std::string, std::string>>::iterator it_type;

#include "sharp/JsonOutput.hpp"

#include <iostream>
#include <map>
#include <sstream>

#include <htd/PreOrderTreeTraversal.hpp>


namespace sharp {
    typedef std::map <std::string, std::map<std::string, std::string>> mapping;

    std::map <std::string, std::map<std::string, std::string>> JsonOutput::output;

    const std::string JsonOutput::EMPTY;

    const std::string JsonOutput::to_string(htd::ITreeDecomposition *td) {
        htd::PreOrderTreeTraversal traversal;
        std::ostringstream out;

        traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s) {
            out << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v);
        });

        return out.str();
    }

    void JsonOutput::data(const std::string &key, htd::ITreeDecomposition *td) {
        output[EMPTY].insert(std::make_pair(key, to_string(td)));
    }

    void JsonOutput::data(const std::string &key, const mpz_class &value) {
        this->data(key, value.get_str());
    }

    void JsonOutput::data(const std::string &key, double value) {
        this->data(key, std::to_string(value));
    }

    void JsonOutput::data(const std::string &key, const std::size_t value) {
        this->data(key, std::to_string(value));
    }

    void JsonOutput::data(const std::string &key, const std::string &value) {
        output[EMPTY].insert(std::make_pair(key, value));
    }

    void JsonOutput::data(const std::string &group, const std::string &key, double value) {
        output[group].insert(std::make_pair(key, std::to_string(value)));
    }
        void JsonOutput::data(const std::string &group, const std::string &key, const std::size_t value) {
        output[group].insert(std::make_pair(key, std::to_string(value)));
    }

    void JsonOutput::info(htd::ITreeDecomposition *td) {
        //TODO: change to htd format here, so that we can reimport the decomposition
        output[EMPTY].insert(std::make_pair("decomposition", to_string(td)));
    }

    void JsonOutput::info(const std::string &value) {
        std::cerr << "# " << value << std::endl;
    }

    void JsonOutput::info(const std::string &key, const std::size_t value) {
        data(key, value);
    }

    void JsonOutput::info(const std::string &key, const std::string &value) {
        data(key, value);
    }

    void JsonOutput::debug(const std::string &key, const std::size_t value) {
        std::cerr << "# " << key << " " << value << std::endl;
    }

    void JsonOutput::debug(const std::string &value, htd::ITreeDecomposition *td) {
        std::cerr << "# " << value << to_string(td) << std::endl;
    }

    void JsonOutput::debug(const std::string &value) {
        std::cerr << "# " << value << std::endl;
    }

    void JsonOutput::debug(const std::string &group, const std::string &key, const std::size_t value) {
        std::cerr << "# " << group << " " << key << " " << value << std::endl;
    }

    void JsonOutput::warning(const std::string &value) {
        std::cerr << "# ! " << value << std::endl;
    }

    void JsonOutput::error(const std::string &value) {
        std::cerr << "# !! " << value << std::endl;
    }

    void JsonOutput::exit(void) {
        std::cout << "{";
        auto last = output.end();
        --last;

        for (auto it = output.begin(); it != output.end(); ++it) {
            std::cout << "\"" << it->first << "\"" << ":";
            std::cout << "  {" << std::endl;
            auto last2 = it->second.end();
            --last2;

            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                std::cout << "  \"" << it2->first << "\"" << " : " << "\"" << it2->second << "\"";
                if (it2 != last2) {
                    std::cout << ",";
                }
                std::cout << std::endl;
            }
            std::cout << "  }";
            if (it != last) {
                std::cout << ",";
            }
            std::cout << std::endl;
        }
        std::cout << "}" << std::endl;
    }
}



