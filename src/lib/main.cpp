#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

vector<string> sequences;
vector<int> rewards;
int maxPoint = 0;
vector<pair<int, int>> optCoord;
string resToken;

// INPUT
void inputFromFile(int &buffer_size, int &nRow, int &nCol, int &sequences_length, vector<vector<string>> &matrix)
{
    string dir = "../test/input/";
    string name;
    cout << "Masukkan nama file: ";
    cin >> name;
    dir += name;
    cin.ignore(); // Ignore newline character

    ifstream file(dir);
    if (!file.is_open())
    {
        cerr << "Unable to open file: " << dir << endl;
        return;
    }

    // Read buffer_size, nCol, nRow from file
    file >> buffer_size;
    file >> nCol >> nRow;
    matrix.resize(nRow, vector<string>(nCol));

    // Read matrix data from file
    for (int i = 0; i < nRow; ++i)
    {
        for (int j = 0; j < nCol; ++j)
        {
            file >> matrix[i][j];
        }
    }

    // Read sequences and rewards from file
    file >> sequences_length;
    file.ignore(); // Ignore the newline character after reading sequences_length
    sequences.resize(sequences_length);
    rewards.resize(sequences_length);
    for (int i = 0; i < sequences_length; ++i)
    {
        getline(file >> std::ws, sequences[i]); // Read sequence line
        file >> rewards[i];                     // Read reward
        file.ignore();                          // Ignore the newline character after reading reward
    }

    file.close(); // Close the file
}

// random input
// Function to generate a random integer within a range
int getRandomNumber(int min, int max)
{
    static random_device rd;
    static mt19937 eng(rd());
    uniform_int_distribution<> distribution(min, max);
    return distribution(eng);
}

// Function to generate random sequences and rewards
void generateRandomSequencesAndRewards(int numberOfSequences, int maxSequenceLength,
                                       int tokenNumber, const vector<string> &tokens)
{
    sequences.clear();
    rewards.clear();

    for (int i = 0; i < numberOfSequences; ++i)
    {
        int rdLen = getRandomNumber(2, maxSequenceLength);
        string temp;

        for (int j = 0; j < rdLen; ++j)
        {
            int rdIndex = getRandomNumber(0, tokenNumber - 1);
            temp += tokens[rdIndex];
            if (j != rdLen - 1)
            {
                temp += " ";
            }
        }

        if (find(sequences.begin(), sequences.end(), temp) == sequences.end())
        {
            int reward = getRandomNumber(0, 1000);
            rewards.push_back(reward);
            sequences.push_back(temp);
        }
    }
}

// Function to generate a random matrix
vector<vector<string>> generateRandomMatrix(int row, int col, int tokenNumber, const vector<string> &uniqueToken)
{
    vector<vector<string>> matrix(row, vector<string>(col));

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            int rdIndex = getRandomNumber(0, tokenNumber - 1);
            matrix[i][j] = uniqueToken[rdIndex];
        }
    }

    return matrix;
}

// Function to get input from user that used to automate the variables
void inputRandom(int &buffer_size, int &nRow, int &nCol, int &sequences_length, vector<vector<string>> &matrix)
{
    // Token
    int nTokens;
    cin >> nTokens;

    string rawToken;
    getline(cin >> std::ws, rawToken);

    vector<string> uniqueToken;
    for (int i = 0; i < nTokens; i++)
    {
        uniqueToken.push_back(rawToken.substr(3 * i, 2)); // Extract a substring of length 2 starting from the index 3 * i + 1
    }

    // vector<string> uniqueToken = {"AA", "BB", "CC", "E9"};

    // Input buffer size
    cin >> buffer_size;

    // Matrix dimensions
    cin >> nCol >> nRow;

    // Sequences
    int nSeq;
    cin >> nSeq;

    int lengthSeq;
    cin >> lengthSeq;

    generateRandomSequencesAndRewards(nSeq, lengthSeq, nTokens, uniqueToken);
    sequences_length = sequences.size();
    // Assume rewards.size() == sequences.size()

    matrix = generateRandomMatrix(nRow, nCol, nTokens, uniqueToken);
}

// UTILS
// getPoint
int getPoint(string token)
{
    int point = 0;
    for (size_t i = 0; i < sequences.size(); i++)
    {
        if (token.find(sequences[i]) != string::npos)
        {
            point += rewards[i];
        }
    }
    return point;
}

