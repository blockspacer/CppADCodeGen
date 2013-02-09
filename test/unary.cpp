/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
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
#include "CppADCGOperationTest.hpp"
#include "unary.hpp"

using namespace CppAD;

TEST_F(CppADCGOperationTest, unaryPlus) {
    std::vector<double> u(2);
    u[0] = -3.;
    u[1] = 2.;

    test0nJac("UnaryPlus", &UnaryPlusFunc<double >, &UnaryPlusFunc<CG<double> >, u);
}

TEST_F(CppADCGOperationTest, unaryMinus) {
    std::vector<double> u(2);
    u[0] = -3.;
    u[1] = 2.;

    test0nJac("UnaryMinus", &UnaryMinusFunc<double >, &UnaryMinusFunc<CG<double> >, u);
}
