#include <iostream>
#include <fstream>
#include <string>
#include <queue>

// For design purposes.
#include <time.h>
#include <unistd.h>


using namespace std;

struct NodeData {
    char data;
    int frequency;
    string huffmanCode;
    NodeData* left;
    NodeData* right;
    NodeData(char c, int freq) : data(c), frequency(freq), huffmanCode(""), left(NULL), right(NULL) {}
};

struct Node {
    NodeData* node;
    Node* next;
    Node(NodeData* bn) : node(bn), next(NULL) {}
};

class PriorityQueue {
private:
    Node* head;

public:
    PriorityQueue() : head(NULL) {}

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

    void assignHuffmanCode(NodeData* root, string code) {
        if (!root) {
            return;
        }
        assignHuffmanCode(root->left, code + "0");
        if (root->data != '\0') {
            root->huffmanCode = code;
            cout << "\t     " << " " << static_cast<int>(root->data) << "\t\t" << root->data << "\t\t    " << root->frequency << "\t\t\t" << root->huffmanCode << endl;
        }
        assignHuffmanCode(root->right, code + "1");
    }


    string BinaryString(NodeData* root, const string& sentence) {
        string binaryString;
        for (char c : sentence) {
            binaryString += getBinaryCode(root, c);
        }
        return binaryString;
    }


    string getBinaryCode(NodeData* root, char character) {
        if (!root) {
            return "";
        }

        if (root->data == character) {
            return root->huffmanCode;
        }

        string leftSearch = getBinaryCode(root->left, character);
        if (!leftSearch.empty()) {
            return leftSearch;
        }
        return getBinaryCode(root->right, character);
    }


    void printCompressedBinary(string binaryString, int frequencies[]) {
        int originalLength = binaryString.length();

        // Pad the binary string with zeros if necessary
        if (binaryString.length() % 8 != 0) {
            while (binaryString.length() % 8 != 0) {
                binaryString += "0";
            }
        }

        ofstream outFile("compressed.bin", ios::binary);
        if (!outFile.is_open()) {
            cout << "Error opening the compressed file!" << endl;
            exit(1);
        }

        // Write the original length to the file
        outFile << originalLength << "\n";

        // Write the frequencies to the file
        // Remove the space at the end of the line
        for (int i = 0; i < 256; i++) {
            outFile << frequencies[i] << (i == 255 ? "" : " ");
        }
        outFile << "\n";  // Add a newline at the end

        for (size_t i = 0; i < binaryString.length(); i += 8) {
            string eightBits = binaryString.substr(i, 8);
            if (eightBits.find_first_not_of("01") != string::npos) {
                cout << "Invalid binary string: " << eightBits << endl;
                exit(1);
            }
            int decimalValue = stoi(eightBits, nullptr, 2);
            char byteValue = static_cast<char>(decimalValue);
            outFile.put(byteValue);
        }
        //Delay output design for binary string.
        int t = 0;
        for (t = 0; binaryString[t]; t++)
        {
            printf("%c", binaryString[t]);
            fflush(stdout);
            usleep(100000);
        }
        outFile.close();
    }
};

void delay(unsigned int secs)
{
    clock_t interval = clock() + secs;
    while(interval > clock());
}

void loading()
{
    system("cls");

    string word = "COMPRESSING MESSAGE...";
    int t = 0;
    cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t";
    for (t = 0; word[t]; t++)
    {
        printf("%c", word[t]);
        fflush(stdout);
        usleep(100000);
    }
}

int main() {
    PriorityQueue pq;
    char ch;
    string sentence;
    int frequencies[256] = {0};

    ifstream file("FILE.txt");
    if (!file.is_open()) {
        cout << "Error opening the file!" << endl;
        return 1;
    }

    cout << "\n\t\t\t\t\t+---------------+";
    cout << "\n\t\t\t\t\t| DECOMPRESSION |";
    cout << "\n\t\t\t\t\t+---------------+";
    cout << "\n\n\nInput Message: ";
    while (file.get(ch)) {
        cout << ch;
        sentence += ch;
    }
    cout << "\n\n" << endl;
    file.close();

    for (char c : sentence) {
        frequencies[static_cast<unsigned char>(c)]++;
    }

    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            NodeData* node = new NodeData(static_cast<char>(i), frequencies[i]);
            Node* newNode = new Node(node);
            pq.enqueue(newNode);
        }
    }

    Node* huffmanTree = pq.createTree();


    system("pause");
    system("cls");
    cout << "\n\t\t\t\t\t+-------------+";
    cout << "\n\t\t\t\t\t| COMPRESSION |";
    cout << "\n\t\t\t\t\t+-------------+";
    cout << "\n\t+---------------------------------------------------------------------------------+" << endl;
    cout << "\t|ASCII DECIMAL\tCHARACTER/SYMBOL\tFREQUENCY\tHUFFMAN CODE (BINARY CODE)|" << endl;
    cout << "\t+---------------------------------------------------------------------------------+" << endl;
    pq.assignHuffmanCode(huffmanTree->node, "");
    cout << "\t+---------------------------------------------------------------------------------+" << endl;
    cout << "\n\nBinary String representation of the sentence:                                    \n" << endl;
    string binarySentence = pq.BinaryString(huffmanTree->node, sentence);
    pq.printCompressedBinary(binarySentence, frequencies);
    cout << "\n\n\n" << endl;

    system("pause");
    loading();
    delay(1000);
    system("cls");

    ifstream messagefile("FILE.txt");
    if (!messagefile.is_open()) {
        cout << "Error opening the file!" << endl;
        return 1;
    }
    cout << "\nThe message [";
    while (messagefile.get(ch)) {
        cout << ch;
        sentence += ch;
    }
    cout << "] compressed successfully." << endl;
    //remove("FILE.txt");
    file.close();

    return 0;
}
