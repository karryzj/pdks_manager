#include "quanSystem.h"
#include <kinsol/kinsol.h> /* access to KINSOL func., consts. */
#include <math.h>
#include <nvector/nvector_serial.h> /* access to serial N_Vector       */
#include <stdio.h>
#include <stdlib.h>
#include <sundials/sundials_types.h>   /* defs. of sunrealtype, sunindextype */
#include <sunlinsol/sunlinsol_dense.h> /* access to dense SUNLinearSolver */
#include <sunmatrix/sunmatrix_dense.h> 



namespace QuanSystem {
    class API_EXPORT_CXX SystemSundias : public SystemBase
    {
        #define FTOL SUN_RCONST(1.e-10) /* function tolerance */
        #define STOL SUN_RCONST(1.e-10) /* step tolerance     */
        /* Accessor macro */
        #define  Ith(v, i) NV_Ith_S(v, i - 1)
    
    public:
        SystemSundias() {
            m_fnormtol = FTOL;
            m_scsteptol = STOL;
            m_strategy = -1;
            m_msbset = 0;
            m_tstrategy = 2;
        };
        SystemSundias(mu::Parser* env_) : SystemBase(env_) {
            m_fnormtol = FTOL;
            m_scsteptol = STOL;
            m_strategy = -1;
            m_msbset = 0;
            m_tstrategy = 2;
        };
        void SetFuncTolerance(double tol) { m_fnormtol = tol; }
        void SetStepTolerance(double tol) { m_scsteptol = tol; }
        void SetKinsolStrategy(int strategy) { m_strategy = strategy; }
        void SetMaxNonlinIter(long int msbset) { m_msbset = msbset; }

        virtual int solveSub(SubSystem* pSubsys) override;
        int solveSunContext(SubSystem* pSubsys);

        virtual int solve() override {
            ParamVec params;
            StringMapParam variables = getVar();
            for (StringMapParam::iterator it = variables.begin(); it != variables.end(); ++it) {
                params.emplace_back(it->second);
            }
            SubSystem allSubsys(m_functions, params);
            return solveSunContext(&allSubsys);
        }
        
        void getSol(N_Vector x, SubSystem* pSubsys)
        {
            for (auto it = pSubsys->m_varIdx.begin(); it != pSubsys->m_varIdx.end(); ++it) {
                if (it->second >= 0) {
                    *(it->first)= NV_Ith_S(x, it->second);
                }
            }
        }
        void setInitialVar(N_Vector x, SubSystem* pSubSys, int part)
        {
            if (part == -1) {
                for (auto it = pSubSys->m_varIdx.begin(); it != pSubSys->m_varIdx.end(); ++it) {
                    if (it->second >= 0) {
                        NV_Ith_S(x, it->second) = *(it->first);
                    }
                }
            }
            else {              
                for (auto it = pSubSys->m_varIdx.begin(); it != pSubSys->m_varIdx.end(); ++it) {
                    if (it->second >= 0) {
                        double ub = 10000;
                        double lb = -10000;
                        if (pSubSys->m_varGuessUB.find(it->first) != pSubSys->m_varGuessUB.end())
                            ub = pSubSys->m_varGuessUB[it->first];
                        if (pSubSys->m_varGuessLB.find(it->first) != pSubSys->m_varGuessLB.end())
                            lb = pSubSys->m_varGuessLB[it->first];
                        NV_Ith_S(x, it->second) = (ub - lb) / (1.0 * m_initstr) * part + lb;
                        *(it->first) = NV_Ith_S(x, it->second);
                    }
                }
            }
            int auxVarStartIdx = (int)pSubSys->m_functions.size() - pSubSys->m_inequal_num;
            for (Func* curFunc : pSubSys->m_functions) {
                if (curFunc->func_type != 0) {
                    NV_Ith_S(x,auxVarStartIdx) = fabs(curFunc->callFunc());
                    if(curFunc->func_type == funcType::GE||curFunc->func_type == funcType::GEQ)
                        NV_Ith_S(x,auxVarStartIdx) = -NV_Ith_S(x,auxVarStartIdx);
                    if (NV_Ith_S(x, auxVarStartIdx) == 0) {
                        if (curFunc->func_type == funcType::GE) {
                            NV_Ith_S(x, auxVarStartIdx) = NV_Ith_S(x, auxVarStartIdx) + FTOL;
                        }
                        else if (curFunc->func_type == funcType::LE) {
                            NV_Ith_S(x, auxVarStartIdx) = NV_Ith_S(x, auxVarStartIdx) - FTOL;
                        }
                    }
#ifdef DEBUG
                    printf("aux :%lf\n", NV_Ith_S(x, auxVarStartIdx));
#endif // DEBUG 
                    auxVarStartIdx++;
                }
            }
        }
    private:
        int m_tstrategy; /* total number of strategy */
        int m_initstr=10;
        sunrealtype m_fnormtol; /* function tolerance */
        sunrealtype m_scsteptol; /* step tolerance     */
        int m_strategy;  /* KINSOL strategy*/
        long int m_msbset; //maximum number of nonlinear iterations without a call to the preconditioner or Jacobian setup function. Pass 0 to indicate the default.
    };
}
