#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 40
#define ROWS 4
#define COLUMNS 4
#define V (ROWS * COLUMNS)

typedef struct graph{
    int numOfNodes;
    bool** edges;
    bool** barcodes;
    bool** humps;
    char** directionsWhenNorth;
    char** directionsWhenSouth;
    char** directionsWhenWest;
    char** directionsWhenEast;
    //added for visited and adjacent nodes
    int* visited;
    struct node** adjacentNodes;
    //Added strct node to store array for DFS
    struct node** adjLists;
    int numVertices;
    int* visit;
}graph;

//queue struct
typedef struct queue {
    int list[SIZE];
    int front;
    int rear;
}queue;

//node struct
typedef struct node {
    int nodeNum;
    struct node* next;

}node;


//queue methods
struct queue* createQueue() {
    struct queue* queue = malloc(sizeof(struct queue));
    queue->front = -1;
    queue->rear = -1;

    return queue;
}

bool queueIsEmpty(struct queue* queue) {
    if (queue->rear == -1) {
        return true;
    }
    else {
        return false;
    }
}

bool queueIsFull(struct queue* queue) {
    if (queue->rear == SIZE - 1) {
        return true;
    }
    else {
        return false;
    }
}

void enqueue(struct queue* queue, int num) {
    //check if queue is full
    if (queueIsFull(queue)) {
        printf("Queue is full.\n");
    }
    else { //if not
        //increment rear by 1
        queue->rear++;
        queue->list[queue->rear] =num;
        //set front to 0
        if (queue->front == -1) {
            queue->front = 0;
        }
    }
}

int dequeue(struct queue* queue) {
    int removed;
    //check if is empty
    if (queueIsEmpty(queue)) {
        printf("Queue is empty.\n");
        removed = -1;
    }
    else { //if not
        //dequeue from front
        removed = queue->list[queue->front];
        queue->front++;
        if (queue->front > queue->rear) {
            queue->front = -1;
            queue->rear = -1;
        }
    }

    return removed;
}

//create node method
struct node* createNode(int n) {
    struct node* node = malloc(sizeof(struct node));
    node->nodeNum = n;
    node->next = NULL;
    return node;
}

//breadth-first search
void BFS(graph* graph, int startingPoint) {
    //create queue for all nodes
    struct queue* nodesList = createQueue();
    //create queue to store order to print at the end
    struct queue* visitedOrder = createQueue();

    //mark starting point as visited
    graph->visited[startingPoint] = 1;
    enqueue(nodesList, startingPoint);

    printf("\n");

    //while nodes queue has items
    while (!queueIsEmpty(nodesList)) {
        int currentNode = dequeue(nodesList);
        printf("Visited node: %d\n", currentNode);
        //add current node into visitedOrder list
        enqueue(visitedOrder, currentNode);

        //get adjacent nodes of currentNode
        //while there are adjacent nodes for currentNode
        while (graph->adjacentNodes[currentNode]) {
            //if current node number has not been visited yet
            if (graph->visited[graph->adjacentNodes[currentNode]->nodeNum] == 0) {
                //put into visited
                graph->visited[graph->adjacentNodes[currentNode]->nodeNum] = 1;
                //put adjacent nodes from current node into nodes queue
                enqueue(nodesList, graph->adjacentNodes[currentNode]->nodeNum);
            }
            //go to the next node
            graph->adjacentNodes[currentNode] = graph->adjacentNodes[currentNode]->next;
        }

    }

    printf("\nNodes visited in order: ");
    for (int i = visitedOrder->front; i < visitedOrder->rear + 1; i++) {
        printf("%d ", visitedOrder->list[i]);
    }
    printf("\n \n");
}

//Depth-first search
void DFS(graph* graph, int vertex) {
    struct node* adjList = graph->adjLists[vertex];
    struct node* temp = adjList;

    graph->visit[vertex] = 1;
    printf("Visited in order of DFS %d \n", vertex);

    while (temp != NULL) {
        int connectedVertex = temp->nodeNum;

        if (graph->visit[connectedVertex] == 0) {
            //put into visited
            DFS(graph, connectedVertex);
        }
        temp = temp->next;
    }
}


// Help function to find the node with the shortest distance, from the set of nodes not included in the shortest path tree set
int dijkstraMinDistance(int shortestDistance[], bool shortestSpanTreeSet[]) {
    // Initialise minimum value
    int min = INT_MAX, min_index;
    for (int v = 0; v < V; v++) {
        if (shortestSpanTreeSet[v] == false && shortestDistance[v] <= min) {
            min = shortestDistance[v], min_index = v;
        }
    }
    return min_index;
}


