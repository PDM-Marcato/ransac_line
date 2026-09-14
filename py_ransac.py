

import pyTrackinglines
import numpy as np



# Path to your text file
filename = "test2Dlines.dat"
#filename = "datos.dat"

# Load the file (assumes space or tab separated values)
vX, vY = np.loadtxt(filename, unpack=True)
vZ = [0]*len(vX)
vQ = [1]*len(vX)

#print(len(vZ))
#print(vQ)

obj = pyTrackinglines.LMedS()
inicia = obj.Init(vX, vY, vZ, vQ)
resuelve = obj.Solve(10.0,30,2500)
clusters = obj.GetClusters()

print('number of clusters: ', len(clusters))
k = 0

for c in clusters:
        clustersize = c.ClusterSize
        indicesCluster = c.ClusterIndex
        costo =  c.ClusterStrength
        Chi2 = c.ClusterChi2
        punto1 = c.ClusterFitP1
        #punto2 = c.ClusterFitP2

        ## m, b, 0, number inliers
        #print(punto1.x(), punto1.y(), punto1.z(),  clustersize )
        print(punto1[0], punto1[1],  clustersize )
        #print(punto2[0], punto2[1], punto2[2] )
        '''
        for i in range(clustersize):
                #print(indicesCluster[i], indicesCluster[i])
                print(vX[indicesCluster[i]],    vY[indicesCluster[i]],  k )

        k=k+1
    '''
