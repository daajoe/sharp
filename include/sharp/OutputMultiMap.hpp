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

#ifndef SHARP_OUTPUTMULTIMAP_H
#define SHARP_OUTPUTMULTIMAP_H

#include <sharp/OutputMultiMap.hpp>

#include <sharp/global>
#include <htd/TreeDecompositionFactory.hpp>

#include <gmpxx.h>

#include <map>
#include <string>
#include <vector>

namespace sharp {
    class OutputMultiMap {
    public:
        typedef enum ValidType {
            DOUBLE, MPZ, SIZE_T, STRING, TDECOMP
        } ValidType;

        void add(std::string group, std::string key, double value);

        void add(std::string group, std::string key, mpz_class value);

        void add(std::string group, std::string key, std::size_t value);

        void add(std::string group, std::string key, std::string value);

        void add(std::string group, std::string key, htd::ITreeDecomposition *td);

        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>>::iterator begin(void);

        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>>::iterator end(void);

        double get_double(unsigned long index);

        mpz_class get_mpz(unsigned long index);

        std::size_t get_size_t(unsigned long index);

        std::string get_string(unsigned long index);

        htd::ITreeDecomposition* get_td(unsigned long index);

    private:
        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>>
                output;

        std::vector<double> outputv_double;
        std::vector<mpz_class> outputv_mpz;
        std::vector<std::size_t> outputv_size_t;
        std::vector<std::string> outputv_str;
        std::vector<htd::ITreeDecomposition*> outputv_td;

    };
}

#endif //SHARP_OUTPUTMULTIMAP_H
