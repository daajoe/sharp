/*
 * File:   JsonOutput.hpp
 *
 * Author: FICHTE Johannes (fichte@dbai.tuwien.ac.at)
 *
 * Copyright 2016, Johannes K. Fichte
 *    E-Mail: <fichte@dbai.tuwien.ac.at>
 *
 * This file is part of sharp.
 *
 * sharp is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * sharp is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with sharp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sharp/OutputMultiMap.hpp"

#include <sharp/global>

#include <gmpxx.h>

#include <string>
#include <vector>

namespace sharp {
    void OutputMultiMap::add(std::string group, std::string key, double value) {
        unsigned long index = outputv_double.size();
        outputv_double.push_back(value);
        output[group].insert(std::make_pair(key, std::make_pair(DOUBLE, index)));
    }

    void OutputMultiMap::add(std::string group, std::string key, mpz_class value) {
        unsigned long index = outputv_mpz.size();
        outputv_mpz.push_back(value);
        output[group].insert(std::make_pair(key, std::make_pair(MPZ, index)));
    }

    void OutputMultiMap::add(std::string group, std::string key, std::size_t value) {
        unsigned long index = outputv_size_t.size();
        outputv_size_t.push_back(value);
        output[group].insert(std::make_pair(key, std::make_pair(SIZE_T, index)));
    }

    void OutputMultiMap::add(std::string group, std::string key, std::string value) {
        unsigned long index = outputv_str.size();
        outputv_str.push_back(value);
        output[group].insert(std::make_pair(key, std::make_pair(STRING, index)));
    }

    void OutputMultiMap::add(std::string group, std::string key, htd::ITreeDecomposition *td) {
        unsigned long index = outputv_td.size();
        outputv_td.push_back(td);
        output[group].insert(std::make_pair(key, std::make_pair(TDECOMP, index)));
    }

    std::map<std::string, std::map<std::string, std::pair<OutputMultiMap::ValidType, unsigned long>>>::iterator
    OutputMultiMap::begin(void) {
        return output.begin();
    }

    std::map<std::string, std::map<std::string, std::pair<OutputMultiMap::ValidType, unsigned long>>>::iterator
    OutputMultiMap::end(void) {
        return output.end();
    }

    double OutputMultiMap::get_double(unsigned long index) {
        return outputv_double[index];
    }

    mpz_class OutputMultiMap::get_mpz(unsigned long index) {
        return outputv_mpz[index];
    }

    std::size_t OutputMultiMap::get_size_t(unsigned long index) {
        return outputv_size_t[index];
    }

    std::string OutputMultiMap::get_string(unsigned long index) {
        return outputv_str[index];
    }

    htd::ITreeDecomposition* OutputMultiMap::get_td(unsigned long index) {
        return outputv_td[index];
    }

}