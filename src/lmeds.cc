//////////////////////////////////////////////////////////
// LMedS Class
// J.C. Zamora, cardona@if.usp.br
// University of Sao Paulo, 2020
//////////////////////////////////////////////////////////
#define LMedS_cc

#include "lmeds.h"
#include<iostream>

using namespace std;

//ClassImp(LMedS)

LMedS::LMedS() {

  fLMedSMaxIteration = 500;
	fLMedSMinPoints = 30;
	fLMedSThreshold = 15;
  fRandSamplMode = 0;
}

LMedS::~LMedS() {

  //delete Rand;
}

//void LMedS::Init(vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4)
 void LMedS::Init(const vector<double>& v1, const vector<double>& v2, const vector<double>& v3, const vector<double>& v4)
{

  Reset();
  vX = v1;
	vY = v2;
	vZ = v3;
	vQ = v4;

	fOriginalCloudSize = vX.size();
	Vs.reserve(3);
  Ps.reserve(3);

  //std::cout << "/* LMedS inicializa  */" << '\n';

}

void LMedS::Reset()
{

	vX.clear();
	vY.clear();
	vZ.clear();
	vQ.clear();
  Vs.clear();
  Ps.clear();
  errorsVec.clear();
  cluster_vector.clear();
  //delete Rand;

}


void LMedS::Solve(double dist_thres, double Nminpoints, int Nintera)
{

    fLMedSThreshold = dist_thres;
    fLMedSMinPoints = Nminpoints;
    fLMedSMaxIteration = Nintera;

      //std::cout << "numero de puntos  "<<vX.size()<< '\n';
    std::vector<int> remainIndex;
    for (size_t i = 0; i < vX.size(); i++)
    remainIndex.push_back(i);

  	//TVector3 V1, V2;
  	std::vector< int> inliners;
    inliners.clear();
    std::vector< std::pair <double,int> >  IdxMod1;
    std::vector< std::pair <double,int> > IdxMod2;




  	  for(int i=0;i<fLMedSMaxIteration;i++){


        if(remainIndex.size()<fLMedSMinPoints) break;

        //std::vector< int> Rsamples = RandSam(remainIndex,fRandSamplMode);  //random sampling
        std::array<int, 2> Rsamples = RandSam(remainIndex,fRandSamplMode);  //random sampling
        EstimModel(Rsamples); //estimate the linear model


        //std::vector<int> inlIdxR;
        int nbInliers = 0;

        for (auto j = remainIndex.begin(); j != remainIndex.end(); ++j){

  	      double error = EstimError(*j); //error of each point relative to the model
          error = error*error;

  	      if(error<(fLMedSThreshold*fLMedSThreshold)){
  	        	//inlIdxR.push_back(*j);
              nbInliers++;
              errorsVec.push_back(error);
  	        	}
  		     }

         double med = GetMedian(errorsVec);
         errorsVec.clear();


  	    if(nbInliers>fLMedSMinPoints){
          //getting the best models
          double scale = med/nbInliers;
          IdxMod1.push_back( std::make_pair(scale, Rsamples[0]) );
          IdxMod2.push_back( std::make_pair(scale, Rsamples[1]) );
  			} //if a cluster was found

  		}//for Lmeds interactions


      //sort clusters
      sort(IdxMod1.begin(), IdxMod1.end());
      sort(IdxMod2.begin(), IdxMod2.end());

      remainIndex.clear(); // track remaining points
      for (size_t i = 0; i < vX.size(); i++)
      remainIndex.push_back(i);

      //extract inliers using the models
      for (int i = 0; i < IdxMod1.size(); ++i)
      {
        //std::vector<int> ModInx = {IdxMod1[i].second, IdxMod2[i].second};
        std::array<int, 2> ModInx = {IdxMod1[i].second, IdxMod2[i].second};
        EstimModel(ModInx);
        std::vector<int> inlIdxR;
        //ModInx.clear();

        if(remainIndex.size()<fLMedSMinPoints) break;

        int counter = 0;

        for (auto j = remainIndex.begin(); j != remainIndex.end(); ++j)
        {
            double error = EstimError(*j);

            if((error*error)<(fLMedSThreshold*fLMedSThreshold))
            {
              inlIdxR.push_back(*j);
              counter++;
            }

        }

        if(counter>fLMedSMinPoints){
          //TVector3 v1, v2;
          std::vector<double> v20(2);
  	  //double chi2=Fit3D(inlIdxR,v1,v2);
          double chi2=Fit2D(inlIdxR, v20);
          SetCluster(inlIdxR, IdxMod1[i].first, chi2,v20,v20);

          v20.clear();
        }
        std::vector<int> tempRemain;
        std::set_difference(remainIndex.begin(), remainIndex.end(), inlIdxR.begin(), inlIdxR.end(),
        std::inserter(tempRemain, tempRemain.begin()));
        remainIndex = tempRemain;
        inlIdxR.clear();
        tempRemain.clear();
      }

      IdxMod1.clear();
      IdxMod2.clear();
      remainIndex.clear();


}