// Function to print shortest path from source to j using parent array
void printRouteOfShortestPath(int parent[], int j, int routeTaken[], int pos) {
    // Terminate if base case: j == src
    if (parent[j] == -1 ) {
        return;
    }

    // "Push' to routeTaken array. This array returns the steps taken by algo to traverse from src node to dest node
    pos--;
    routeTaken[pos] = j;

    // Recursive Call
    printRouteOfShortestPath(parent, parent[j], routeTaken, pos);

    printf("%d ", j);
}

// A helper function to print the shortestDistance array.
// This function also returns an array (routeTaken[]) which holds the all the nodes
// traversed in the algorithm (inclusive of start node and end node)
int printDijkstraSolution(int shortestDistance[], int parent[], int src, int dest) {
    // routeTaken array holds the nodes traversed from start to end
    int routeTaken[shortestDistance[dest] + 1];
    int pos = shortestDistance[dest] + 1;
    routeTaken[0] = src; // First element is start node

    // Printing out the vertex, distance, path taken to traverse from src node to dest node
    printf("Vertex\t  Distance\tPath");
    for (int i = 0; i < V; i++) {
        // Print only for shortest path
        if (i == dest) {
            printf("\n%d -> %d \t\t %d\t\t%d ", src, i, shortestDistance[i], src);
            printRouteOfShortestPath(parent, i, routeTaken, pos);
        }

    }

    routeTaken[pos-1] = dest; // Last element is end node

    // Print out the routes of the shortest path for checking
    printf("\nRoutes are:\n");
    for (int i = 0; i <= shortestDistance[dest]; i++) {
        printf("%d ", routeTaken[i]);
    }
    printf("\n");

    return routeTaken;
}

// Function that implements Dijkstra's algorithm on a graph represented using adjacency matrix
void dijkstraTraversal(graph* graph, int src, int dest) {
    // shortestDistance[i] holds the shortest distance from src to i
    int shortestDistance[V];

    // shortestSpanTreeSet[i] == true if node i is included in the shortestSpanTreeSet
    // or shortest distance from src to i is finalised
    bool shortestSpanTreeSet[V];

    // Parent array to store shortest path tree
    int parent[V];

    // Initialize all distances as infinity and shortestSpanTreeSet[] as false
    for (int i = 0; i < V; i++) {
        parent[0] = -1;
        shortestDistance[i] = INT_MAX;
        shortestSpanTreeSet[i] = false;
    }

    // Initialise distance of src node to itself == 0
    shortestDistance[src] = 0;

    // Find the shortest path for all nodes
    for (int count = 0; count < V-1; count++) {
        // Pick the minimum distance node from set of
        // nodes not yet traversed. u == src in first iteration.
        int u = dijkstraMinDistance(shortestDistance, shortestSpanTreeSet);

        // Mark the picked node as traversed
        shortestSpanTreeSet[u] = true;

        // Update shortestDistance value of the adjacent nodes of the picked node.
        for (int v = 0; v < V; v++) {
            // Update shortestDistance[v] only if (all 3 conditions are fulfilled)
            // 1) shortestDistance[v] is not in shortestSpanTreeSet[],
            // 2) there is an edge from u to v,
            // 3) and total distance of path from src to v through u is smaller than current value of shortestDistance[v]
            if (!shortestSpanTreeSet[v] &&
                graph->edges[u][v] &&
                shortestDistance[u] + graph->edges[u][v] < shortestDistance[v]) {
                parent[v]  = u;
                shortestDistance[v] = shortestDistance[u] + graph->edges[u][v];
            }
        }
    }

    // print dijkstra algorithm solution
    printDijkstraSolution(shortestDistance, parent, src, dest);

}

