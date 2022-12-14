#include "../main.h"

/**
 * @brief Postorder file has the following structure
 * NodeWithPostOrderIndex1 \n 
 * NodeWithPostOrderIndex2 \n
 * NodeWithPostOrderIndex3 \n
 * NodeWithPostOrderIndex4 \n
 * .... \n
 * 
 * 
 * @param filename 
 * @param SocialGraph 
 */
void readPostorder(string filename, Graph* SocialGraph){
    cout << "Read postorder ";
    ifstream file;
    file.open("data/postorder/" + filename);
    string line;
    int counter = 1;

    int node;
    if (file.is_open()){
        while (file >> node) {
            SocialGraph->nodeHasPostorder[counter] = node;
            SocialGraph->postOrderWithIndex[node] = counter;
            SocialGraph->postOrder.push_back(node);
            counter++;
        }
    }
    cout << counter << " points read\n" << endl;
    file.close();
}

/**
 * @brief SCC file has the following structure
 * SccNode \t childNodeOne \t childNodeTwo \t ... \n
 * SccNodeTwo \t childNodeOne \t childNodeTwo \t ... \n
 * SccNodeThree \t childNodeOne \t childNodeTwo \t ... \n
 * ... \n”
 * @param filename 
 * @param SocialGraph 
 */
void readSuperConnectedComponents(string filename, Graph* SocialGraph){
    cout << "Read strongly connected components" << endl;
    ifstream file;
    file.open("data/strongly_connected_components/" + filename);
    string line;
    int sccNode, node;
    bool sccDoesntExist = true;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            stringstream   linestream(line);
            string  nodeString;
            while (getline(linestream, nodeString, '\t')) {
                node = stoi(nodeString);
                if (sccDoesntExist) {
                    sccNode = node;
                    sccDoesntExist = false;
                }

                if (sccNode != node) {
                    SocialGraph->SuperConnectedComponents[sccNode].push_back(node);
                    SocialGraph->NodeBelongsToSCC[node] = sccNode;
                }
            }
            sccDoesntExist = true;
        }
    }
    file.close();
    cout << "scc file read" << endl;
}

/**
 * @brief Spatial data file needs to have the following strucutre
 * 
 * For Points:
 *       Node \t xPos \t yPos \t xPos \t yPos \n
 * For Mbrs (multiple points)
 *       Node \t xPosLowest \t yPosLowest \t xPosHighest \t yPosHighest \t xPosNodeOne \t yPosNodeOne \t xPosNodeTwo \t yPosNodeTwo \t ... \n
 * ..... \n
 * 
 * @param filename 
 * @param SpatialGraph 
 */
void readSpatialData(string filename, LocationMap* SpatialGraph){
    cout << "Read spatial data" << endl;
    vector<coordinates> _minMaxCorners {float(INT_MAX), float(INT_MAX), float(INT_MIN), float(INT_MIN)};

    ifstream file;
    file.open("data/reduced_spatial_data/" + filename);
    string line;
    vector<coordinates> SpatialData;
    int node;
    int counter = 0;
    
    if (file.is_open()) {
        while (getline(file, line))
        {
            counter++;
            stringstream linestream(line);
            string data;
            while (getline(linestream, data, '\t')) {
                SpatialData.push_back(stof(data));
            }
            node = SpatialData[0];
            SpatialData.erase(SpatialData.begin());

            bool isMbr;
            if (SpatialData[0] != SpatialData[2] || SpatialData[1] != SpatialData[3])
            {
                isMbr = true;
            }else
            {
                isMbr = false;
            }
            SpatialGraph->Map[node] = spatialMbrRelation(isMbr, SpatialData);

        
            if (SpatialData[0] < _minMaxCorners[0]) _minMaxCorners[0] = SpatialData[0];
            if (SpatialData[1] < _minMaxCorners[1]) _minMaxCorners[1] = SpatialData[1];
            if (SpatialData[2] > _minMaxCorners[2]) _minMaxCorners[2] = SpatialData[2];
            if (SpatialData[3] > _minMaxCorners[3]) _minMaxCorners[3] = SpatialData[3];        
            SpatialData.clear();
        }
    }
    SpatialGraph->MinMaxCorners = _minMaxCorners;
    cout << counter << " points loaded." << endl;
}