// Recursive function to find all Token
void findToken(vector<vector<string>> &matrix, int x, int y, string token, int buffer_size, vector<vector<bool>> &visited, int steps, vector<pair<int, int>> coord)
{

    // Mark current position as visited and update the coord
    coord.push_back({x, y});

    // Check current point
    int currPoint = getPoint(token);
    if (currPoint > maxPoint || (currPoint == maxPoint && coord.size() > optCoord.size()))
    {
        maxPoint = currPoint;
        optCoord = coord;
        resToken = token;
    }

    // make sure the steps is still < buffer_size
    if (steps < buffer_size)
    {
        // Mark visited
        visited[x][y] = true;

        if (steps % 2 == 1)
        {
            // move vertically

            for (int i = 0; i < matrix.size(); i++)
            {
                if (!visited[i][y])
                {
                    findToken(matrix, i, y, token + " " + matrix[i][y], buffer_size, visited, steps + 1, coord);
                }
            }
        }

        else
        {
            // move horizontally
            for (int j = 0; j < matrix[0].size(); j++)
            {
                {
                    if (!visited[x][j])
                    {
                        findToken(matrix, x, j, token + " " + matrix[x][j], buffer_size, visited, steps + 1, coord);
                    }
                }
            }
        }
    }
    // Mark current position as unvisited when backtracking
    visited[x][y] = false;

    coord.push_back(make_pair(x, y));
}

// OUTPUT
void printResult(int time, char menu, int nRow, int nCol, vector<vector<string>> matrix)
{
    if (maxPoint == 0)
    {
        cout << "No solution" << endl;
    }
    else
    {
        if (menu == '2')
        {
            cout << "Matrix" << endl;
            for (int i = 0; i < nRow; ++i)
            {
                for (int j = 0; j < nCol; ++j)
                {
                    cout << matrix[i][j] << " ";
                }
                cout << endl;
            }

            cout << endl
                 << "Sequences" << endl;

            for (int i = 0; i < sequences.size(); ++i)
            {
                cout << "Sequence: " << sequences[i] << ", Reward: " << rewards[i] << endl;
            }
        }

        // Print max point, token, coordinats
        cout << maxPoint << endl;
        cout << resToken << endl;
        for (auto resCoord : optCoord)
        {
            cout << resCoord.second + 1 << ", " << resCoord.first + 1 << endl;
        }
        cout << endl;

        // Output the duration
        cout << time << " ms" << endl;
    }
}

// save to file
void saveToFile(int time)
{
    string dir = "../test/output/";
    string name;
    cout << "Masukkan nama file: ";
    cin >> name;
    dir += name;

    string maxPointStr = to_string(maxPoint);
    string resTokenStr = resToken;
    string timeStr = to_string(time);
    ostringstream oss;
    for (const auto &coord : optCoord)
    {
        oss << coord.second + 1 << ", " << coord.first + 1 << endl;
    }
    string resCoordStr = oss.str();

    ofstream outputFile(dir);
    if (outputFile.is_open())
    {
        if (maxPoint != 0)
        {
            outputFile << maxPointStr << endl;
            outputFile << resTokenStr << endl;
            outputFile << resCoordStr << endl;
            outputFile << timeStr << " ms" << endl;
            outputFile.close();
            cout << "Data telah disimpan pada " + dir << endl;
        }
        else
        {
            outputFile << "No Solution" << endl;
        }
    }
    else
    {
        cout << "Tidak dapat membuat file" << endl;
    }
}

int main()
{
    // Define Variables
    int buffer_size;
    int nRow;
    int nCol;
    int sequences_length;
    vector<vector<string>> matrix;

    // Initialize token
    string token;

    // Seed the random number generator
    srand(time(nullptr));

    // Get Input
    cout << "Menu :" << endl;
    cout << "1. Input from file" << endl;
    cout << "2. Input random " << endl;
    cout << "Masukkan pilihan: ";
    char menu;
    cin >> menu;

    if (menu == '1')
    {
        inputFromFile(buffer_size, nRow, nCol, sequences_length, matrix);
    }
    else if (menu == '2')
    {
        inputRandom(buffer_size, nRow, nCol, sequences_length, matrix);
    }
    else
    {
        cout << "Input tidak valid." << endl;
        return 0;
    }

    // Initialize the visited array
    vector<vector<bool>> visited(nRow, vector<bool>(nCol, false));

    // Get the current time before executing the code
    auto start = high_resolution_clock::now();

    // findToken starting from the first row
    for (int j = 0; j < matrix[0].size(); j++)
    {
        findToken(matrix, 0, j, matrix[0][j], buffer_size, visited, 1, {});
    }

    // Get the current time after executing the code
    auto stop = high_resolution_clock::now();

    // Calculate the duration by subtracting start time from stop time
    auto duration = duration_cast<milliseconds>(stop - start);

    int time = duration.count();

    printResult(time, menu, nRow, nCol, matrix);
    cout << endl
         << "Simpan solusi?(y/n) ";

    char save;
    cin >> save;
    if (save == 'y' || save == 'Y')
    {
        saveToFile(time);
    }

    cout << "Terima Kasih" << endl;

    return 0;
}
