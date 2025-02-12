#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <fstream>

#define VERSION 2.0.0

/*
    Todo:
        implement file handling
            1. add cli parameter for file name
            2. read file and compress - done
            3. write to a new comperssed file

*/

using namespace std;

class node
{
public:
    char ch;
    int freq;
    node *left;
    node *right;

    node(char c, int f, node *l, node *r)
    {
        this->ch = c;
        this->freq = f;
        this->left = l;
        this->right = r;
    }
};

// function to find char freq using map
map<char, int> charFreq(string str)
{
    // populating freqTable map
    map<char, int> freqTable;
    for (char i : str)
        freqTable[i]++;

    // printing the freqTable map
    // this code should be remove afterwards
    // map<char, int>:: iterator it = freqTable.begin();;
    // while (it != freqTable.end()){
    //     cout << "Key: " << it->first << " Value: " << it->second << std::endl;
    //     ++it;
    // }

    return freqTable;
}

// overloaded function which creates freqTable with file
map<char, int> charFreq(ifstream &text)
{
    map<char, int> freqTable;
    char ch;
    while (text.get(ch))
    {
        freqTable[ch]++;
    }

    // printing the freqTable map
    // this code should be remove afterwards
    map<char, int>::iterator it = freqTable.begin();
    ;
    while (it != freqTable.end())
    {
        cout << "Key: " << it->first << " Value: " << it->second << std::endl;
        ++it;
    }
    return freqTable;
}

// object to compare frequency of two nodes
// used in priority queue
class comp
{
public:
    bool operator()(node *a, node *b)
    {
        return a->freq > b->freq;
    }
};

// function to build huffman tree
node *BuildHuffTree(map<char, int> freqTable)
{
    // creating a priority queue
    priority_queue<node *, vector<node *>, comp> pq;
    // wtf is auto??
    for (auto pair : freqTable)
    {
        // adding nodes to priority queue in increasing order of frequency
        pq.push(new node(pair.first, pair.second, NULL, NULL));
    }

    // creating the nodes
    while (pq.size() != 1)
    {
        // left node
        node *l = pq.top();
        pq.pop();
        // right node
        node *r = pq.top();
        pq.pop();

        int sum = l->freq + r->freq;
        // pushing null character containing the sum of the left and right nodes and having the left and right nodes
        // as its children
        pq.push(new node('\0', sum, l, r));
    }

    node *root = pq.top();
    return root;
}

void inorderTraversal(node *root)
{
    if (root == NULL)
        return;
    inorderTraversal(root->left);
    cout << root->ch << " ";
    inorderTraversal(root->right);
}

void printSummary(int stringLength, unordered_map<char, string> &huffMap, string para)
{
    int decodedLength = stringLength * 8;
    int encodedLength = 0;
    for (auto ch : para)
    {
        encodedLength += huffMap[ch].length(); // huffMap[ch] will give corresponding huffman binary string whose len we will add to encoded data
    }
    int mapSize = 0;
    for (auto pair : huffMap)
    {
        // adding 8 for every iteration since ascii represn of every char takes 8 bits
        mapSize += 8 + pair.second.length();
        ;
    }

    cout << endl;
    cout << "Compression Performance" << endl;
    cout << "Initial length: " << decodedLength << endl;
    cout << "Encoded length: " << encodedLength << endl;
    float ratio = float(encodedLength + mapSize) / float(decodedLength);
    cout << "Map size:" << mapSize << endl;
    cout << "Size reduced to :" << ratio * 100 << "%" << endl;
    cout << "Bits saved: " << decodedLength - (mapSize + encodedLength) << endl;
}

void buildCharToBinaryMapping(node *root, string bin, unordered_map<char, string> &huffMap)
{
    if (root == NULL)
        return;

    buildCharToBinaryMapping(root->left, bin + "0", huffMap);
    // jar current node cha character not equal to MrRightNa, then add it to the map along with its huffman bin reprsn
    if (root->ch != '\0')
    {
        cout << root->ch << " : " << bin << endl;
        huffMap[root->ch] = bin;
    }
    buildCharToBinaryMapping(root->right, bin + "1", huffMap);
}

string createEncodedString(string para, unordered_map<char, string> &HuffMap)
{
    string encoded = "";
    for (auto ch : para)
    {
        encoded += HuffMap[ch];
    }
    return encoded;
}

string createEncodedString(ifstream &text, unordered_map<char, string> &HuffMap)
{
    string encoded = "";
    char ch;
    while (text.get(ch))
    {
        encoded += HuffMap[ch];
    }
    return encoded;
}

