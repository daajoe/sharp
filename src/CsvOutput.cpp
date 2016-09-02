/*
 * File:   CsvOutput.cpp
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

#include "sharp/CsvOutput.hpp"

#include <iostream>
#include <sstream>

#include <htd/PreOrderTreeTraversal.hpp>

namespace sharp {
    void CsvOutput::data(const std::string &key, htd::ITreeDecomposition *td) {
        std::cout << key << std::endl;
        info(td);
    }

    void CsvOutput::data(const std::string &key, const mpz_class &value) {
        this->data(key, value.get_str());
    }

    void CsvOutput::data(const std::string &key, const std::size_t value) {
        this->data(key, std::to_string(value));
    }

    void CsvOutput::data(const std::string &key, const std::string &value) {
        std::cout << key << " : " << value << std::endl;
    }

    void CsvOutput::info(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void CsvOutput::data(const std::string &group, const std::string &key, htd::ITreeDecomposition *td) {
        std::cout << group << " ";
        this->data(key,td);
    }

    void CsvOutput::data(const std::string &group, const std::string &key, double value) {
        std::cout << group << " ";
        this->data(key, std::to_string(value));
    }

    void CsvOutput::data(const std::string &group, const std::string &key, const std::size_t value) {
        std::cout << group << " ";
        this->data(key, std::to_string(value));
    }

    void CsvOutput::preproc_data(const std::string &key, htd::ITreeDecomposition *td) {
        std::cout << key << std::endl;
        info(td);
    }

    void CsvOutput::preproc_data(const std::string &key, const std::size_t value) {
        this->data(key, std::to_string(value));
    }

    void CsvOutput::info(htd::ITreeDecomposition *td) {
        htd::PreOrderTreeTraversal traversal;

        traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s) {
            std::cout << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v) << std::endl;
        });
    }

    void CsvOutput::info(const std::string &key, const std::size_t value) {
        data(key, value);
    }

    void CsvOutput::info(const std::string &key, const std::string &value) {
        data(key, value);
    }

    void CsvOutput::debug(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void CsvOutput::debug(const std::string &key, const std::size_t value) {
        std::cout << "# " << key<< ":" << value << std::endl;
    }

    void CsvOutput::debug(const std::string &key, const std::string &value) {
        std::cout << "# " << key<< ":" << value << std::endl;
    }

    void CsvOutput::debug(const std::string &value, htd::ITreeDecomposition *td) {
        data(value, td);
    }

    void CsvOutput::debug(const std::string &group, const std::string &key, const std::size_t value) {
        data(group, key, value);
    }

    void CsvOutput::warning(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

    void CsvOutput::error(const std::string &value) {
        std::cout << "# " << value << std::endl;
    }

}



