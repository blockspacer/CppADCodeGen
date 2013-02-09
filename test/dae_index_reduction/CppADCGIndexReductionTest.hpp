/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2013 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */
#ifndef CPPADCGOO_CPPADCGINDEXREDUCTIONTEST_HPP
#define	CPPADCGOO_CPPADCGINDEXREDUCTIONTEST_HPP

#include "../CppADCGTest.hpp"

namespace CppAD {

    class CppADCGIndexReductionTest : public CppADCGTest {
    public:

        inline CppADCGIndexReductionTest(bool verbose = false, bool printValues = false) :
            CppADCGTest(verbose, printValues) {
            /**
             * disable memory check because CPPAD_USER_ATOMIC is used by the
             * index reduction which creates static variables that will not be
             * destroyed until the program ends
             */
            this->memory_check_ = false;
        }
    };
}


#endif
