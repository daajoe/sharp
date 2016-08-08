/* 
 * File:   TextOutput.hpp
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

#include <sharp/ISharpOutput.hpp>

#include <string>


namespace sharp {
    class TextOutput : public virtual ISharpOutput {
    public:
        void decomposition(const std::string &key, htd::ITreeDecomposition *td);

        void decomposition(htd::ITreeDecomposition *td);

        void key_value(const std::string &key, const mpz_class &value);

        void key_value(const std::string &key, const std::size_t value);

        void key_value(const std::string &key, const std::string &value);

        void info(const std::string &value);

        void debug(const std::string &value);

        void warning(const std::string &value);

        void error(const std::string &value);
    };
}

