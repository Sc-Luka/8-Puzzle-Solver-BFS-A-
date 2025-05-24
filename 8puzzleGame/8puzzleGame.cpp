#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace std;

const int N = 3;

// Node structure representing a state in the puzzle
struct Node {
    int board[N][N];     // Puzzle board
    int x, y;            // Coordinates of the empty tile (0)
    int depth;           // Depth for BFS (number of moves from start)
    int cost;            // Cost for A* (g + h)
    Node* parent;        // Pointer to parent node

    // Constructor
    Node(int b[N][N], int x_, int y_, int d, Node* p = nullptr)
        : x(x_), y(y_), depth(d), parent(p) {
        memcpy(board, b, sizeof(board));
    }

    // Generates a unique string representation of the board
    string hash() const {
        string s;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                s += to_string(board[i][j]);
        return s;
    }
};

// Prints the solution path from the start to the current node
void printSolution(Node* node) {
    if (!node) return;

    vector<Node*> path;
    while (node) {
        path.push_back(node);
        node = node->parent;
    }
    reverse(path.begin(), path.end());

    for (int row = 0; row < N; ++row) {
        for (int step = 0; step < path.size(); ++step) {
            for (int col = 0; col < N; ++col) {
                cout << path[step]->board[row][col] << " ";
            }
            cout << "   ";
        }
        cout << endl;
    }
}

// Checks if the current board matches the goal state
bool isGoal(Node* node, int goal[N][N]) {
    return memcmp(node->board, goal, sizeof(node->board)) == 0;
}

// Generates all possible moves from the current node
vector<Node*> generateMoves(Node* node) {
    vector<Node*> moves;
    const int dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // Down, Up, Right, Left

    for (auto [dx, dy] : dir) {
        int nx = node->x + dx;
        int ny = node->y + dy;

        // Ensure the move is within bounds
        if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
            int newBoard[N][N];
            memcpy(newBoard, node->board, sizeof(newBoard));
            swap(newBoard[node->x][node->y], newBoard[nx][ny]);

            Node* child = new Node(newBoard, nx, ny, node->depth + 1, node);
            moves.push_back(child);
        }
    }
    return moves;
}

// Solves the puzzle using Breadth-First Search
void solveBFS(int initial[N][N], int goal[N][N], int x, int y) {
    unordered_set<string> visited;
    queue<Node*> q;

    Node* root = new Node(initial, x, y, 0);
    visited.insert(root->hash());
    q.push(root);

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();

        if (isGoal(current, goal)) {
            cout << "BFS solution found in " << current->depth << " moves!\n";
            printSolution(current);
            return;
        }

        for (Node* child : generateMoves(current)) {
            if (visited.find(child->hash()) == visited.end()) {
                visited.insert(child->hash());
                q.push(child);
            }
        }
    }

    cout << "No BFS solution found!\n";
}

// Solves the puzzle using A* Search
void solveAStar(int initial[N][N], int goal[N][N], int x, int y) {
    // Lambda function to calculate Manhattan distance heuristic
    auto heuristic = [goal](Node* node) {
        int h = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (node->board[i][j] == 0) continue;
                for (int gi = 0; gi < N; gi++) {
                    for (int gj = 0; gj < N; gj++) {
                        if (goal[gi][gj] == node->board[i][j]) {
                            h += abs(i - gi) + abs(j - gj);
                        }
                    }
                }
            }
        }
        return h;
    };

    // Priority queue with comparator based on A* cost
    auto comp = [](Node* a, Node* b) { return a->cost > b->cost; };
    priority_queue<Node*, vector<Node*>, decltype(comp)> pq(comp);
    unordered_set<string> visited;

    Node* root = new Node(initial, x, y, 0);
    root->cost = heuristic(root);
    pq.push(root);
    visited.insert(root->hash());

    while (!pq.empty()) {
        Node* current = pq.top();
        pq.pop();

        if (isGoal(current, goal)) {
            cout << "A* solution found in " << current->depth << " moves!\n";
            printSolution(current);
            return;
        }

        for (Node* child : generateMoves(current)) {
            if (visited.find(child->hash()) == visited.end()) {
                child->cost = child->depth + heuristic(child);
                pq.push(child);
                visited.insert(child->hash());
            }
        }
    }

    cout << "No A* solution found!\n";
}

// Main function to test the BFS and A* solvers
int main() {
    int initial[N][N] = {
        {1, 2, 3},
        {4, 0, 5},
        {6, 7, 8}
    };

    int goal[N][N] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    int x = 1, y = 1; // Starting coordinates of the empty tile (0)

    cout << "=== BFS ===\n";
    solveBFS(initial, goal, x, y);

    cout << "\n=== A* ===\n";
    solveAStar(initial, goal, x, y);

    return 0;
}
