//  **********************************
//  Reinforcement Learning Trees (RLT)
//  Regression
//  **********************************

// my header file
# include "../RLT.h"
# include "../Trees//Trees.h"
# include "../Utility/Utility.h"
# include "../regForest.h"

using namespace Rcpp;
using namespace arma;

void Cal_Multi_Find_A_Split(Uni_Split_Class& OneSplit,
                          const RLT_REG_DATA& REG_DATA,
                          const PARAM_GLOBAL& Param,
                          const PARAM_RLT& RLTParam,
                          uvec& obs_id,
                          uvec& var_id)
{
  
  size_t mtry = Param.mtry;
  size_t nmin = Param.nmin;
  double alpha = Param.alpha;
  int nsplit = Param.nsplit;
  int split_gen = Param.split_gen;
  int split_rule = Param.split_rule;
  
  size_t N = obs_id.n_elem;
  size_t P = var_id.n_elem;

  mtry = ( (mtry <= P) ? mtry:P ); // take minimum
  
  uvec var_try = arma::randperm(P, mtry);
  
  DEBUG_Rcout << "    --- Reg_Find_A_Split with mtry = " << mtry << std::endl;

  for (size_t j = 0; j < mtry; j++)
  {
    size_t temp_var = var_id(var_try(j));
    
    Uni_Split_Class TempSplit;
    TempSplit.var = temp_var;
    TempSplit.value = 0;
    TempSplit.score = -1;
      
    Cla_Multi_Split(TempSplit, 
                        obs_id, 
                        REG_DATA.X.unsafe_col(temp_var), 
                        REG_DATA.Ncat(temp_var),
                        REG_DATA.Y, 
                        REG_DATA.obsweight, 
                        0.0, // penalty
                        split_gen, 
                        split_rule, 
                        nsplit, 
                        nmin, 
                        alpha, 
                        useobsweight);


    
    if (TempSplit.score > OneSplit.score)
    {
      OneSplit.var = TempSplit.var;
      OneSplit.value = TempSplit.value;
      OneSplit.score = TempSplit.score;
    }
  }
}