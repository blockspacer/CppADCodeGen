#ifndef CPPAD_CG_NAN_HPP
#define	CPPAD_CG_NAN_HPP
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

namespace CppAD {

    template <class Base>
    inline bool isnan(const CG<Base> &s) {
        CPPAD_ASSERT_FIRST_CALL_NOT_PARALLEL;
        static Base scalar_nan = nan(Base(0));
        if (s.isVariable()) {
            return false;
        } else {
            // a parameter
            const Base& v = s.getParameterValue();
            return (v != v) | (v == scalar_nan);
        }
    }
}

#endif
