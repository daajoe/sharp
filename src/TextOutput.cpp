/*
 * File:   TextOutput.cpp
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

#include "sharp/TextOutput.hpp"

#include <iostream>
#include <sstream>

#include <htd/PreOrderTreeTraversal.hpp>

namespace sharp {
    void TextOutput::decomposition(const std::string &key, htd::ITreeDecomposition *td) {
        std::cout << key << std::endl;
        this->decomposition(td);
    }

    void TextOutput::decomposition(htd::ITreeDecomposition *td) {
        htd::PreOrderTreeTraversal traversal;

        traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s) {
            std::cout << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v) << std::endl;
        });
    }

    void TextOutput::key_value(const std::string &key, const mpz_class &value) {
//        std::ostringstream str;
//        str << value;
        this->key_value(key, value.get_str());
    }

    void TextOutput::key_value(const std::string &key, const std::size_t value) {
        this->key_value(key, std::to_string(value));
    }

    void TextOutput::key_value(const std::string &key, const std::string &value) {
        std::cout << key << " : " << value << std::endl;
    }

    void TextOutput::info(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void TextOutput::debug(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void TextOutput::warning(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void TextOutput::error(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }
}



