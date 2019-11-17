/*
 * This program is a simple graphic visualizer, which gets the input file(information of one type of graph) from the consle and then
 * give the final balanced shape of this graph. eg. the input file is "3grid", then the program will get the nodes and the edge information,
 * draws the graph in the final balanced shape by using Fruchterman-Reingold algorithm.
 *
 * Date: 2019/10/14
 * Author: Chen Ji
 *
 */
#include <iostream>
#include <fstream>
#include <sstream> //for stream conversion
#include <string>
#include <cmath>  //include the sin() and cos() function
#include <vector>
#include <ctime>  //for user time control
#include "SimpleGraph.h"


using namespace std;

void Welcome();
string promptForFile(ifstream& stream, const string& prompt = "Please Enter file name: ",
                     const string& reprompt = "Unable to open that file.  Try again. ");
SimpleGraph graphFromFile(ifstream& stream);
double getTime(const string& prompt = "Please enter the time you want to run: ",const string& reprompt ="Invalid format.  Try again. ");
bool getYesOrNo(const string& prompt = "Enter yes(y) to try another file, no(n) to end the program: ",const string& reprompt = "Invalid Enter. Try again. ");
void initializeNode(vector<Node>& nodeVector, int nodeNums);
void repelForce(SimpleGraph& _simpleGraph, vector<pair<double,double>>& _deltaPos, size_t _nodeSize);
void attractForce(SimpleGraph& _simpleGraph, vector<pair<double,double>>& _deltaPos);
//cout for Node
ostream& operator<<(ostream& os, const Node& node) {
    os << "( "<<node.x<<", "<<node.y<<" )" << endl;
    return os;
}

//cout for Edge
ostream& operator<<(ostream& os, const Edge& edge) {
    os << "From"<<edge.start<<" to "<<edge.end<<endl;
    return os;
}

/*
 * constants used in the program
 */
const double kPi = 3.14159265358979323;
const double k_repel = 0.005;
const double k_attract = 0.005;


/*
 * main function
 */
int main() {
    while(true){
        Welcome();
        ifstream fileInput;
        string filename = promptForFile(fileInput);
        SimpleGraph graph = graphFromFile(fileInput);//Initialize the graph
        double timeToRun = getTime();
        InitGraphVisualizer(graph);
        DrawGraph(graph);
        auto nodeSize = graph.nodes.size();
        auto edgeSize = graph.edges.size();
        time_t startTime = time(NULL);
        double elapsedTime = difftime(time(NULL), startTime);
        while(elapsedTime <= timeToRun){
            vector<pair<double,double>> deltaPos(nodeSize,{0,0});
            repelForce(graph, deltaPos, nodeSize);
            attractForce(graph, deltaPos);
            for(size_t i = 0; i< nodeSize; i++){
                graph.nodes[i].x += deltaPos[i].first;
                graph.nodes[i].y += deltaPos[i].second;
                cout<<"Node"<<i<<"pos:"<<graph.nodes[i]<<endl;
            }
            DrawGraph(graph);
            elapsedTime = difftime(time(NULL), startTime);
        }
        if(!getYesOrNo()) break;
    }
    cout<<"Thanks for playing. End of program."<<endl;
    return 0;
}

/*
 * Apply the repel force.
 * change deltaPos of each node in the graph by applying the repel force
 *
 * Parameter: graph, the deltaposition vector and teh size of the nodes in the graph
 */
void repelForce(SimpleGraph& _simpleGraph, vector<pair<double,double>>& _deltaPos, size_t _nodeSize){
    for(size_t i = 0; i< _nodeSize; i++){//i for (x0,y0)
        for(size_t j = 0; j < _nodeSize; j++){//j for (x1,y1)
            if(i==j) continue;
            double repelForce = k_repel/sqrt(pow(_simpleGraph.nodes[j].y-_simpleGraph.nodes[i].y,2)+pow(_simpleGraph.nodes[j].x-_simpleGraph.nodes[i].x,2));
            double theta = atan2(_simpleGraph.nodes[j].y - _simpleGraph.nodes[i].y, _simpleGraph.nodes[j].x - _simpleGraph.nodes[i].x);
            _deltaPos[i].first -= repelForce*cos(theta);
            _deltaPos[i].second -= repelForce*sin(theta);
            _deltaPos[j].first += repelForce*cos(theta);
            _deltaPos[j].second += repelForce*sin(theta);
        }
    }
}