graph* createGraph(int rows,int columns){
    graph* result = malloc(sizeof(graph));
    //initialize adjacent nodes and visited using total number of nodes
    result->adjacentNodes = malloc(rows * columns * sizeof(struct node*));
    result->visited = malloc(rows * columns * sizeof(int));
    //for BFS
    result->adjLists = malloc(rows * columns * sizeof(struct node*));
    result->visit = malloc(rows * columns * sizeof(int));
    for (int i = 0; i < rows * columns; i++) {
        //set adjacent nodes to null and visited to 0
        result->adjacentNodes[i] = NULL;
        result->visited[i] = 0;

        result->adjLists[i] = NULL;
        result->visit[i] = 0;
    }

    if(result != NULL){
        result->numOfNodes = rows*columns;
        //allocate mem for EdgesMatrix
        //Rows
        result->edges = calloc(sizeof(bool*),result->numOfNodes);
        if(result->edges == NULL){
            destroyGraph(result);
            printf("EdgesMatrix cannot be created(1)");
        }
        //columns
        for (int i = 0; i < result->numOfNodes; i++){
            result->edges[i] = calloc(sizeof(bool*),result->numOfNodes);
            if(result->edges[i] == NULL){
                destroyGraph(result);
                printf("EdgesMatrix cannot be created(2)");
            }
        }
        //allocate mem for BarcodeMatrix
        result->barcodes = calloc(sizeof(bool*),result->numOfNodes);
        if(result->barcodes == NULL){
            destroyGraph(result);
            printf("BarcodesMatrix cannot be created(1)");
        }
        for (int i = 0; i < result->numOfNodes; i++){
            result->barcodes[i] = calloc(sizeof(bool*),result->numOfNodes);
            if(result->barcodes[i] == NULL){
                destroyGraph(result);
                printf("BarcodesMatrix cannot be created(2)");
            }
        }
        //allocate mem for HumpsMatrix
        result->humps = calloc(sizeof(bool*),result->numOfNodes);
        if(result->humps == NULL){
            destroyGraph(result);
            printf("HumpsMatrix cannot be created(1)");
        }
        for (int i = 0; i < result->numOfNodes; i++){
            result->humps[i] = calloc(sizeof(bool*),result->numOfNodes);
            if(result->humps[i] == NULL){
                destroyGraph(result);
                printf("HumpsMatrix cannot be created(2)");
            }
        }
        result->directionsWhenNorth = calloc(sizeof(char*),result->numOfNodes);
        if(result->directionsWhenNorth == NULL){
            destroyGraph(result);
            printf("DirectionsWhenNorthMatrix cannot be created(1)");
        }
        //columns
        for (int i = 0; i < result->numOfNodes; i++){
            result->directionsWhenNorth[i] = calloc(sizeof(char*),result->numOfNodes);
            if(result->directionsWhenNorth[i] == NULL){
                destroyGraph(result);
                printf("DirectionsWhenNorthMatrix cannot be created(2)");
            }
        }
        result->directionsWhenSouth = calloc(sizeof(char*),result->numOfNodes);
        if(result->directionsWhenSouth == NULL){
            destroyGraph(result);
            printf("DirectionsWhenSouthMatrix cannot be created(1)");
        }
        //columns
        for (int i = 0; i < result->numOfNodes; i++){
            result->directionsWhenSouth[i] = calloc(sizeof(char*),result->numOfNodes);
            if(result->directionsWhenSouth[i] == NULL){
                destroyGraph(result);
                printf("DirectionsWhenSouthMatrix cannot be created(2)");
            }
        }
        result->directionsWhenWest = calloc(sizeof(char*),result->numOfNodes);
        if(result->directionsWhenWest == NULL){
            destroyGraph(result);
            printf("DirectionsWhenWestMatrix cannot be created(1)");
        }
        //columns
        for (int i = 0; i < result->numOfNodes; i++){
            result->directionsWhenWest[i] = calloc(sizeof(char*),result->numOfNodes);
            if(result->directionsWhenWest[i] == NULL){
                destroyGraph(result);
                printf("DirectionsWhenWestMatrix cannot be created(2)");
            }
        }
        result->directionsWhenEast = calloc(sizeof(char*),result->numOfNodes);
        if(result->directionsWhenEast == NULL){
            destroyGraph(result);
            printf("DirectionsWhenEastMatrix cannot be created(1)");
        }
        //columns
        for (int i = 0; i < result->numOfNodes; i++){
            result->directionsWhenEast[i] = calloc(sizeof(char*),result->numOfNodes);
            if(result->directionsWhenEast[i] == NULL){
                destroyGraph(result);
                printf("DirectionsWhenEastMatrix cannot be created(2)");
            }
        }
        //sometimes calloc will not initialized everything to zero, so we 
        //explicitly have to initliase it.
        for (int i = 0; i < result->numOfNodes; i++){
            for (int j = 0; j < result->numOfNodes; j++){
                result->edges[i][j] = false;
                result->barcodes[i][j] = false;
                result->humps[i][j] = false;
                result->directionsWhenNorth[i][j] = 'N';
                result->directionsWhenSouth[i][j] = 'N';
                result->directionsWhenWest[i][j] = 'N';
                result->directionsWhenEast[i][j] = 'N';
            }
        }
    }
    return result;
}

