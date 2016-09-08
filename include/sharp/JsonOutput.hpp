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

//#define _GLIBCXX_USE_CXX11_ABI 0

#include <sharp/ISharpOutput.hpp>
#include <sharp/OutputMultiMap.hpp>
#include <map>
#include <iostream>

namespace sharp {
    class JsonOutput : public ISharpOutput {
    public:

        void data(const std::string &key, const std::size_t value);

        void data(const std::string &key, const std::string &value);

        void data(const std::string &key, const mpz_class &value);

        void data(const std::string &key, htd::ITreeDecomposition *td);

        void data(const std::string &group, const std::string &key, htd::ITreeDecomposition *td);

        void data(const std::string &group, const std::string &key, double value);

        void data(const std::string &group, const std::string &key, const std::size_t value);

        void info(const std::string &value);

        void info(htd::ITreeDecomposition *td);

        void debug(const std::string &value);

        void debug(const std::string &key, const std::size_t value);

        void debug(const std::string &key, const std::string &value);

        void debug(const std::string &value, htd::ITreeDecomposition *td);

        void debug(const std::string &group, const std::string &key, const std::size_t value);

        void info(const std::string &key, const std::size_t value);

        void info(const std::string &key, const std::string &value);

        void preproc_data(const std::string &key, const std::size_t value);

        void preproc_data(const std::string &key, htd::ITreeDecomposition *td);

        void warning(const std::string &value);

        void error(const std::string &value);

        void exit(void);

    private:
        static OutputMultiMap output;

        static std::map<std::string, std::map<std::string, std::string>> output_str;
        static std::map<std::string, std::map<std::string, std::size_t>> output_size_t;
        static std::map<std::string, std::map<std::string, double>> output_double;
        static std::map<std::string, std::map<std::string, mpz_class>> output_mpz;

        static const std::string EMPTY;
        static const std::string PRE;

        const std::string to_string(htd::ITreeDecomposition *td);

        void print_value(OutputMultiMap::ValidType validType, unsigned long index);
    };
}