/**
 * @brief Interval scheme needs to have the following structure
 * NodeOne \t intervalOne.pre \t intervalOne.post \t intervalOne.pre \t intervalTwo.post \n 
 * NodeTwo \t intervalOne.pre \t intervalOne.post \t intervalOne.pre \t intervalTwo.post \n 
 * .... \n
 * @param filename 
 * @param SocialGraph 
 */
void readIntervalScheme(string filename, Graph *SocialGraph){
    cout << "Load interval scheme from file... " << endl;
    ifstream file;
    file.open("data/interval_scheme/" + filename);
    string line;
    vector<int> IntervalData;
    int node;
    int counter  = 0;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            stringstream   linestream(line);
            string  data;
            while (getline(linestream, data, '\t')) {
                IntervalData.push_back(stoi(data));
            }
            for (int i = 2; i < IntervalData.size(); i = i+2)
            {
                counter++;
                SocialGraph->IntervalSchemeGraphMap[IntervalData[0]].push_back(IntervalScheme(IntervalData[i - 1], IntervalData[i]));
            }
            counter++;
            IntervalData.clear();
        }
    }
    file.close();
    cout << counter << " intervals loaded." << endl;
}

/**
 * @brief Query file needs to have following structure
 * QueryNodeOne \t xMin \t yMin \t xMax \t yMax \n
 * QueryNodeTwo \t xMin \t yMin \t xMax \t yMax \n
 *  .... \n
 * @param filename 
 * @return vector<queryParameter> 
 */
vector<queryParameter> readQueries(string filename){
    vector<queryParameter> queries;
    ifstream queryFile;
    int node, cardinality, degree;
    coordinates xMin, yMin, xMax, yMax;
    int counter = 0;
    float area;
    queryFile.open("data/queries/" + filename);
    if (queryFile.is_open()) {
        string str;
        getline(queryFile, str);
        while (queryFile >> node >> degree >> xMin >> yMin >> xMax >> yMax >>  area >> cardinality) {
            box area_window;
            counter++;
            // cout << node << " " << degree << " " << xMin << " " << yMin << " " << xMax << " " << yMax << " " << area << " " << cardinality << "\t"; 
            queries.push_back(queryParameter(node, box(point(xMin, yMin), point(xMax, yMax)), degree, cardinality));
        }
    }
    cout << counter << endl;
    cout << queries.size() << " queries loaded.\n";
    return queries;
}


/**
 * @brief Reduced graph file needs to have the following structure
 * sourceNode \t reachableNodeOne \t reachableNodeTwo \t .... \n
 * .... \n
 * @param filename 
 * @param SocialGraph 
 */
void readReducedGraph(string filename, Graph* SocialGraph){
    SocialGraph->GraphScheme.clear();
    cout << "Read reduced graph file: ";
    ifstream file;
    file.open("data/reduced_graph/" + filename);

    bool isRootNode = true;
    int counter = 0;
    int rootNode, node;
    string line;
    if (file.is_open())
    {
        while (getline(file,  line))
        {
            stringstream   linestream(line);
            string  nodeString;
            while (getline(linestream, nodeString, '\t')) {
                node = stoi(nodeString);
                if (isRootNode) {
                    rootNode = node;
                    isRootNode = false;
                }
                counter++;
                if(rootNode != node){
                    SocialGraph->GraphScheme[rootNode].push_back(node);
                    SocialGraph->GraphSchemeReverse[node].push_back(rootNode);
                }
            }
            isRootNode = true;
        }
    }
    cout << counter << " lines read.\n";
    file.close();
}

void readBflForNodeIdentifer(string filename, Graph* SocialGraph){
    cout << "Read bfl Identifier" << endl;
    ifstream file;
    file.open("data/bfl/" + filename);
    int source, target;
    if (file.is_open()){
        while (file >> source >> target) {
            SocialGraph->nodeToBflIdentifier[source] = target;
            SocialGraph->bflIdentifierToNode[target] = source;
        }
    }
    cout << "bfl file read\n";
    file.close();
}