//
// Created by iansg on 4/16/2024.
//

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
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
    static const int startToLeft() {
        return 25*(int)sizeof(char) + 3*(int)sizeof(int);
    }
    static int startToRight() {
        return 25*(int)sizeof(char) + 4*(int)sizeof(int);
    }
};


class AVLFile {
    string filename;
    int pos {}; // position of root
    int n {}; // number of records
public:
    AVLFile(string filename) : filename(filename) {
        fstream f (filename.c_str(), ios::in | ios::out | ios::binary);
        if (!f.is_open()) {
            f.clear();
            f.open(filename.c_str(), ios::binary);
            f.write(reinterpret_cast<char*>(&pos), sizeof(int));
            f.write(reinterpret_cast<char*>(&n), sizeof(Record)-sizeof(int));
        }
        f.read(reinterpret_cast<char*>(&pos), sizeof(int));
        f.read(reinterpret_cast<char*>(&n), sizeof(int));
        f.close();
    }
    pair<int,bool> search(char key[]) { // returns pos of found record or last valid pos if not found
        int u {};
        int nu = pos;
        ifstream f (filename.c_str(), ios::binary);

        while (nu != u) {
            u = nu;
            f.seekg(u*(int)sizeof(Record));
            Record aux {};
            f.read(reinterpret_cast<char*>(&aux), sizeof(Record));
            int cmp = strcmp(key,aux.code);
            if (cmp==0) {
                f.close();
                return {u,true};
            }
            else if (cmp>0) {
                if (aux.right) nu = aux.right;
                else {
                    f.close();
                    return {u,false};
                };
            }
            else {
                if (aux.left) nu = aux.left;
                else {
                    f.close();
                    return {u,false};
                };
            }
        }

        f.close();
        return {u,false};
    }
    Record find(char key[]) {
        auto [u,found] = search(key);
        if (found) {
            ifstream f (filename.c_str(), ios::binary);
            Record record{};
            f.seekg(u*(int)sizeof(Record));
            f.read(reinterpret_cast<char*>(&record), sizeof(Record));
            f.close();
            return record;
        }
        return Record {};
    }
    void insert(Record record) {
        fstream f (filename.c_str(), ios::in | ios::out | ios::binary);
        if (++n==1) {
            f.seekp(sizeof(Record));
            f.write(reinterpret_cast<char*>(&record), sizeof(Record));
            pos = 1;
        }
        else {
            auto [u,found] = search(record.code);
            if (found) return;

            Record prev{};
            f.seekg(u*(int)sizeof(Record));
            f.read(reinterpret_cast<char*>(&prev), sizeof(Record));

            int cmp = strcmp(prev.code,record.code);
            if (cmp>0) {
                f.seekp(u*(int)sizeof(Record) + Record::startToRight());
                f.write(reinterpret_cast<char*>(&n), sizeof(int));
            }
            else {
                f.seekp(u*(int)sizeof(Record) + Record::startToLeft());
                f.write(reinterpret_cast<char*>(&n), sizeof(int));
            }

            f.seekp(n*(int)sizeof(Record));
            record.parent = u;
            record.height = prev.height + 1;
            f.write(reinterpret_cast<char*>(&record), sizeof(Record));
        }

        f.seekp(0);
        f.read(reinterpret_cast<char*>(&pos), sizeof(int));
        f.read(reinterpret_cast<char*>(&n), sizeof(int));

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
                    continue;
                }
                vis[u]=true;
                if (record.right) s.push(record.right);
                if (record.left) s.push(record.left);
            }
        }

        return records;
    }
};

void test() {

}

int main() {
    test();
    return 0;
}