void destroyGraph(graph* graph){
    if(graph->edges == NULL){
        free(graph);
    }
    for(int i = 0; i < graph->numOfNodes; i++){
        free(graph->edges[i]);
    }
    free(graph->edges);
}

void printEdges(graph* graph){
    printf("Edges Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf("%02d\n",graph->edges[i][j]);
            }
            else{
                printf("%02d ",graph->edges[i][j]);
            }
        }
    }
}
void printDirectionsWhenNorth(graph* graph){
    printf("DirectionsWhenNorth Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf(" %c\n",graph->directionsWhenNorth[i][j]);
            }
            else{
                printf(" %c ",graph->directionsWhenNorth[i][j]);
            }
        }
    }
}

void printDirectionsWhenSouth(graph* graph){
    printf("DirectionsWhenSouth Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf(" %c\n",graph->directionsWhenSouth[i][j]);
            }
            else{
                printf(" %c ",graph->directionsWhenSouth[i][j]);
            }
        }
    }
}

void printDirectionsWhenWest(graph* graph){
    printf("DirectionsWhenWest Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf(" %c\n",graph->directionsWhenWest[i][j]);
            }
            else{
                printf(" %c ",graph->directionsWhenWest[i][j]);
            }
        }
    }
}

void printDirectionsWhenEast(graph* graph){
    printf("DirectionsWhenEast Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf(" %c\n",graph->directionsWhenEast[i][j]);
            }
            else{
                printf(" %c ",graph->directionsWhenEast[i][j]);
            }
        }
    }
}

void printBarcodes(graph* graph){
    printf("Barcode Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf("%02d\n",graph->barcodes[i][j]);
            }
            else{
                printf("%02d ",graph->barcodes[i][j]);
            }
        }
    }
}
void printHumps(graph* graph){
    printf("Humps Matrix:\n");
    printf("   ");
    for(int i = 0; i<graph->numOfNodes; i++){
        if(i == graph->numOfNodes-1){
            printf("%02d\n", i);
        }
        else{
            printf("%02d ", i);
        }
    }
    for(int i = 0 ; i < graph->numOfNodes; i++){
        printf("%02d ", i);
        for(int j = 0; j < graph->numOfNodes; j++){
            if(j == graph->numOfNodes - 1){
                printf("%02d\n",graph->humps[i][j]);
            }
            else{
                printf("%02d ",graph->humps[i][j]);
            }
        }
    }
}

