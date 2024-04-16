//
// Created by iansg on 4/16/2024.
//

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cassert>
#include <queue>
#include <stack>
#include <vector>
#include <iomanip>
using namespace std;

struct Record {
    char code[5]{};
    char name[20]{};
    int semester {};
    int height {};
    int parent {};
    int left {};
    int right {};
    static int dataSize() {
        return 25*(int)sizeof(char) + (int)sizeof(int);
    }
    static int startToLeft() {
        return 25*(int)sizeof(char) + 3*(int)sizeof(int);
    }
    static int startToRight() {
        return 25*(int)sizeof(char) + 4*(int)sizeof(int);
    }
    friend bool operator==(const Record& r1, const Record& r2) {
        return strcmp(r1.code,r2.code)==0 && strcmp(r1.name,r2.name)==0
            && r1.semester==r2.semester;
    }
    friend bool operator!=(const Record& r1, const Record& r2) {
        return !(r1==r2);
    }
    friend ostream& operator<<(ostream& out, const Record& r){
//        out << r.code << '|' << r.name << '|' << r.semester << '\n';
        out << r.code << '|' << r.name << '|' << r.semester << '|' << r.left << '|' << r.right << '\n';
        return out;
    }
};


class AVLFile {
    string filename;
    int pos {}; // position of root
    int n {}; // number of records
public:
    explicit AVLFile(const string& filename) : filename(filename) {
        fstream f (filename.c_str(), ios::in | ios::out | ios::binary);
        if (!f.is_open()) {
            f.clear();
            f.open(filename.c_str(), ios::out | ios::binary);
            f.write(reinterpret_cast<char*>(&pos), sizeof(int));
            f.write(reinterpret_cast<char*>(&n), sizeof(Record)-sizeof(int));
        }
        else {
            f.read(reinterpret_cast<char*>(&pos), sizeof(int));
            f.read(reinterpret_cast<char*>(&n), sizeof(int));
        }
        f.close();
    }
    pair<int,bool> search(char* key) { // returns pos of found record or last valid pos if not found
        int u {pos};
        ifstream f (filename.c_str(), ios::binary);

        while (u) {
            f.seekg(u*(int)sizeof(Record));
            Record aux {};
            f.read(reinterpret_cast<char*>(&aux), sizeof(Record));
            int cmp = strcmp(key,aux.code);
            if (cmp==0) {
                f.close();
                return {u,true};
            }
            else if (cmp>0) {
                if (aux.right) u = aux.right;
                else {
                    f.close();
                    return {u,false};
                };
            }
            else {
                if (aux.left) u = aux.left;
                else {
                    f.close();
                    return {u,false};
                };
            }
        }

        f.close();
        return {u,false};
    }
    Record find(char* key) {
        auto [u,found] = search(key);
        if (found) {
            ifstream f (filename.c_str(), ios::binary);
            Record record{};
            f.seekg(u*(int)sizeof(Record));
            f.read(reinterpret_cast<char*>(&record), sizeof(Record));
            f.close();
            //cout << record;
            return record;
        }
        return Record {};
    }
    void insert(Record record) {
        fstream f (filename.c_str(), ios::in | ios::out | ios::binary);
        if (++n==1) {
            pos = 1;
            f.seekp(sizeof(Record));
            f.write(reinterpret_cast<char*>(&record), sizeof(Record));
        }
        else {
            auto [u,found] = search(record.code);
            if (found) {
                f.close();
                --n;
                return;
            }


            Record prev{};
            f.seekg(u*(int)sizeof(Record));
            f.read(reinterpret_cast<char*>(&prev), sizeof(Record));

            int cmp = strcmp(record.code, prev.code);
            if (cmp>0) {
//                f.seekp(u*(int)sizeof(Record) + Record::startToRight());
//                f.write(reinterpret_cast<char*>(&n), sizeof(int));
                prev.right=n;
//                cout << prev << record;
                f.seekp(u*(int)sizeof(Record));
                f.write(reinterpret_cast<char*>(&prev), sizeof(Record));
            }
            else {
                f.seekp(u*(int)sizeof(Record) + Record::startToLeft());
                f.write(reinterpret_cast<char*>(&n), sizeof(int));
            }

            record.parent = u;
            record.height = prev.height + 1;
            f.seekp(n*(int)sizeof(Record));
            f.write(reinterpret_cast<char*>(&record), sizeof(Record));
        }

        f.seekp(0);
        f.write(reinterpret_cast<char*>(&pos), sizeof(int));
        f.write(reinterpret_cast<char*>(&n), sizeof(int));

        f.close();
    }
    vector<Record> inOrder() {
        vector<Record> records (n);
        if (n) {
            stack<int> s;
            s.push(pos);

            ifstream f (filename.c_str(), ios::binary);
            int i {};
            vector<bool> vis (n+1);
            while (!s.empty()) {
                int u = s.top();
                Record record {};
                f.seekg(u*(int)sizeof(Record));
                f.read(reinterpret_cast<char*>(&record), sizeof(Record));

                if (vis[u]) {
                    records[i++]=record;
                    s.pop();
                    if (record.right) s.push(record.right);
                    continue;
                }
                vis[u]=true;
                if (record.left) s.push(record.left);
            }
            f.close();
        }

        return records;
    }
};

void test() {
    Record records[16] = {
            {"0001", "John Smith", 1,0,0,0,0},
            {"0002", "Emily Johnson", 2,0,0,0,0},
            {"0003", "Michael Williams", 3,0,0,0,0},
            {"0004", "Emma Jones", 4,0,0,0,0},
            {"0005", "Daniel Brown", 5,0,0,0,0},
            {"0006", "Olivia Davis", 6,0,0,0,0},
            {"0007", "Matthew Miller", 7,0,0,0,0},
            {"0008", "Sophia Wilson", 8,0,0,0,0},
            {"0009", "James Taylor", 1,0,0,0,0},
            {"0010", "Ava Anderson", 2,0,0,0,0},
            {"0011", "David Martinez", 3,0,0,0,0},
            {"0012", "Isabella Jackson", 4,0,0,0,0},
            {"0013", "Joseph Harris", 5,0,0,0,0},
            {"0014", "Charlotte Moore", 6,0,0,0,0},
            {"0015", "Christopher Lee", 7,0,0,0,0},
            {"0016", "Mia Thompson", 8,0,0,0,0}
    };
    AVLFile avl ("data.bin");
    for (auto& r : records)
        avl.insert(r);
    for (auto& r : records) {
//        cout << avl.find(r.code);
        assert(avl.find(r.code) == r);
    }
    vector<Record> v = avl.inOrder();
    cout << (int)v.size() << '\n';
    for (const auto& r : v)
        cout << r;
}

int main() {
    test();
    return 0;
}
