#ifndef CPPAD_CG_SOURCE_CODE_FRAGMENT_INCLUDED
#define CPPAD_CG_SOURCE_CODE_FRAGMENT_INCLUDED
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
     * An operation
     * 
     * \author Joao Leal
     */
    template<class Base>
    class SourceCodeFragment {
    private:
        // the operations used to create this variable (temporary variables only)
        CGOpCode operation_;
        // the code blocks this block depends upon (empty for independent 
        // variables and possibly for the 1st assignment of a dependent variable)
        std::vector<Argument<Base> > arguments_;
        // extra information for some operations (temporary variables only)
        std::vector<CGOpCodeExtra> operationInfo_;
        // variable ID that was altered/assigned in this source code
        // the same ID can be used in more than one variable, meaning these
        // variables can be saved in the same memory location at different times
        // (zero means that no variable is assigned)
        size_t var_id_;
        //
        size_t evaluation_order_;
        // the total number of times the result of this operation is used
        size_t total_use_count_;
        // the number of times the result of this operation has been used
        size_t use_count_;
        // the last source code order in the call graph that uses the result of
        // this operations as an argument
        size_t last_usage_order_;
        // generated variable name
        std::string* name_;
    public:

        inline SourceCodeFragment(CGOpCode op) :
            operation_(op),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(0),
            name_(NULL) {
        }

        inline SourceCodeFragment(CGOpCode op,
                                  const Argument<Base>& arg) :
            operation_(op),
            arguments_(1),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(0),
            name_(NULL) {
            assert(arg.operation() != NULL);
            arguments_[0] = arg;
        }

        inline SourceCodeFragment(CGOpCode op,
                                  const Argument<Base>& arg1,
                                  const Argument<Base>& arg2) :
            operation_(op),
            arguments_(2),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(0),
            name_(NULL) {
            assert(arg1.operation() != NULL || arg2.operation() != NULL);
            arguments_[0] = arg1;
            arguments_[1] = arg2;
        }

        inline SourceCodeFragment(CGOpCode op,
                                  const Argument<Base>& arg1,
                                  const Argument<Base>& arg2,
                                  const Argument<Base>& arg3,
                                  const Argument<Base>& arg4) :
            operation_(op),
            arguments_(4),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(0),
            name_(NULL) {
            assert(arg1.operation() != NULL || arg2.operation() != NULL ||
                   arg3.operation() != NULL || arg4.operation() != NULL);
            arguments_[0] = arg1;
            arguments_[1] = arg2;
            arguments_[2] = arg3;
            arguments_[3] = arg4;
        }

        inline SourceCodeFragment(CGOpCode op,
                                  const std::vector<Argument<Base> >& args,
                                  const std::vector<CGOpCodeExtra>& operationInfo) :
            operation_(op),
            arguments_(args),
            operationInfo_(operationInfo),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(0) {
        }
        
        inline SourceCodeFragment(CGOpCode op,
                                  const std::vector<Argument<Base> >& args,
                                  const std::vector<CGOpCodeExtra>& operationInfo,
                                  const SourceCodeFragment& orig) :
            operation_(op),
            arguments_(args),
            operationInfo_(operationInfo),
            var_id_(orig.var_id_),
            evaluation_order_(orig.evaluation_order_),
            total_use_count_(orig.total_use_count_),
            use_count_(orig.use_count_),
            last_usage_order_(orig.last_usage_order_),
            name_(orig.name_) {
        }

        SourceCodeFragment(const SourceCodeFragment& orig) :
            operation_(orig.operation_),
            arguments_(orig.arguments_),
            operationInfo_(orig.operationInfo_),
            var_id_(0),
            evaluation_order_(0),
            total_use_count_(0),
            use_count_(0),
            last_usage_order_(orig.last_usage_order_),
            name_(orig.name_ != NULL ? name_ = new std::string(*orig.name_) : NULL) {
        }

        inline void makeAlias(const Argument<Base>& other) {
            operation_ = CGAliasOp;
            arguments_.resize(1);
            arguments_[0] = other;
            var_id_ = 0;
            delete name_;
            name_ = NULL;
        }

        inline CGOpCode operationCode() const {
            return operation_;
        }

        /**
         * Provides the arguments used in the operation represnted by this
         * code fragment.
         * \return the arguments for the operation in this code fragment
         */
        inline const std::vector<Argument<Base> >& arguments() const {
            return arguments_;
        }

        /**
         * Provides additional information related with this operation.
         * \return additional information on this operation
         */
        inline const std::vector<CGOpCodeExtra>& operationInfo() const {
            return operationInfo_;
        }

        /**
         * Provides the variable ID that was altered/assigned in this source 
         * code (zero means that no variable is assigned).
         * \return the variable ID
         */
        inline size_t variableID() const {
            return var_id_;
        }

        /**
         * Specifies a variable ID to the result of this source code
         * (zero means that no variable is created).
         */
        inline void setVariableID(size_t var_id) {
            var_id_ = var_id;
        }

        inline size_t getEvaluationOrder() const {
            return evaluation_order_;
        }

        inline void setEvaluationOrder(size_t evaluation_order) {
            evaluation_order_ = evaluation_order;
        }

        /**
         * Provides the total number of times the result of this operation is being 
         * used as an argument for another operation.
         * \return the total usage count
         */
        inline size_t totalUsageCount() const {
            return total_use_count_;
        }

        /**
         * Provides the number of times the result of this operation has been 
         * used as an argument for another operation.
         * \return the current usage count
         */
        inline size_t usageCount() const {
            return use_count_;
        }

        inline size_t getLastUsageEvaluationOrder() const {
            return last_usage_order_;
        }

        inline void setLastUsageEvaluationOrder(size_t last) {
            last_usage_order_ = last;
        }

        inline const std::string* getName() const {
            return name_;
        }

        inline void setName(const std::string& name) {
            assert(name_ == NULL);
            name_ = new std::string(name);
        }

        virtual ~SourceCodeFragment() {
            delete name_;
        }

    protected:

        inline void cloneInfo(const SourceCodeFragment& orig) {
            var_id_ = orig.var_id_;
            evaluation_order_ = orig.evaluation_order_;
            total_use_count_ = orig.total_use_count_;
            use_count_ = orig.use_count_;
            last_usage_order_ = orig.last_usage_order_;
            name_ = orig.name_;
        }


        friend class CodeHandler<Base>;

    };

    template<class Base>
    inline std::ostream& operator <<(
    std::ostream& os, //< stream to write to
    const CppAD::SourceCodeFragment<Base>& c) {
        switch (c.operationCode()) {
            case CGAbsOp:
                os << "abs( $1 )";
                break;
            case CGAcosOp:
                os << "acos( $1 )";
                break;
            case CGAddOp:
                os << "$1 + $2";
            case CGAddSubOp:
                os << "$1 (+/-) $2 (+/-) ...";
                break;
            case CGAliasOp:
                os << "alias($1)";
                break;
            case CGAsinOp:
                os << "asin( $1 )";
                break;
            case CGAtanOp:
                os << "atan( $1 )";
                break;
            case CGComOpLt:
                os << "($1 < $2)? $3 : $4";
                break;
            case CGComOpLe:
                os << "($1 <= $2)? $3 : $4";
                break;
            case CGComOpEq:
                os << "($1 == $2)? $3 : $4";
                break;
            case CGComOpGe:
                os << "($1 > $2)? $3 : $4";
                break;
            case CGComOpGt:
                os << "($1 >= $2)? $3 : $4";
                break;
            case CGComOpNe:
                os << "($1 != $2)? $3 : $4";
                break;
            case CGCoshOp:
                os << "cosh( $1 )";
                break;
            case CGCosOp:
                os << "cosh( $1 )";
                break;
            case CGDivOp:
                os << "$1 / $2";
                break;
            case CGExpOp:
                os << "e^$1";
                break;
            case CGInvOp:
                os << "independent( $1 )";
                break;
            case CGLogOp:
                os << "log( $1 )";
                break;
            case CGMulOp:
                os << "$1 * $2";
                break;
            case CGPowOp:
                os << "$1^$2";
                break;
            case CGSignOp:
                os << "if($1 > 0) { 1 } else if($1 == 0) { 0 } else { -1 }";
                break;
            case CGSinhOp:
                os << "sinh( $1 )";
                break;
            case CGSinOp:
                os << "sin( $1 )";
                break;
            case CGSqrtOp:
                os << "sqrt( $1 )";
                break;
            case CGSubOp:
                os << "$1 - $2";
                break;
            case CGTanhOp:
                os << "tanh( $1 )";
                break;
            case CGTanOp:
                os << "tan( $1 )";
                break;
            case CGUnMinusOp:
                os << "-$1";
                break;
            default:
                os << "???";
        }

        return os;
    }

}

#endif
