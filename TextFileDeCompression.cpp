#include <iostream>
#include <fstream>
#include <queue>
#include <string>

// For design purposes.
#include <time.h>
#include <unistd.h>

using namespace std;

struct NodeData {
    char data;
    int frequency;
    string huffmanCode;
    NodeData* left; NodeData* right;
    NodeData(char c, int freq) : data(c), frequency(freq), huffmanCode(""), left(NULL), right(NULL) {}
};


struct Node {
    NodeData* node;
    Node* next;
    Node(NodeData* bn) : node(bn), next(NULL) {}
};

class PriorityQueue {
private: Node* head;

public: PriorityQueue() : head(NULL) {}

    void enqueue(Node* newNode) {
        if (!head || newNode->node->frequency < head->node->frequency) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* current = head;
            while (current->next && newNode->node->frequency >= current->next->node->frequency) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    Node* dequeue() {
        if (!head) {
            cout << "Priority Queue is empty." << endl;
            return NULL;
        }
        Node* temp = head;
        head = head->next;
        return temp;
    }

    bool isEmpty() {
        return head == NULL;
    }

    Node* createTree() {
        while (head->next) {
            Node* left = dequeue();
            Node* right = dequeue();
            char combinedData = '\0';
            int combinedFrequency = left->node->frequency + right->node->frequency;
            Node* combinedNode = new Node(new NodeData(combinedData, combinedFrequency));
            combinedNode->node->left = left->node;
            combinedNode->node->right = right->node;
            enqueue(combinedNode);
        }
        return dequeue();
    }

    string decompressBinary(NodeData* root, string binaryString, size_t originalLength) {
        string decompressedString;
        NodeData* current = root;
        for (char bit : binaryString) {
            if (bit == '0') {
                current = current->left;
            } else {
                current = current->right;
            }
            if (current->data != '\0') {
                decompressedString += current->data;
                current = root;
                if (decompressedString.size() == originalLength) {
                    break;
                }
            }
        }
        return decompressedString;
    }
};

void delay(unsigned int secs)
{
    clock_t interval = clock() + secs;
    while(interval > clock());
}

void loading()
{
    string word = "DECOMPRESSING MESSAGE...";
    int t = 0;
    cout <<"\n\n\n\n\n\n\n\n\t\t\t\t    ";
    for (t = 0; word[t]; t++)
    {
        printf("%c", word[t]);
        fflush(stdout);
        usleep(100000);
    }
}

int main() {
    PriorityQueue pq;
    int frequencies[256] = {0};
    string binaryString;
    char byte;

    ifstream inFile("compressed.bin", ios::binary);
    if (!inFile.is_open()) {
        cout << "Error opening the compressed file!" << endl;
        return 1;
    }

    size_t originalLength;
    // Read the original length
    inFile >> originalLength;
    inFile.ignore(); // Ignore the newline character

    // Read the frequencies
    for (int i = 0; i < 256; i++) {
        inFile >> frequencies[i];
    }
    inFile.ignore(); // Ignore the newline character

    // Populate the priority queue with non-zero frequencies
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            NodeData* node = new NodeData(static_cast<char>(i), frequencies[i]);
            Node* newNode = new Node(node);
            pq.enqueue(newNode);
        }
    }
    Node* huffmanTree = pq.createTree();

    size_t bitsToRead = originalLength; // Keep track of how many bits to read
    while (inFile.get(byte) && (bitsToRead > 0 || bitsToRead % 8 != 0)) {
        for (int i = 7; i >= 0 && (bitsToRead > 0 || bitsToRead % 8 != 0); i--) {
            char bit = (byte & (1 << i)) ? '1' : '0';
            binaryString += bit;
            if (bitsToRead > 0) {
                bitsToRead--;
            }
        }
    }
    inFile.close();

    string decompressedSentence = pq.decompressBinary(huffmanTree->node, binaryString, originalLength);

    ofstream outFile("decompressed.txt");
    if (!outFile.is_open()) {
        cout << "Error opening the decompressed file!" << endl;
        return 1;
    }
    else{
        cout << "\n\t\t\t\t\t+---------------+";
        cout << "\n\t\t\t\t\t| DECOMPRESSION |";
        cout << "\n\t\t\t\t\t+---------------+";
        loading();
        delay(1000);
        cout << "\n\n\n\n\n\t\t\tYou can now look the message in [decompressed.txt]\n\n\n\n\n";
    }
    outFile << decompressedSentence;
    outFile.close();

    return 0;
}