vector<double> LMedS::GetPDF(const std::vector<int>  samplesIdx){

  size_t pclouds = samplesIdx.size();
  double Tcharge = 0;
  for(int i=0;i<pclouds;i++) Tcharge += vQ[samplesIdx[i]];

  SetAvCharge(Tcharge/pclouds);
  std::vector<double> w;
  if(Tcharge>0)
  for(int i=0;i<pclouds;i++) w.push_back(vQ[samplesIdx[i]]/Tcharge);

  return w;
}




std::array<int, 2> LMedS::RandSam( const vector<int>& indX, int mode)
{

  size_t pclouds = indX.size();
  int p1,p2, p3;
  double w1,w2;
  std::array<int, 2> ranpair;
  //ranpair.resize(3);
   static std::random_device rd;
   static std::mt19937 gen(rd());
   std::uniform_real_distribution<double> dis(0.0, 1.0);



  if(mode==0){
    //-------Uniform sampling
    //p1=(int)(gRandom->Uniform(0,pclouds));
    p1=(int)(pclouds* dis(gen));

	   do{
                //p2=(int)(gRandom->Uniform(0,pclouds));
                p2=(int)(pclouds* dis(gen));
     } while(p2==p1);

     ranpair[0] = indX[p1];
     ranpair[1] = indX[p2];

  }

  if(mode==1){
  //--------Gaussian sampling
    double dist = 0;
    double sigma = 30.0;
    double y = 0;
    double gauss = 0;
    int counter = 0;
    //p1=(int)(gRandom->Uniform(0,pclouds));
    p1=(int)(pclouds* dis(gen));
    double P1[3] = {vX[indX[p1]],vY[indX[p1]],vZ[indX[p1]]};
	  do{
      //p2=(int)(gRandom->Uniform(0,pclouds));
        p2=(int)(pclouds* dis(gen));
       double P2[3] ={vX[indX[p2]],vY[indX[p2]],vZ[indX[p2]]};
      std::array<double, 3> dif = DiffVectors(P2, P1);
      dist = Mag_vec(dif);
      gauss = 1.0*exp(-1.0*pow(dist/sigma,2.0));
      //y = (gRandom->Uniform(0,1));
        y = (dis(gen));
      counter++;
      if(counter>20 && p2!=p1) break;
      } while(p2==p1 || y>gauss);



      ranpair[0] = indX[p1];
      ranpair[1] = indX[p2];

  }


  return ranpair;

}

std::array<double, 3> LMedS:: DiffVectors(double* a, double* b)
{
    std::array<double,3> c;

     for(int i=0;i<3; i++) c[i] = a[i] -b[i] ;

        return c;
}

double LMedS::Mag_vec(std::array<double, 3> a)
{
        double norm = 0;
        int sa = sizeof(a) / sizeof(a[0]);
        for(int i =0; i<sa; i++) norm += a[i]*a[i];
        return sqrt(norm);

}



