#ifndef CPPAD_CG_OPERATION_INCLUDED
#define CPPAD_CG_OPERATION_INCLUDED
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

    /**
     * Possible operations
     * 
     * \author Joao Leal
     */
    enum CGOpCode {
        CGAbsOp, //  abs(variable)
        CGAcosOp, // asin(variable)
        CGAddOp, //  a + b
        CGAliasOp, //  alias (reference to another operation)
        CGAsinOp, // asin(variable)
        CGAtanOp, // atan(variable)
        CGComOpLt, // result = left < right? trueCase: falseCase
        CGComOpLe, // result = left <= right? trueCase: falseCase
        CGComOpEq, // result = left == right? trueCase: falseCase
        CGComOpGe, // result = left >= right? trueCase: falseCase
        CGComOpGt, // result = left > right? trueCase: falseCase
        CGComOpNe, // result = left != right? trueCase: falseCase
        CGCoshOp, // cosh(variable)
        CGCosOp, //  cos(variable)
        CGDivOp, // a / b
        CGExpOp, //  exp(variable)
        CGInvOp, //                             independent variable
        CGLogOp, //  log(variable)
        CGMulOp, // a * b
        CGPowOp, //  pow(a,   b)
        //        PriOp, //  PrintFor(text, parameter or variable, parameter or variable)
        CGSignOp, // result = (x > 0)? 1.0:((x == 0)? 0.0:-1)
        CGSinhOp, // sinh(variable)
        CGSinOp, //  sin(variable)
        CGSqrtOp, // sqrt(variable)
        CGSubOp, //  a - b
        CGTanhOp, //  tanh(variable)
        CGTanOp, //  tan(variable)
        CGUnMinusOp // -(a)
    };

    inline std::ostream& operator <<(std::ostream& os, const CGOpCode& op) {
        switch (op) {
            case CGAbsOp:
                os << "abs()";
                break;
            case CGAcosOp:
                os << "acos()";
                break;
            case CGAddOp:
                os << "a+b";
                break;
            case CGAsinOp:
                os << "asin()";
                break;
            case CGAtanOp:
                os << "atan()";
                break;
            case CGComOpLt:
                os << "result = left < right? trueCase: falseCase";
                break;
            case CGComOpLe:
                os << "result = left <= right? trueCase: falseCase";
                break;
            case CGComOpEq:
                os << "result = left == right? trueCase: falseCase";
                break;
            case CGComOpGe:
                os << "result = left >= right? trueCase: falseCase";
                break;
            case CGComOpGt:
                os << "result = left > right? trueCase: falseCase";
                break;
            case CGComOpNe:
                os << "result = left != right? trueCase: falseCase";
                break;
            case CGCoshOp:
                os << "cosh()";
                break;
            case CGCosOp:
                os << "cos()";
                break;
            case CGDivOp:
                os << "a / b";
                break;
            case CGExpOp:
                os << "exp()";
                break;
            case CGInvOp:
                os << "independent()";
                break;
            case CGLogOp:
                os << "log()";
                break;
            case CGMulOp:
                os << "a * b";
                break;
            case CGPowOp:
                os << "pow(a, b)";
                break;
                //        PriOp: //  PrintFor(text, parameter or variable, parameter or variable)
            case CGSignOp:
                os << "sign()";
                break;
            case CGSinhOp:
                os << "sinh()";
                break;
            case CGSinOp:
                os << "sin()";
                break;
            case CGSqrtOp:
                os << "sqrt()";
                break;
            case CGSubOp:
                os << "a - b";
                break;
            case CGTanhOp:
                os << "tanh()";
                break;
            case CGTanOp:
                os << "tan()";
                break;
            case CGUnMinusOp:
                os << "-(a)";
                break;
            default:
                os << "\?\?\?\?()";
                break;
        }
        return os;
    }

}

#endif
