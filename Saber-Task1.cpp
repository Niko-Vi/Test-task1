#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <iterator>

class ListNode {
public:
    ListNode* Prev;
    ListNode* Next;
    ListNode* Rand; // произвольный элемент внутри списка
    std::string Data;

    ListNode() : Prev(this), Next(this), Rand(this), Data("") {
    }
    ListNode(std::string str) {
        Data = str;
        Prev = this;
        Next = this;
        Rand = this;
    }
    ListNode(std::string str, ListNode* Prev) {
        Data = str;
        this->Prev = Prev;
        Next = this;
        Rand = this;
    }
};

class IteratorLN {
    ListNode* ptr;
public:
    IteratorLN() {
        ptr = new ListNode("empty");
    }
    IteratorLN(ListNode* p) {
        ptr = p;
    }
    ~IteratorLN(){
    if(ptr->Data == "empty"){
        delete ptr;
    }
    }

    IteratorLN* operator++() {
        if (ptr->Next != nullptr) {
            ptr = ptr->Next;
        }
        return this;
    }
    bool operator==(ListNode* LN) {
        if (ptr == LN) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator!=(ListNode* LN) {
        if (ptr == LN) {
            return false;
        }
        else {
            return true;
        }
    }
    ListNode* operator->() {
        return ptr;
    }
    ListNode* operator*() {
        return ptr;
    }
};

class ListRand {
public:
    ListNode* Head;
    ListNode* Tail;
    int Count;

    ListRand() : Head(nullptr), Tail(nullptr), Count(0) {}

    ListRand(std::string firstNodeData) {
        Head = new ListNode(firstNodeData);
        Tail = Head;
        Count = 1;
    }

    ~ListRand() {
        for (int i = 0; i < Count; ++i) {
            if (Tail != nullptr) {
                if (Tail->Prev != nullptr) {
                    Head = Tail->Prev;
                    delete Tail;
                    Tail = Head;
                }
                else {
                    delete Tail;
                    Tail = nullptr;
                }
            }
        }
    }

    void addNode(std::string newNodeData) {
        if (Count == 0) {
            Head = new ListNode(newNodeData);
            Tail = Head;
            Count = 1;
        }
        else {
            Tail->Next = new ListNode(newNodeData, Tail);
            Tail = Tail->Next;
            Count++;
        }
    }

    void setRand(int fromNodeNum, int toNodeNum) {
        IteratorLN it = Head;
        IteratorLN rIt = Head;
        for (int i = 0; i < fromNodeNum; ++i) {
            if (it->Next != nullptr) {
                ++it;
            }
            else {
                std::cerr << "There is no Node #" << fromNodeNum << ". Last Node is #" << i << std::endl;
                break;
            }
        }
        for (int i = 0; i < toNodeNum; ++i) {
            if (rIt->Next != nullptr) {
                ++rIt;
            }
            else {
                std::cerr << "There is no Node #" << toNodeNum << ". Last Node is #" << i << std::endl;
                break;
            }
        }
        it->Rand = *rIt;
    }

    void Serialize(std::ofstream& s) {
        std::map<ListNode*, int> pMap; // map need to save keys for Rand elements
        IteratorLN lnIt = Head;
        int i;
        for (i = 1; i <= Count; ++i) { // fill map with keys, 0 = nothing
            pMap.insert({ *lnIt, i });
            ++lnIt;
        }

        lnIt = Head;
        std::map<ListNode*, int>::iterator mIt;
        for (i = 0; i < Count; ++i) { // save keys in first line
            if (lnIt->Rand != nullptr) {
                mIt = pMap.find(lnIt->Rand);
                s << mIt->second << " ";
            }
            else {
                s << NULL << " "; // 0 key is for no Rand elem
            }
            ++lnIt;
        }

        s << std::endl;

        lnIt = Head;
        for (i = 0; i < Count; ++i) {
            s << lnIt->Data << std::endl; // writing data to file
            ++lnIt;
        }
    }

    void Deserialize(std::ifstream& s) {
        std::stringstream buf1;
        std::string buf;
        int key, i;
        std::vector<int> keys;
        std::vector<ListNode*> pointers;
        IteratorLN iter;

        std::getline(s, buf); // reading keys from first line
        buf1 << buf;
        while (buf1 >> key) {
            keys.push_back(key);
        }
        buf1.clear();

        while (std::getline(s, buf)) {//creating nodes from strings of data
            addNode(buf);
        }
        iter = Head;
        for (i = 0; i < Count; i++) { //writing down pointers for direct access
            pointers.push_back(*iter);
            ++iter;
        }
        iter = Head;
        for (i = 0; i < Count; i++) {// setting rands
            iter->Rand = pointers[keys[i]-1];
            ++iter;
        }
    }
};

int main()
{
    ListRand lr0("node 0");
    lr0.addNode("node 1");
    lr0.addNode("node 2");
    lr0.addNode("node 3");
    lr0.addNode("node 4");

    lr0.setRand(0, 3);
    lr0.setRand(1, 0);
    lr0.setRand(2, 4);
    lr0.setRand(3, 2);
    lr0.setRand(4, 1);

    IteratorLN it = lr0.Head;
    int i;
    for (i = 0; i < lr0.Count; ++i) { // print Rands
        if (it->Rand != nullptr) {
            std::cout << it->Rand->Data << std::endl;
        }
        ++it;
    }

    std::ofstream save("test.txt", std::ios::binary);
    lr0.Serialize(save);
    save.close();

    ListRand lr1;
    std::ifstream load("test.txt", std::ios::binary);
    lr1.Deserialize(load);
    load.close();

    it = lr1.Head;
    std::cout << std::endl;
    for (i = 0; i < lr1.Count; ++i) { //print Rands again for check if all right
        if (it->Rand != nullptr) {
            std::cout << it->Rand->Data << std::endl;
        }
        ++it;
    }

    return 0;
}