bool addDirection(graph* graph, int from_node, int to_node, char direction, char orientation){
    if(hasDirectionNorth(graph, from_node, to_node)!='N' && hasDirectionSouth(graph, from_node, to_node)!='N' 
    && hasDirectionWest(graph, from_node, to_node)!='N' && hasDirectionEast(graph, from_node, to_node)!='N'){
        return false;
    }
    else{
        if(orientation == 'N'){
            if(direction == 'F'){
                addDirectionNorth(graph, from_node, to_node, direction);
                addDirectionSouth(graph, from_node,to_node, 'B');
                addDirectionWest(graph, from_node,to_node,'R');
                addDirectionEast(graph,from_node,to_node,'L');
            }
            else if(direction == 'B'){
                addDirectionNorth(graph, from_node, to_node, direction);
                addDirectionSouth(graph, from_node,to_node, 'F');
                addDirectionWest(graph, from_node,to_node,'L');
                addDirectionEast(graph,from_node,to_node,'R');
            }
            else if(direction == 'L'){
                addDirectionNorth(graph, from_node, to_node, direction);
                addDirectionSouth(graph, from_node,to_node, 'R');
                addDirectionWest(graph, from_node,to_node,'F');
                addDirectionEast(graph,from_node,to_node,'B');
            }
            else if(direction == 'R'){
                addDirectionNorth(graph, from_node, to_node, direction);
                addDirectionSouth(graph, from_node,to_node, 'L');
                addDirectionWest(graph, from_node,to_node,'B');
                addDirectionEast(graph,from_node,to_node,'F');
            }   
        }
        else if(orientation == 'S'){
            if(direction == 'F'){
                addDirectionSouth(graph, from_node, to_node,direction);
                addDirectionNorth(graph,from_node,to_node, 'B');
                addDirectionWest(graph, from_node, to_node,'L');
                addDirectionEast(graph,from_node,to_node,'R');
            }
            else if(direction == 'B'){
                addDirectionSouth(graph, from_node, to_node,direction);
                addDirectionNorth(graph,from_node,to_node, 'F');
                addDirectionWest(graph, from_node, to_node,'R');
                addDirectionEast(graph,from_node,to_node,'L');
            }
            else if(direction == 'L'){
                addDirectionSouth(graph, from_node, to_node,direction);
                addDirectionNorth(graph,from_node,to_node, 'R');
                addDirectionWest(graph, from_node, to_node,'B');
                addDirectionEast(graph,from_node,to_node,'F');
            }
            else if(direction == 'R'){
                addDirectionSouth(graph, from_node, to_node,direction);
                addDirectionNorth(graph,from_node,to_node, 'L');
                addDirectionWest(graph, from_node, to_node,'F');
                addDirectionEast(graph,from_node,to_node,'B');
            }
        }
        else if(orientation == 'W'){
            if(direction == 'F'){
                addDirectionWest(graph,from_node,to_node,direction);
                addDirectionNorth(graph,from_node,to_node,'L');
                addDirectionSouth(graph,from_node,to_node, 'R');
                addDirectionEast(graph, from_node,to_node, 'B');
            }
            else if(direction == 'B'){
                addDirectionWest(graph,from_node,to_node,direction);
                addDirectionNorth(graph,from_node,to_node,'R');
                addDirectionSouth(graph,from_node,to_node, 'L');
                addDirectionEast(graph, from_node,to_node, 'F');
            }
            else if(direction == 'L'){
                addDirectionWest(graph,from_node,to_node,direction);
                addDirectionNorth(graph,from_node,to_node,'B');
                addDirectionSouth(graph,from_node,to_node, 'F');
                addDirectionEast(graph, from_node,to_node, 'R');
            }
            else if(direction == 'R'){
                addDirectionWest(graph,from_node,to_node,direction);
                addDirectionNorth(graph,from_node,to_node,'F');
                addDirectionSouth(graph,from_node,to_node, 'B');
                addDirectionEast(graph, from_node,to_node, 'L');
            }
        }
        else if(orientation == 'E'){
            if(direction == 'F'){
                addDirectionEast(graph,from_node,to_node,direction);
                addDirectionNorth(graph, from_node,to_node,'R');
                addDirectionSouth(graph,from_node, to_node,'L');
                addDirectionWest(graph,from_node,to_node,'B');
            }
            else if(direction == 'B'){
                addDirectionEast(graph,from_node,to_node,direction);
                addDirectionNorth(graph, from_node,to_node,'L');
                addDirectionSouth(graph,from_node, to_node,'R');
                addDirectionWest(graph,from_node,to_node,'F');
            }
            else if(direction == 'L'){
                addDirectionEast(graph,from_node,to_node,direction);
                addDirectionNorth(graph, from_node,to_node,'F');
                addDirectionSouth(graph,from_node, to_node,'B');
                addDirectionWest(graph,from_node,to_node,'R');
            }
            else if(direction == 'R'){
                addDirectionEast(graph,from_node,to_node,direction);
                addDirectionNorth(graph, from_node,to_node,'B');
                addDirectionSouth(graph,from_node, to_node,'F');
                addDirectionWest(graph,from_node,to_node,'L');
            }
        }
        return true;
    }
}

bool addDirectionNorth(graph* graph, int from_node, int to_node , char direction){
    if(hasDirectionNorth(graph, from_node, to_node) != 'N'){
        return false;
    }
    else{
        graph->directionsWhenNorth[from_node][to_node] = direction;
        if(direction == 'F'){
            graph->directionsWhenNorth[to_node][from_node] = 'B';
        }
        else if(direction == 'B'){
            graph->directionsWhenNorth[to_node][from_node] = 'F';
        }
        else if(direction == 'L'){
            graph->directionsWhenNorth[to_node][from_node] = 'R';
        }
        else if(direction == 'R'){
            graph->directionsWhenNorth[to_node][from_node] = 'L';
        }
        printf("Direction Added!\n");
        return true;
    }
}
char hasDirectionNorth(graph* graph, int from_node, int to_node){
    return graph->directionsWhenNorth[from_node][to_node];
}

