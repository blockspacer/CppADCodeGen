/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2012 Ciengis

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#include <cppad_cgoo/cg.hpp>

#include "gcc_load_dynamic.hpp"
#include "div_mul.hpp"

namespace { // BEGIN empty namespace

    bool DivMulTestOne() {
        bool ok = true;

        using namespace CppAD;

        // independent variable vector, indices, values, and declaration
        std::vector<double> u(3);
        u[0] = 2.;
        u[1] = 3.;
        u[2] = 4.;

        ok &= test0nJac("DivMulTestOne", &DivMulTestOneFunc<double >, &DivMulTestOneFunc<CG<double> >, u);

        return ok;
    }

    bool DivMulTestTwo() {
        using namespace CppAD;

        bool ok = true;

        // independent variable vector
        std::vector<double> u(4);
        u[0] = 2.;
        u[1] = 3.;
        u[2] = 4.;
        u[3] = 5.;

        ok &= test0nJac("DivMulTestTwo", &DivMulTestTwoFunc<double >, &DivMulTestTwoFunc<CG<double> >, u);

        return ok;
    }

} // END empty namespace

bool DivMul() {
    bool ok = true;
    ok &= DivMulTestOne();
    ok &= DivMulTestTwo();
    return ok;
}