void LMedS::EstimModel(const std::array<int, 2>  samplesIdx)
{

  //line from two points
  double Po1[3] = {vX[samplesIdx[0]], vY[samplesIdx[0]], 0};
  double Po2[3] = {vX[samplesIdx[1]], vY[samplesIdx[1]], 0};

  double dx = Po2[0] - Po1[0];
  double dy = Po2[1] - Po1[1];

  Vs = {dx,dy,0};
  Ps = {Po1[0], Po1[1], 0};

}

double LMedS::EstimError(int i)
{
    //distance point to line
    //TVector3 newPoint = {vX[i], vY[i], vZ[i]};
    double newPoint[3] = {vX[i], vY[i], vZ[i]};
    double vec[3] = {Ps[0]-newPoint[0], Ps[1]-newPoint[1], Ps[2]-newPoint[2]};
    double nDx = Vs[1]*vec[2] - Vs[2]*vec[1];
    double nDy = Vs[2]*vec[0] - Vs[0]*vec[2];
    double nDz = Vs[0]*vec[1] - Vs[1]*vec[0];
    double nD[3]  = {nDx, nDy, nDz};

	  double dist = sqrt( (nD[0]*nD[0]+nD[1]*nD[1]+nD[2]*nD[2])/(Vs[0]*Vs[0]+Vs[1]*Vs[1]+Vs[2]*Vs[2]));


    return  dist;
}


void LMedS::SetCluster(const std::vector<int> samplesIdx, const double cost, const double Chi2, const std::vector<double>& CP1, const std::vector<double>& CP2)
{

    Cluster cstr;
    cstr.ClusterIndex = samplesIdx;
    cstr.ClusterSize = samplesIdx.size();
    cstr.ClusterStrength = cost;
    cstr.ClusterChi2 = Chi2;
    cstr.ClusterFitP1 = CP1;
    cstr.ClusterFitP2 = CP2;
    cluster_vector.push_back(cstr);
}


double LMedS::GetMedian(const std::vector<double>& errvec)
{
    const size_t vsize = errvec.size();

    if (vsize == 0)
        return 0.0;

    // Make one local copy because nth_element modifies data
    std::vector<double> temp(errvec);

    const size_t mid = vsize / 2;

    // Put median element in correct position
    std::nth_element(temp.begin(),
                     temp.begin() + mid,
                     temp.end());

    double median = temp[mid];

    // Even number of elements
    if (vsize % 2 == 0)
    {
        std::nth_element(temp.begin(),
                         temp.begin() + mid - 1,
                         temp.begin() + mid);

        median = 0.5 * (median + temp[mid - 1]);
    }

    return median;
}






double LMedS::Fit2D(const std::vector<int>& inliners,std::vector<double>& V1)
{
    const int npoints = inliners.size();

    if (npoints < 2)
        return 1e9;

    double Sx  = 0.0;
    double Sy  = 0.0;
    double Sxx = 0.0;
    double Sxy = 0.0;

    // Accumulate sums
    for (const int idx : inliners)
    {
        const double x = vX[idx];
        const double y = vY[idx];

        Sx  += x;
        Sy  += y;
        Sxx += x * x;
        Sxy += x * y;
    }

    const double denom = npoints * Sxx - Sx * Sx;

    if (std::abs(denom) < 1e-12)
        return 1e9;

    // Linear regression
    const double m = (npoints * Sxy - Sx * Sy) / denom;
    const double b = (Sy - m * Sx) / npoints;

    // Compute chi2
    double chi2 = 0.0;

    for (const int idx : inliners)
    {
        const double residual = vY[idx] - (m * vX[idx] + b);
        chi2 += residual * residual;
    }

    const double nu = npoints - 2;

    const double chi2_red =
        (nu > 0) ? chi2 / nu : chi2;

    // Store fit parameters
    V1 = {m, b};


    return chi2_red;
}