bool addDirectionSouth(graph* graph, int from_node, int to_node , char direction){
    if(hasDirectionSouth(graph, from_node, to_node) != 'N'){
        return false;
    }
    else{
        graph->directionsWhenSouth[from_node][to_node] = direction;
        if(direction == 'F'){
            graph->directionsWhenSouth[to_node][from_node] = 'B';
        }
        else if(direction == 'B'){
            graph->directionsWhenSouth[to_node][from_node] = 'F';
        }
        else if(direction == 'L'){
            graph->directionsWhenSouth[to_node][from_node] = 'R';
        }
        else if(direction == 'R'){
            graph->directionsWhenSouth[to_node][from_node] = 'L';
        }
        printf("Direction Added!\n");
        return true;
    }
}
char hasDirectionSouth(graph* graph, int from_node, int to_node){
    return graph->directionsWhenSouth[from_node][to_node];
}

bool addDirectionWest(graph* graph, int from_node, int to_node , char direction){
    if(hasDirectionWest(graph, from_node, to_node) != 'N'){
        return false;
    }
    else{
        graph->directionsWhenWest[from_node][to_node] = direction;
        if(direction == 'F'){
            graph->directionsWhenWest[to_node][from_node] = 'B';
        }
        else if(direction == 'B'){
            graph->directionsWhenWest[to_node][from_node] = 'F';
        }
        else if(direction == 'L'){
            graph->directionsWhenWest[to_node][from_node] = 'R';
        }
        else if(direction == 'R'){
            graph->directionsWhenWest[to_node][from_node] = 'L';
        }
        printf("Direction Added!\n");
        return true;
    }
}
char hasDirectionWest(graph* graph, int from_node, int to_node){
    return graph->directionsWhenWest[from_node][to_node];
}

bool addDirectionEast(graph* graph, int from_node, int to_node , char direction){
    if(hasDirectionEast(graph, from_node, to_node) != 'N'){
        return false;
    }
    else{
        graph->directionsWhenEast[from_node][to_node] = direction;
        if(direction == 'F'){
            graph->directionsWhenEast[to_node][from_node] = 'B';
        }
        else if(direction == 'B'){
            graph->directionsWhenEast[to_node][from_node] = 'F';
        }
        else if(direction == 'L'){
            graph->directionsWhenEast[to_node][from_node] = 'R';
        }
        else if(direction == 'R'){
            graph->directionsWhenEast[to_node][from_node] = 'L';
        }
        printf("Direction Added!\n");
        return true;
    }
}
char hasDirectionEast(graph* graph, int from_node, int to_node){
    return graph->directionsWhenEast[from_node][to_node];
}

bool addEdge(graph* graph, int from_node, int to_node){
    if(hasEdge(graph, from_node, to_node)){
        return false;
    }
    else{
        graph->edges[from_node][to_node] = true;
        graph->edges[to_node][from_node] = true;
        printf("Edge Added!\n");

        //adding for nodes
        struct node* node = createNode(from_node);
        node->next = graph->adjacentNodes[to_node];
        graph->adjacentNodes[to_node] = node;
    // for BFS
        node->next = graph->adjLists[to_node];
        graph->adjLists[to_node] = node;

        node = createNode(to_node);
        node->next = graph->adjacentNodes[from_node];
        graph->adjacentNodes[from_node] = node;
        //for BFS
        node->next = graph->adjLists[from_node];
        graph->adjLists[from_node] = node;

        return true;
    }
}
bool hasEdge(graph* graph, int from_node, int to_node){
    return graph->edges[from_node][to_node];
}

bool addBarcode(graph* graph, int from_node, int to_node){
    if(hasBarcode(graph, from_node, to_node)){
        return false;
    }
    else{
        graph->barcodes[from_node][to_node] = true;
        graph->barcodes[to_node][from_node] = true;
        printf("Barcode Added!\n");
        return true;
    }
}
bool hasBarcode(graph* graph, int from_node, int to_node){
    return graph->barcodes[from_node][to_node];
}

bool addHump(graph* graph, int from_node, int to_node){
    if(hasHump(graph, from_node, to_node)){
        return false;
    }
    else{
        graph->humps[from_node][to_node] = true;
        graph->humps[to_node][from_node] = true;
        printf("Hump Added!\n");
        return true;
    }
}
bool hasHump(graph* graph, int from_node, int to_node){
    return graph->humps[from_node][to_node];
}

