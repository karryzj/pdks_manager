#include"systemSundialsLink.h"

namespace QuanSystem
{

    static int check_retval(void* retvalvalue, const char* funcname, int opt)
    {
        int* errretval;

        /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
        if (opt == 0 && retvalvalue == NULL)
        {
            fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
                funcname);
            return (1);
        }

        /* Check if retval < 0 */
        else if (opt == 1)
        {
            errretval = (int*)retvalvalue;
            if (*errretval < 0)
            {
                fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with retval = %d\n\n",
                    funcname, *errretval);
                return (1);
            }
        }

        /* Check if function returned NULL pointer - no memory allocated */
        else if (opt == 2 && retvalvalue == NULL)
        {
            fprintf(stderr, "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
                funcname);
            return (1);
        }

        return (0);
    }

    static int SolveIt(void* kmem, N_Vector u, N_Vector s, int glstr, int mset)
    {
        int retval;
#ifdef DEBUG
        if (mset == 1) { printf("Exact Newton"); }
        else { printf("Modified Newton"); }

        if (glstr == KIN_NONE) { printf("\n"); }
        else { printf(" with line search\n"); }
#endif // DEBUG
        retval = KINSetMaxSetupCalls(kmem, mset);
        if (check_retval(&retval, "KINSetMaxSetupCalls", 1)) { return (1); }

        retval = KINSol(kmem, u, glstr, s, s); //return>= 2 for error
        if (retval==1|| retval==0) {
            return (1); 
        }
        return retval;
    }

    //static int sundialsFunction(N_Vector x, N_Vector y, void* user_data)
    //{
    //    sunrealtype* udata, * fdata;
    //    SystemSundias* data;

    //    data = (SystemSundias*)user_data;

    //    udata = N_VGetArrayPointer(x);
    //    fdata = N_VGetArrayPointer(y);

    //    std::vector<double*> varList;
    //    std::vector<double> retValue;

    //    printf("x:");
    //    for (int i = 0; i < (int)data->m_functions.size(); i++) {
    //        varList.push_back(udata + i);
    //        printf("%.10f ", udata[i]);
    //    }
    //    printf("\ny:");
    //    //retValue = data->callAllFunction(varList);
    //    for (int i = 0; i < (int)data->m_functions.size(); i++) {
    //        fdata[i] = retValue[i];
    //        printf("%.10f ", fdata[i]);
    //    }
    //    //printf("\n");
    // 

    //    return (0);
    //}

    static int sundialsFunction(N_Vector x, N_Vector y, void* user_data)
    {
        sunrealtype* udata, * fdata;

        SubSystem *subSys = (SubSystem*)user_data;

        udata = N_VGetArrayPointer(x);
        fdata = N_VGetArrayPointer(y);
#ifdef DEBUG
        printf("x:");
        for (int i = 0; i < NV_LENGTH_S(x); i++) {
            printf("%.10f ", udata[i]);
        }
#endif // DEBUG


        for (Func *curFunc: subSys->m_functions) {
            for (sunrealtype* param: curFunc->params) {
                if (subSys->m_varIdx[param]>=0) {
                  //  std::cout<<"param: "<<param<<" idx: "<<subSys->m_varIdx[param]<<std::endl;
                    *param = udata[subSys->m_varIdx[param]];
                }
            }
        }
        int i = 0;
#ifdef DEBUG
        printf("\n");
#endif // DEBUG
        
        int auxVarStartIdx = (int)subSys->m_functions.size() - subSys->m_inequal_num;
        for (Func *curFunc: subSys->m_functions) {
            fdata[i] = curFunc->callFunc();
            if (curFunc->func_type != 0) {
                fdata[i] = fdata[i] + udata[auxVarStartIdx++];
            }
#ifdef DEBUG
            printf("func: %.10f\n", fdata[i]);
#endif // DEBUG
            i++;
        }
#ifdef DEBUG
        printf("\n\n");
#endif // DEBUG
        return (0);
    }

    

    int SystemSundias::solveSub(SubSystem* pSubsys)
    {
        return solveSunContext(pSubsys);
    }
    int SystemSundias::solveSunContext(SubSystem* pSubSys)
    {
        SUNContext sunctx;
        int retval;
        sunrealtype fnormtol, scsteptol;
        N_Vector x, s, c;
        N_Vector y;
        void* kmem;
        SUNMatrix J;
        SUNLinearSolver LS;

        int NEQ = (int)pSubSys->m_functions.size();
        int NINEQ = pSubSys->m_inequal_num;
        x = s = c = NULL;
        kmem = NULL;
        J = NULL;
        LS = NULL;

        /* Create the SUNDIALS context that all SUNDIALS objects require */
        retval = SUNContext_Create(SUN_COMM_NULL, &sunctx);
        if (check_retval(&retval, "SUNContext_Create", 1)) { return (1); }

        SUNLogger logger;
        SUNContext_GetLogger(sunctx, &logger);
        SUNLogger_SetErrorFilename(logger, "E:/coding/vsstudio/muParserSundials/sundials.err");
        /* Create serial vectors of length NEQ */
        x = N_VNew_Serial(NEQ, sunctx);
        if (check_retval((void*)x, "N_VNew_Serial", 0)) { return (1); }

        s = N_VNew_Serial(NEQ, sunctx);
        if (check_retval((void*)s, "N_VNew_Serial", 0)) { return (1); }

        c = N_VNew_Serial(NEQ, sunctx);
        if (check_retval((void*)c, "N_VNew_Serial", 0)) { return (1); }

        y = N_VNew_Serial(NEQ, sunctx);
        if (check_retval((void*)y, "N_VNew_Serial", 0)) { return (1); }
        //can be set by user
        //SetInitialGuess1(u1, data);

        N_VConst(0.0, c);
       int auxVarStartIdx = NEQ - NINEQ;
       for (int i = 0; i < NEQ; i++) { 
           if (pSubSys->m_functions[i]->func_type == funcType::LEQ) {
               NV_Ith_S(c, auxVarStartIdx++) = 1.0;
           }
           else if (pSubSys->m_functions[i]->func_type == funcType::GEQ) {
               NV_Ith_S(c, auxVarStartIdx++) = -1.0;
           }
           else if (pSubSys->m_functions[i]->func_type == funcType::LE) {
               NV_Ith_S(c, auxVarStartIdx++) = 2.0;
           }
           else if (pSubSys->m_functions[i]->func_type == funcType::GE) {
               NV_Ith_S(c, auxVarStartIdx++) = -2.0;
           }
       }
        
        fnormtol = m_fnormtol;
        scsteptol = m_scsteptol;

        kmem = KINCreate(sunctx);
        if (check_retval((void*)kmem, "KINCreate", 0)) { return (1); }

        //void* userdata = (void*)this;
        void* userdata = (void*)pSubSys;
        retval = KINSetUserData(kmem, userdata);
        if (check_retval(&retval, "KINSetUserData", 1)) { return (1); }

        retval = KINSetConstraints(kmem, c);
        if (check_retval(&retval, "KINSetConstraints", 1)) { return (1); }
        retval = KINSetFuncNormTol(kmem, fnormtol);
        if (check_retval(&retval, "KINSetFuncNormTol", 1)) { return (1); }
        retval = KINSetScaledStepTol(kmem, scsteptol);
        if (check_retval(&retval, "KINSetScaledStepTol", 1)) { return (1); }
        retval = KINInit(kmem, sundialsFunction, x);
        if (check_retval(&retval, "KINInit", 1)) { return (1); }

        /* Create dense SUNMatrix */
        J = SUNDenseMatrix(NEQ, NEQ, sunctx);
        if (check_retval((void*)J, "SUNDenseMatrix", 0)) { return (1); }

        /* Create dense SUNLinearSolver object */
        LS = SUNLinSol_Dense(x, J, sunctx);
        if (check_retval((void*)LS, "SUNLinSol_Dense", 0)) { return (1); }

        /* Attach the matrix and linear solver to KINSOL */
        retval = KINSetLinearSolver(kmem, LS, J);
        if (check_retval(&retval, "KINSetLinearSolver", 1)) { return (1); }

        int glstr = m_strategy;//KIN_LINESEARCH;
        long int mset = m_msbset; //mset = 0;
          
        N_VConst(1.0, s); //interface to set scale
       
        bool shouldBreak = false;
        for (int init = -m_initialSet; init < m_initstr; init++) {
            setInitialVar(x, pSubSys, init);
            N_VScale(1, x, y); //y = x
            for (int i = m_tstrategy-1; i >=0 ; i--) {
                if (glstr | (1 << i)) {
                    N_VScale(1.0, y, x);
                    retval = SolveIt(kmem, x, s, i, mset);
                    if (retval == 1) {
                        //printf("sol success\n");
                        getSol(x, pSubSys);
                        shouldBreak = true;
                        break;
                    }
                }
            }
            if (shouldBreak) {
                break;  // Ìø³öÍâ²ãÑ­»·
            }
        }
        /* --------------------------- */

        /* Free memory */

        N_VDestroy(x);
        N_VDestroy(s);
        N_VDestroy(c);
        N_VDestroy(y);
        KINFree(&kmem);
        SUNLinSolFree(LS);
        SUNMatDestroy(J);
        SUNContext_Free(&sunctx);

        if (retval != 1) return 0;
        return retval;
    }
    
}