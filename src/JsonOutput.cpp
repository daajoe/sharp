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
#include <sstream>

#include <htd/PreOrderTreeTraversal.hpp>


namespace sharp {
    OutputMultiMap JsonOutput::output;

    const std::string JsonOutput::EMPTY;
    const std::string JsonOutput::PRE = "After_Preprocessing";

    void JsonOutput::data(const std::string &key, const std::size_t value) {
        output.add(EMPTY, key, value);
    }

    void JsonOutput::data(const std::string &key, const std::string &value) {
        output.add(EMPTY, key, value);
    }

    void JsonOutput::data(const std::string &key, const mpz_class &value) {
        output.add(EMPTY, key, value);
    }

    void JsonOutput::data(const std::string &key, htd::ITreeDecomposition *value) {
        output.add(EMPTY, key, value);
    }

    void JsonOutput::data(const std::string &group, const std::string &key, double value) {
        output.add(group, key, value);
    }

    void JsonOutput::data(const std::string &group, const std::string &key, const std::size_t value) {
        output.add(group, key, value);
    }


    void JsonOutput::data(const std::string &group, const std::string &key, htd::ITreeDecomposition *td) {
        output.add(group, key, td);
    }

    void JsonOutput::preproc_data(const std::string &key, htd::ITreeDecomposition *td) {
        output.add(PRE, key, td);
    }

    void JsonOutput::preproc_data(const std::string &key, const std::size_t value) {
        output.add(PRE, key, value);
    }

    void JsonOutput::info(htd::ITreeDecomposition *td) {
        //TODO: change to htd format here, so that we can reimport the decomposition
        output.add(EMPTY, "decomposition", td);
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

    void JsonOutput::debug(const std::string &value) {
        std::cerr << "# " << value << std::endl;
    }

    void JsonOutput::debug(const std::string &key, const std::size_t value) {
        std::cerr << "# " << key << " " << value << std::endl;
    }

    void JsonOutput::debug(const std::string &key, const std::string &value) {
        std::cerr << "# " << key << " " << value << std::endl;
    }

    void JsonOutput::debug(const std::string &value, htd::ITreeDecomposition *td) {
        std::cerr << "# " << value << to_string(td) << std::endl;
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

    const std::string JsonOutput::to_string(htd::ITreeDecomposition *td) {
        htd::PreOrderTreeTraversal traversal;
        std::ostringstream out;

        traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s) {
            out << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v);
        });

        return out.str();
    }

    void JsonOutput::print_value(OutputMultiMap::ValidType validType, unsigned long index) {
        //TODO: improve multitype handling here
        switch (validType) {
            case OutputMultiMap::ValidType::DOUBLE:
                std::cout << output.get_double(index);
                break;
            case OutputMultiMap::ValidType::MPZ:
                std::cout << output.get_mpz(index).get_str();
                break;
            case OutputMultiMap::ValidType::SIZE_T:
                std::cout << output.get_size_t(index);
                break;
            case OutputMultiMap::ValidType::STRING:
                std::cout << "\"" << output.get_string(index) << "\"";
                break;
            case OutputMultiMap::ValidType::TDECOMP:
                std::cout << "\"" << to_string(output.get_td(index)) << "\"";
                break;
        }
    }

    void JsonOutput::exit(void) {
        std::cout << "{";
        for (auto it = output.begin(); it != output.end(); ++it) {

            if (it->first != JsonOutput::EMPTY) {
                std::cout << "\"" << it->first << "\"" << ":";
                std::cout << "[";
            }

            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                if (it->first != JsonOutput::EMPTY) {
                    std::cout << "   [";
                    std::cout << "\"" << it2->first << "\", ";
                } else {
                    std::cout << "\"" << it2->first << "\": ";
                }
                this->print_value(it2->second.first, it2->second.second);

                if (it->first != JsonOutput::EMPTY) {
                    std::cout << "]";
                }
                if (it2 != --it->second.end()) {
                    std::cout << ",";
                }
                std::cout << std::endl;
            }
            if (it->first != JsonOutput::EMPTY) {
                std::cout << "]";
            }
            if (it != --output.end()) {
                std::cout << "," << std::endl;
            }

        }
        std::cout << "}" << std::endl;

    }

}



