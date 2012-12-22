#ifndef CPPAD_CG_IDENTICAL_INCLUDED
#define CPPAD_CG_IDENTICAL_INCLUDED
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

namespace CppAD {

    template<class Base>
    inline bool IdenticalPar(const CG<Base>& x) throw (CGException) {
        if (!x.isParameter()) {
            return false; // its value may change after tapping
        }
        return IdenticalPar(x.getParameterValue());
    }

    template<class Base>
    inline bool IdenticalZero(const CG<Base>& x) throw (CGException) {
        if (!x.isParameter()) {
            return false;
        }
        return IdenticalZero(x.getParameterValue());
    }

    template<class Base>
    inline bool IdenticalOne(const CG<Base>& x) throw (CGException) {
        if (!x.isParameter()) {
            return false;
        }
        return IdenticalOne(x.getParameterValue());
    }

    template<class Base>
    inline bool IdenticalEqualPar(const CG<Base>& x, const CG<Base>& y) {
        return x.isParameter() && y.isParameter() && IdenticalEqualPar(x.getParameterValue(), y.getParameterValue());
    }
}

#endif

