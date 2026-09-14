//////////////////////////////////////////////////////////
// LMedS Class
// J.C. Zamora, cardona@if.usp.br
// University of Sao Paulo, 2020
//////////////////////////////////////////////////////////
#ifndef LMedS_H
#define LMedS_H

/*
// ROOT Headers
#include <TObject.h>
#include <TH2F.h>
#include <TGraph2D.h>
#include <TMath.h>
#include <TVector3.h>
#include <TRandom.h>
*/

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <vector>
#include <random>
#include <array>
#include <algorithm>
#include <cmath>

using namespace std;



class LMedS
{
  public:
  LMedS();
  ~LMedS();

  void Reset();
	//void Init(vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4);
  void Init(const vector<double>& v1, const vector<double>& v2, const vector<double>& v3, const vector<double>& v4);
	void Solve(double dist_thres, double Nminpoints, int Nintera);
  //vector<int> RandSam(vector<int> indX, Int_t mode);
  std::array<int, 2> RandSam(const vector<int>& indX, int mode);
  //void EstimModel(const std::vector<int>  samplesIdx);
  void EstimModel(const std::array<int, 2>  samplesIdx);
  double EstimError(int i);
	//vector<double> GetChargeOfTracks();
	//vector<double> GetTrackLength();
  vector<double> GetPDF(const std::vector<int>  samplesIdx);
  void SetAvCharge(double charge){Avcharge = charge;};
  //double GetAvCharge(){return Avcharge;};
	//double Fit3D(vector<int> inliners, TVector3& V1, TVector3& V2);
  double Fit2D(const std::vector<int>& inliners,std::vector<double>& V1);
  double GetMedian(const std::vector<double>& errvec);

  //TRandom* Rand;
  std::vector<double> Vs;
  std::vector<double> Ps;

  std::array<double, 3> DiffVectors(double* a, double* b);
  double Mag_vec (std::array<double, 3> a);

  //-----get a cluster (maybe a track class?)
  struct Cluster // return type of structure
    {
      double ClusterStrength;		// strength
      size_t ClusterSize;			// size
      double ClusterChi2;			// size
      std::vector<int> ClusterIndex;			// Indices
      std::vector<double> ClusterFitP1;
      std::vector<double> ClusterFitP2;
    };


  typedef std::vector<Cluster> AllClusters;


  void SetCluster(const std::vector<int> samplesIdx, const double cost, const double Chi2, const std::vector<double>& CP1, const std::vector<double>& CP2);
  //inline AllClusters GetClusters(){return cluster_vector;}
  inline const AllClusters& GetClusters(){return cluster_vector;}
  AllClusters cluster_vector;



  private:
  vector<double> vX, vY, vZ, vQ;
  vector<double> errorsVec;
	vector<double> vTrackCharge;
  float fLMedSMinPoints;
	float fLMedSPointThreshold;
	float fLMedSChargeThreshold;
	float fLMedSThreshold;
	float fLMedSMaxIteration;
	int fNumberOfTracksMax;
	int fOriginalCloudSize;
	double fTotalCharge;
	int fNumberOfPadsX;
	int fNumberOfPadsY;
	int fVerbose;
  double Avcharge;
  int fRandSamplMode;


  //public:
  //ClassDef(LMedS,0)
};

#endif
