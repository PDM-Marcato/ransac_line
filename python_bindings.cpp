
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"   // important for std::vector conversions
//#include "ransac_2Dcircle.h"
#include "lmeds.h"
//#include "TVector3.h"


using namespace std;
namespace py = pybind11;


/*
// Helper: bind TVector3 in Python (simple version)
void bind_tvector3(py::module_ &m) {
    py::class_<TVector3>(m, "TVector3")
        .def(py::init<>())
        .def(py::init<double, double, double>())
        .def("x", &TVector3::X)
        .def("y", &TVector3::Y)
        .def("z", &TVector3::Z)
        .def("mag", &TVector3::Mag);
}
*/

PYBIND11_MODULE(pyTrackinglines, m) {
    m.doc() = "Python bindings for Trackinglib";

    // First bind TVector3
    //bind_tvector3(m);

    // Bind the Cluster struct
    py::class_<LMedS::Cluster>(m, "Cluster")
        .def_readonly("ClusterStrength", &LMedS::Cluster::ClusterStrength)
        .def_readonly("ClusterSize", &LMedS::Cluster::ClusterSize)
        .def_readonly("ClusterChi2", &LMedS::Cluster::ClusterChi2)
        .def_readonly("ClusterIndex", &LMedS::Cluster::ClusterIndex)
        .def_readonly("ClusterFitP1", &LMedS::Cluster::ClusterFitP1)
        .def_readonly("ClusterFitP2", &LMedS::Cluster::ClusterFitP2);

    // Bind your main class
    py::class_<LMedS>(m, "LMedS")
        .def(py::init<>())
        .def("Init", &LMedS::Init)
        .def("Solve", &LMedS::Solve)
        //.def("GetClusters", &LMedS::GetClusters);  // std::vector<Cluster> handled automatically
        .def("GetClusters", &LMedS::GetClusters,  // std::vector<Cluster> handled automatically
           py::return_value_policy::reference_internal);
}