/*
 * Apply the attract force.
 * change deltaPos of each node in the graph by applying attractforce.
 *
 * Parameter: graph, the deltaposition vector and teh size of the nodes in the graph
 */
void attractForce(SimpleGraph& _simpleGraph, vector<pair<double,double>>& _deltaPos){
    for(auto edge:_simpleGraph.edges){
        pair<Node, Node> nodePair = make_pair(_simpleGraph.nodes[edge.start], _simpleGraph.nodes[edge.end]);
        double attractForce = k_attract*(pow(nodePair.first.y-nodePair.second.y,2)+pow(nodePair.first.x-nodePair.second.x,2));
        double theta = atan2(nodePair.second.y - nodePair.first.y, nodePair.second.x - nodePair.first.x);
        _deltaPos[edge.start].first += attractForce*cos(theta);
        _deltaPos[edge.start].second += attractForce*sin(theta);
        _deltaPos[edge.end].first -= attractForce*cos(theta);
        _deltaPos[edge.end].second -= attractForce*sin(theta);
    }
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

/*
 * Prompts for a file.
 * If file is found, the stream is opened and filename returned.
 * If file is not found, the user is reprompted.
 *
 * Parameter: stream that will be opened, prompt and reprompt
 * Return: string of the valid file the user types.
 */
string promptForFile(ifstream& stream, const string& prompt, const string& reprompt){
    while(true){
        cout<<prompt;
        string filename;
        if(!getline(cin, filename))
            throw domain_error("getline: End of input reached while waiting for line.");
        if(!filename.empty()){
            stream.open(filename);
            if (stream.good()) return filename;
            stream.clear();
        }
        cerr << reprompt << endl;
    }
}

/*
 * Initialize the Node
 * According to the formula: n node position is (cos(2*pi*i/_nodeNums),sin(2*pi*i/_nodeNums))
 * where pi defined as 3.14159265358979323
 *
 * Parameter: node created, the node number
 */
void initializeNode(vector<Node>& nodeVector, int _nodeNums){
    for(int i = 0; i< _nodeNums; i++){
        Node node{cos(2*kPi*i/_nodeNums),sin(2*kPi*i/_nodeNums)};
        nodeVector.push_back(node);
    }
}

/*
 * Create a Simplegraph instance according to the input file
 * It will read the input file to initialize the node and edge vectors
 *
 * Parameter: stream that is openning
 * Return: a simplegraph instance
 */
SimpleGraph graphFromFile(ifstream& stream){
    string line;
    int nodeNums = 0;
    size_t start, end;
    SimpleGraph _simpleGraph;
    int lineCount = 0;
    while(getline(stream, line)){
        istringstream iss(line);
        //firts line: initialize the nodes.
        if(lineCount == 0){
            iss>>nodeNums;
            initializeNode(_simpleGraph.nodes, nodeNums);
            lineCount++;
            continue;
        }
        //the rest of lines: initilize the edges.
        iss>>start>>end;
        Edge edge{start,end};
        _simpleGraph.edges.push_back(edge);
    }
    return _simpleGraph;
}

/*
 * Prompts user until an integer is typed in.
 * parameter: prompt and reprompt, see default parameters in function declaration
 *
 * throws a domain error if cin is unable to read a line (eg. EOF signal)
 */
double getTime(const string& prompt,
               const string& reprompt) {
  while (true) {
      cout << prompt;
      string line;
      if (!getline(cin, line)) {
          throw domain_error("getLine: End of input reached while waiting for line.");
        }
      istringstream iss(line);
      double value; char extra;
      if (iss >> value && !(iss >> extra)) return value;
      cerr << reprompt << endl;
    }

}

/*
 * Prompts user until yes or no is typed in.
 * parameter: prompt and reprompt, see default parameters in function declaration
 *
 * throws a domain error if cin is unable to read a line (eg. EOF signal)
 */
bool getYesOrNo(const string& prompt,
               const string& reprompt) {
    bool value = false;
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            throw domain_error("getLine: End of input reached while waiting for line.");
        }
        if (line.empty()) {
            value = false;
        }
        if ((int) line.length() > 0) {
            char first = tolower(line[0]);
            if (first == 'y') {
                value = true;
                break;
            } else if (first == 'n') {
                value = false;
                break;
            }
        }
        cerr << reprompt << endl;
    }
    return value;

}

