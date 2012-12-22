#ifndef CPPAD_CG_COMPARE_INCLUDED
#define CPPAD_CG_COMPARE_INCLUDED
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
    inline bool operator ==(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return left.getParameterValue() == right.getParameterValue();
        } else if (left.isParameter() || right.isParameter()) {
            return false;
        } else {
            return left.getSourceCodeFragment() == right.getSourceCodeFragment();
        }
    }

    template<class Base>
    inline bool operator !=(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return left.getParameterValue() != right.getParameterValue();
        } else if (left.isParameter() || right.isParameter()) {
            return true;
        } else {
            return left.getSourceCodeFragment() != right.getSourceCodeFragment();
        }
    }

#define CPPAD_CG_OPERATOR(Op)                                                  \
    template<class Base>                                                       \
    inline bool operator Op(const CG<Base> &left, const CG<Base> &right) {     \
        if (left.isParameter() && right.isParameter()) {                       \
            return left.getParameterValue() Op right.getParameterValue();      \
        } else {                                                               \
            throw CGException("Cannot use the "#Op" comparison operator in non parameter variables");\
        }                                                                      \
    }

    CPPAD_CG_OPERATOR(>)
    CPPAD_CG_OPERATOR( >=)
    CPPAD_CG_OPERATOR(<)
    CPPAD_CG_OPERATOR( <=)
}

#endif

