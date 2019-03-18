/* 
 * File:   ISharpOutput.hpp
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
#ifndef SHARP_SHARP_SHARPOUTPUT_H_
#define SHARP_SHARP_SHARPOUTPUT_H_

#include <cstddef>
#include <sharp/global>
#include <string>
#include <gmpxx.h>

#include <htd/TreeDecompositionFactory.hpp>

namespace sharp {

    class SHARP_API ISharpOutput {
    public:
        virtual void data(const std::string &key, const std::size_t value) = 0;

        virtual void data(const std::string &key, const std::string &value) = 0;

        virtual void data(const std::string &key, const mpz_class &value) = 0;

        virtual void data(const std::string &key, htd::ITreeDecomposition *td) = 0;

        virtual void data(const std::string &group, const std::string &key, htd::ITreeDecomposition *td) = 0;

        virtual void data(const std::string &group, const std::string &key, double value) = 0;

        virtual void data(const std::string &group, const std::string &key, const std::size_t value) = 0;

        virtual void info(const std::string &value) = 0;

        virtual void info(htd::ITreeDecomposition *td) = 0;

        virtual void debug(const std::string &value) = 0;

        virtual void debug(const std::string &key, const std::size_t value) = 0;

        virtual void debug(const std::string &key, const std::string &value)= 0;

        virtual void debug(const std::string &value, htd::ITreeDecomposition *td) = 0;

        virtual void debug(const std::string &group, const std::string &key, const std::size_t value) = 0;

        virtual void info(const std::string &key, const std::size_t value) = 0;

        virtual void info(const std::string &key, const std::string &value)= 0;

        virtual void preproc_data(const std::string &key, htd::ITreeDecomposition *td) = 0;

        virtual void preproc_data(const std::string &key, const std::size_t value) = 0;

        virtual void warning(const std::string &value)= 0;

        virtual void error(const std::string &value)= 0;

        virtual void exit(void) {};
    };
}

#endif
