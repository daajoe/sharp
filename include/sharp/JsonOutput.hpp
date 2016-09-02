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

#include <sharp/ISharpOutput.hpp>
#include <map>
#include <vector>
#include <iostream>

namespace sharp {
    class multimap {
    public:
        typedef enum ValidType {
            DOUBLE, MPZ, SIZE_T, STRING
        } ValidType;

        void add(std::string group, std::string key, double value) {
            unsigned long index = outputv_double.size();
            outputv_double.push_back(value);
            output[group].insert(std::make_pair(key, std::make_pair(DOUBLE, index)));
        };

        void add(std::string group, std::string key, mpz_class value) {
            unsigned long index = outputv_mpz.size();
            outputv_mpz.push_back(value);
            output[group].insert(std::make_pair(key, std::make_pair(MPZ, index)));
        }

        void add(std::string group, std::string key, std::size_t value) {
            unsigned long index = outputv_size_t.size();
            outputv_size_t.push_back(value);
            output[group].insert(std::make_pair(key, std::make_pair(SIZE_T, index)));
        };

        void add(std::string group, std::string key, std::string value) {
            unsigned long index = outputv_str.size();
            outputv_str.push_back(value);
            output[group].insert(std::make_pair(key, std::make_pair(STRING, index)));
        }

        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>>::iterator
        begin() {
            return output.begin();
        }

        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>>::iterator end() {
            return output.end();
        }

        double get_double(unsigned long index) {
            return outputv_double[index];
        }

        mpz_class get_mpz(unsigned long index) {
            return outputv_mpz[index];
        }

        std::size_t get_size_t(unsigned long index) {
            return outputv_size_t[index];
        }

        std::string get_string(unsigned long index) {
            return outputv_str[index];
        }


    private:
//        const std::string JsonOutput::EMPTY;
//        inline const std::string JsonOutput::PRE = "After_Preprocessing";

        std::map<std::string, std::map<std::string, std::pair<ValidType, unsigned long>>> output;

        std::vector<std::string> outputv_str;
        std::vector<std::size_t> outputv_size_t;
        std::vector<double> outputv_double;
        std::vector<mpz_class> outputv_mpz;
    };


//    class map_values {
//        union {
//            double d;
////            mpz_class m;
//            std::size_t t;
////            std::string s;
//        };
//        enum { DOUBLE_VALUE, MPZ_VALUE, SIZE_T_VALUE, STRING_VALUE, } data_type;
//    };


    class JsonOutput : public virtual ISharpOutput {
    public:
        void data(const std::string &group, htd::ITreeDecomposition *td);

//        template<typename T>
//        void data(const std::string &group, const T &value);

        void data(const std::string &group, const mpz_class &value);

        void data(const std::string &group, double value);

        void data(const std::string &group, const std::size_t value);

        void data(const std::string &group, const std::string &value);

        void data(const std::string &group, const std::string &key, htd::ITreeDecomposition *td);

        void data(const std::string &group, const std::string &key, double value);

        void data(const std::string &group, const std::string &key, const std::size_t value);

        void preproc_data(const std::string &group, htd::ITreeDecomposition *td);

        void preproc_data(const std::string &group, const std::size_t value);

        void info(const std::string &value);

        void info(htd::ITreeDecomposition *td);

        void info(const std::string &group, const std::size_t value);

        void info(const std::string &group, const std::string &value);

        void debug(const std::string &value);

        void debug(const std::string &group, const std::size_t value);

        void debug(const std::string &group, const std::string &value);

        void debug(const std::string &value, htd::ITreeDecomposition *td);

        void debug(const std::string &group, const std::string &key, const std::size_t value);

        void warning(const std::string &value);

        void error(const std::string &value);

        void exit(void);

    private:
        static multimap output;

        static std::map<std::string, std::map<std::string, std::string>> output_str;
        static std::map<std::string, std::map<std::string, std::size_t>> output_size_t;
        static std::map<std::string, std::map<std::string, double>> output_double;
        static std::map<std::string, std::map<std::string, mpz_class>> output_mpz;

        static const std::string EMPTY;
        static const std::string PRE;

        const std::string to_string(htd::ITreeDecomposition *td);

//        template<typename T>
//        const T &JsonOutput::id(const T &value) {
//            return value;
//        }

//        template<typename T>
        template<typename T>
        void
        print_map(std::map<std::string, std::map<std::string, T>> output, char delim, std::function<const std::string
                (const T)> f);

        void print_value(multimap::ValidType validType, unsigned long index);
    };
}

