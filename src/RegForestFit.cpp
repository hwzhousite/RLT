//  **********************************
//  Reinforcement Learning Trees (RLT)
//  Regression
//  **********************************

// my header file
# include "RLT.h"
# include "Utility/Utility.h"
# include "regForest.h"

using namespace Rcpp;
using namespace arma;

// [[Rcpp::export()]]
List RegForestUniFit(arma::mat& X,
          					 arma::vec& Y,
          					 arma::uvec& Ncat,
          					 List& param,
          					 List& RLTparam,
          					 arma::vec& obsweight,
          					 arma::vec& varweight,
          					 int usecores,
          					 int verbose,
          					 arma::umat& ObsTrackPre)
{
  DEBUG_Rcout << "/// THIS IS A DEBUG MODE OF RLT REGRESSION ///" << std::endl;

  // check number of cores
  usecores = checkCores(usecores, verbose);

  // readin parameters 
  PARAM_GLOBAL Param(param);
  PARAM_RLT Param_RLT;

  // create data objects  
  RLT_REG_DATA REG_DATA(X, Y, Ncat, obsweight, varweight);
  
  size_t N = REG_DATA.X.n_rows;
  size_t P = REG_DATA.X.n_cols;
  size_t ntrees = Param.ntrees;
  size_t seed = Param.seed;
  bool kernel_ready = Param.kernel_ready;
  bool obs_track = Param.obs_track;

  int importance = Param.importance;

  // initiate forest
  arma::field<arma::uvec> NodeType(ntrees);
  arma::field<arma::uvec> SplitVar(ntrees);
  arma::field<arma::vec> SplitValue(ntrees);
  arma::field<arma::uvec> LeftNode(ntrees);
  arma::field<arma::uvec> RightNode(ntrees);
  arma::field<arma::vec> NodeSize(ntrees);  
  arma::field<arma::vec> NodeAve(ntrees);

  Reg_Uni_Forest_Class REG_FOREST(NodeType, SplitVar, SplitValue, LeftNode, RightNode, NodeSize, NodeAve);
  
  // other objects
  
  // ObsTrack
  arma::umat ObsTrack;
  
  if (Param.pre_obstrack and ObsTrackPre.n_rows == N and ObsTrackPre.n_cols == ntrees)
    ObsTrack = umat(ObsTrackPre);
  else
    ObsTrack = umat(N, ntrees, fill::zeros);
  
  // NodeRegi
  arma::field<arma::field<arma::uvec>> NodeRegi(ntrees);
  
  // VarImp
  vec VarImp;
  
  if (importance)
    VarImp.zeros(P);
  
  // prediction
  vec Prediction;
  vec OOBPrediction;
  
  Prediction.zeros(N); // initialization means they will be calculated
  OOBPrediction.zeros(N); 
  
  // initiate obs id and var id
  uvec obs_id = linspace<uvec>(0, N-1, N);
  uvec var_id = linspace<uvec>(0, P-1, P);
  
  // start to fit the model
  Reg_Uni_Forest_Build((const RLT_REG_DATA&) REG_DATA,
                       REG_FOREST,
                       (const PARAM_GLOBAL&) Param,
                       (const PARAM_RLT&) Param_RLT,
                       obs_id,
                       var_id,
                       ObsTrack,
                       Prediction,
                       OOBPrediction,
                       NodeRegi,
                       VarImp,
                       seed,
                       usecores,
                       verbose);

  List ReturnList;
  
  List Forest_R;
  
  Forest_R["NodeType"] = NodeType;
  Forest_R["SplitVar"] = SplitVar;
  Forest_R["SplitValue"] = SplitValue;
  Forest_R["LeftNode"] = LeftNode;
  Forest_R["RightNode"] = RightNode;
  Forest_R["NodeSize"] = NodeSize;    
  Forest_R["NodeAve"] = NodeAve;
  
  ReturnList["FittedForest"] = Forest_R;
  
  if (obs_track) ReturnList["ObsTrack"] = ObsTrack;
  if (kernel_ready) ReturnList["NodeRegi"] = NodeRegi;
  if (importance) ReturnList["VarImp"] = VarImp;
  
  ReturnList["Prediction"] = Prediction;
  ReturnList["OOBPrediction"] = OOBPrediction;

  return ReturnList;
}