void printGraph(graph* graph){
    printf("graph {\n");
    for (int from = 0; from < graph->numOfNodes; from++){
        for (int to = 0; to < graph->numOfNodes; to++){
            if(graph->edges[from][to]){
                printf("%d -- %d", from, to);
                if(graph->humps[from][to]){
                    printf("[label=\"h\"]");
                }
                if(graph->barcodes[from][to]){
                    printf("[label=\"b\"]");
                }
                printf(";\n");
            }
        }
    }
    printf("}\n");
}

void printMap(graph* graph, int rows, int columns){
    int map[rows][columns];
    int counter = 0;
    for(int i=0; i<rows;i++){
        for(int j = 0; j<columns; j++){
            map[i][j] = counter++;
        }
    }
    printf("\n");
    for (int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(j == columns-1){
                printf("(%03d)", map[i][j]);
                if(hasEdge(graph,map[i][j],map[i][j+1])){
                    printf("-");
                    if(hasBarcode(graph,map[i][j],map[i][j+1])){
                        printf("b");
                    }
                    else if(hasHump(graph,map[i][j],map[i][j+1])){
                        printf("h");
                    }
                    else{
                        printf("-");
                    }
                    printf("-");
                }
                printf("\n");
                for(int k = 0; k < columns; k++){
                    if(hasEdge(graph, map[i][k],map[i+1][k])){
                        printf("|");
                        if(hasHump(graph, map[i][k],map[i+1][k])){
                            printf("h");
                        }
                        else if(hasBarcode(graph, map[i][k],map[i+1][k])){
                            printf("b");
                        }
                        else{
                            printf("|");
                        }
                        printf("|\t");
                    }
                    else{
                        if(i != rows-1){
                            printf("|X|\t");
                        }
                    }
                }
                printf("\n");
            }
            else{
                printf("(%03d)", map[i][j]);
                if(hasEdge(graph,map[i][j],map[i][j+1])){
                    printf("-");
                    if(hasBarcode(graph,map[i][j],map[i][j+1])){
                        printf("b");
                    }
                    else if(hasHump(graph,map[i][j],map[i][j+1])){
                        printf("h");
                    }
                    else{
                        printf("-");
                    }
                    printf("-");
                }
                else{
                    printf("-X-");
                }
            }
        }
    }
}

void printAllMatrix(graph* graph){
    printEdges(graph);
    printHumps(graph);
    printBarcodes(graph);
    printDirectionsWhenNorth(graph);
    printDirectionsWhenSouth(graph);
    printDirectionsWhenWest(graph);
    printDirectionsWhenEast(graph);
}

void whatDirectionDoITake(graph* graph, int from_node, int to_node){
    printf("From grid %d to %d\n", from_node, to_node);
    if(hasDirectionNorth(graph,from_node,to_node)!='N'){
        printf("If North: Go %c\n",graph->directionsWhenNorth[from_node][to_node]);
    }
    else{
        printf("If North: NULL\n");
    }
    if(hasDirectionSouth(graph,from_node,to_node)!='N'){
        printf("If South: Go %c\n",graph->directionsWhenSouth[from_node][to_node]);
    }
    else{
        printf("If South: NULL\n");
    }
    if(hasDirectionWest(graph,from_node,to_node)!='N'){
        printf("If West: Go %c\n",graph->directionsWhenWest[from_node][to_node]);
    }
    else{
        printf("If West: NULL\n");
    }
    if(hasDirectionEast(graph,from_node,to_node)!='N'){
        printf("If East: Go %c\n",graph->directionsWhenEast[from_node][to_node]);
    }
    else{
        printf("If East: NULL\n");
    }
    
}

bool checkAllNodesHasAtLeast1Edge(graph* graph){
    int counter = 0;
    for(int i = 0; i < graph->numOfNodes; i++){
        int counter2 =0;
        for(int j =0; j< graph->numOfNodes; j++){
            if(graph->edges[i][j] == true){
                counter2 += 1;
            }
            if(j == graph->numOfNodes-1 && counter2 != 0){
                counter += 1;
            }
        }
        //printf("Node %d has %d edges.\n",i,counter2);
    }
    //printf("Counter: %d\n",counter);
    if(counter == graph->numOfNodes){
        printf("All Nodes have at least 1 edge!\n");
        return true;
    }
    else{
        printf("Some nodes don't have an edge!\n");
        return false;
    }
}