string decodeEncodedString(string encodedStr, unordered_map<char, string> &HuffMap)
{
    string currentHuffStr = "";
    string decoded = "";
    for (auto ch : encodedStr)
    {
        currentHuffStr += ch;
        // check presence of given value to find the key in HuffMap by iterating over the map;
        for (auto pair : HuffMap)
        {
            if (pair.second == currentHuffStr)
            {
                decoded += pair.first;
                currentHuffStr = ""; // reset currentHuffString karan key sapadli
            }
        }
    }
    return decoded;
}

// unnecessary code
// takes text and huffMap as input and outputs the bin string
string encode(string para, unordered_map<char, string> huffMap)
{
    // evaluating the length of compressed string;
    int encodedLength;
    for (auto ch : para)
    {
        encodedLength += huffMap[ch].length();
    }
    // making char array of required length
    string output;
    output.reserve(encodedLength);
    for (auto ch : para)
    {
        output.append(huffMap[ch]);
    }
    return output;
}

// decode takes the bin string, huffman tree and converts it to text
vector<char> decode(node *root, string bin)
{
    node *rootCopy = root;
    int binItr = 0;
    vector<char> decodedText;
    while (binItr < bin.size())
    {
        if (root->ch == '\0' && bin[binItr] == '0')
        {
            root = root->left;
            binItr++;
        }
        else if (root->ch == '\0' && bin[binItr] == '1')
        {
            root = root->right;
            binItr++;
        }
        else if (root->ch != '\0')
        {
            decodedText.push_back(root->ch);
            root = rootCopy;
        }
    }
    decodedText.push_back(root->ch);
    return decodedText;
}

int main(int argc, char **argv)
{
    // string para = string("linus benedict torvalds is a finnish software engineer who is the creator and, historically, the lead developer of the linux kernel, used by linux distributions and other operating systems such as android. he also created the distributed version control system git");

    // if there are insufficient parameters
    /*
    if(argc != 3){
        cout<<"usage:"<<endl;
        cout<<"./main -parameter \"text that will be compressed\""<<endl;
        cout<<"parameters:\n\t-e : encode given text string\n\t-d : decode given bin string"<<endl;
        return 1;
    }
    */
    // if there are sufficient parameters

    // following block for file handling
    // if(strcmp(argv[1], "-f") == 0){

    ifstream text;
    // text.open(argv[2]);
    text.open("linusRizzLord.txt");
    if (!text.is_open())
    {
        std::cout << "File not found." << std::endl;
        return 1; // Return a non-zero value to indicate an error
    }

    // creating char frequencey map and huffman tree
    map<char, int> freqTable = charFreq(text);
    node *huffRoot = BuildHuffTree(freqTable);

    // creating huffMap from huffman tree
    cout << "Char to Bin Mapping" << endl;
    unordered_map<char, string> huffMap;
    buildCharToBinaryMapping(huffRoot, "", huffMap);

    // printSummary(input.length(), huffMap, input);

    // encoding the input string
    // text.seekg(0, ios::beg);        // set cursor to start of file
    text.close();
    text.open("linusRizzLord.txt"); // oddly closing and opening again solved problem
    string encoded = createEncodedString(text, huffMap);
    cout << "encoded string" << encoded << endl;

    // just need to figure out how to write bits to file
    return 0;
    // }
    /*
        // following block for cli program
        // storing text passed as argument into a string
        string input = argv[2];

        // creating char frequencey map and huffman tree
        map<char, int> freqTable = charFreq(input);
        node* huffRoot = BuildHuffTree(freqTable);

        // creating huffMap from huffman tree
        cout<<"Char to Bin Mapping"<<endl;
        unordered_map<char,string> huffMap;
        buildCharToBinaryMapping(huffRoot, "", huffMap);

        printSummary(input.length(), huffMap, input);

        // encoding the input string
        string encoded =  createEncodedString(input, huffMap);
        cout<<endl<<"Encoded string: "<<encoded<<endl;

        // decoding the input string
        string decoded = decodeEncodedString(encoded, huffMap);
        cout<<endl<<"Decoded string: "<<decoded<<endl;

        // comparing input and decoded string
        cout<<"\nChecking data integrity after decompression..."<<endl;
        if(input == decoded)
            cout<<"Success ;)"<<endl;
        else cout<<"Data Loss!!"<<endl;


        return 0;
    */
}