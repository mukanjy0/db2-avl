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
    int height {-1};
    int parent {};
    int left {};
    int right {};
    static int dataSize() {
        return 25*(int)sizeof(char) + (int)sizeof(int);
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
//        out << r.code << '|' << r.height << '|' << r.parent << '|' << r.left << '|' << r.right << '\n';
        return out;
    }
};


class AVLFile {
    string filename;
    int n {}; // number of records
    int rootPos {}; // rootPosition of root

    void readHeader() {
        ifstream f (filename.c_str(), ios::binary);
        f.read(reinterpret_cast<char*>(&n), sizeof(int));
        f.read(reinterpret_cast<char*>(&rootPos), sizeof(int));
        f.close();
    }
    void writeHeader() {
        ofstream f (filename.c_str(), ios::in | ios::binary);
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
        f.write(reinterpret_cast<char*>(&rootPos), sizeof(Record)-sizeof(int));
        f.close();
    }
    Record readRecord(int pos) {
        if (pos > n || pos < 0)
            throw runtime_error("Invalid position for record.");

        if (pos == 0) return Record {};

        ifstream f (filename.c_str(), ios::binary);
        f.seekg(pos * (int)sizeof(Record));
        Record record {};
        f.read(reinterpret_cast<char*>(&record), sizeof(Record));
        f.close();
        return record;
    }
    void writeRecord(Record& record, int pos) {
        if (pos <= 0 || pos > n)
            throw runtime_error("Invalid position for record.");

        ofstream f (filename.c_str(), ios::in | ios::binary);
        f.seekp(pos * (int) sizeof(Record));
        f.write(reinterpret_cast<char *>(&record), sizeof(Record));
        f.close();
    }
    void lr(Record& root, int rtPos, Record& l, int lPos, Record& lr, int lrPos) {
        int lrlPos = lr.left;
        Record lrl = readRecord(lrlPos);
        int lrrPos = lr.right;
        Record lrr = readRecord(lrrPos);

        l.right = lr.left;
        lrl.parent = lPos;

        root.left = lrrPos;
        lrr.parent = rtPos;

        if (root.parent) {
            int rtParentPos = root.parent;
            Record rtParent = readRecord(rtParentPos);

            lr.parent = rtParentPos;
            (rtParent.right == rtPos) ? rtParent.right = lrPos : rtParent.left = lrPos;

            writeRecord(rtParent, rtParentPos);
        }
        else rootPos = lrPos;

        lr.left = lPos;
        l.parent = lrPos;

        lr.right = rtPos;
        root.parent = lrPos;

        lr.height += 1;
        l.height -= 1;

        writeRecord(root, rtPos);
        writeRecord(l, lPos);
        writeRecord(lr, lrPos);
        writeRecord(lrl, lrlPos);
        writeRecord(lrr, lrrPos);
    }
    void rl(Record& root, int rtPos, Record& r, int rPos, Record& rl, int rlPos) {
        int rllPos = rl.left;
        Record rll = readRecord(rllPos);
        int rlrPos = rl.left;
        Record rlr = readRecord(rlrPos);

        root.right = rllPos;
        rll.parent = rtPos;

        r.left = rlrPos;
        rlr.parent = rPos;

        if (root.parent) {
            int rtParentPos = root.parent;
            Record rtParent = readRecord(rtParentPos);

            rl.parent = rtParentPos;
            (rtParent.right == rtPos) ? rtParent.right = rlPos : rtParent.left = rlPos;

            writeRecord(rtParent, rtParentPos);
        }
        else rootPos = rlPos;

        rl.left = rtPos;
        root.parent = rlPos;

        rl.right = rPos;
        r.left = rlPos;

        rl.height += 1;
        r.height -= 1;

        writeRecord(root, rtPos);
        writeRecord(r, rPos);
        writeRecord(rl, rlPos);
        writeRecord(rll, rllPos);
        writeRecord(rlr, rlrPos);
    }
    void ll(Record& root, int rtPos, Record& l, int lPos) {
        int lrPos = l.right;
        Record lr = readRecord(lrPos);

        root.left = l.right;
        lr.parent = rtPos;

        if (root.parent) {
            int rtParentPos = root.parent;
            Record rtParent = readRecord(rtParentPos);

            l.parent = rtParentPos;
            (rtParent.right == rtPos) ? rtParent.right = lPos : rtParent.left = lPos;

            writeRecord(rtParent, rtParentPos);
        }
        else rootPos = lPos;

        l.right = rtPos;
        root.parent = lPos;

        root.height -= 1;
        l.height += 1;

        writeRecord(root, rtPos);
        writeRecord(l, lPos);
        writeRecord(lr, lrPos);
    }
    void rr(Record& root, int rtPos, Record& r, int rPos) {
        int rlPos = r.left;
        Record rl = readRecord(rlPos);

        root.right = r.left;
        rl.parent = rtPos;

        if (root.parent) {
            int rtParentPos = root.parent;
            Record rtParent = readRecord(root.parent);

            r.parent = rtParentPos;
            (rtParent.right == rtPos) ? rtParent.right = rPos : rtParent.left = rPos;

            writeRecord(rtParent, rtParentPos);
        }
        else rootPos = rPos;

        r.left = rtPos;
        root.parent = rPos;

        root.height -= 1;
        r.height += 1;

        writeRecord(root, rtPos);
        writeRecord(r, rPos);
        writeRecord(rl, rlPos);
    }

public:
    explicit AVLFile(const string& filename) : filename(filename) {
        fstream f (filename.c_str(), ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();
        if (exists) readHeader();
        else {
            ofstream of (filename.c_str(), ios::binary);
            of.write(reinterpret_cast<char*>(&n), sizeof(int));
            of.write(reinterpret_cast<char*>(&rootPos), sizeof(int));
            of.close();
        }
    }

    pair<int,bool> search(char* key) { // returns rootPos of found record or last valid rootPos if not found
        int u {rootPos};

        while (u) {
            Record cur = readRecord(u);
            int cmp = strcmp(key,cur.code);
            if (cmp==0) return {u,true};
            else if (cmp>0) {
                if (cur.right) u = cur.right;
                else break;
            }
            else {
                if (cur.left) u = cur.left;
                else break;
            }
        }

        return {u,false};
    }

    Record find(char* key) {
        auto [u,found] = search(key);
        return found ? readRecord(u) : Record{};
    }

    void insert(Record record) {
        if (++n==1) {
            rootPos = 1;
            writeRecord(record, rootPos);
            writeHeader();
            return;
        }

        auto [u,found] = search(record.code);
        if (found) { --n; return; }

        Record parent = readRecord(u);
        (strcmp(record.code, parent.code)>0) ? parent.right=n : parent.left=n;
        writeRecord(parent, u);

        record.parent = u;
        writeRecord(record, n);

        Record child = record;
        int childPos = n, parentPos = u, height {1};
        while (parentPos > 0) {
            if (parent.height < height) {
                Record otherChild {};
                if (childPos == parent.right) {
                    otherChild = readRecord(parent.left);

                    if (height - otherChild.height > 2) { // (height - 1) = child.height
                        Record rightSubChild = readRecord(child.right);
                        if (rightSubChild.height + 1 != height) {
                            Record childLeft = readRecord(child.left);
                            rl(parent, parentPos, child, childPos, childLeft, child.left);
                        }
                        else {
                            rr(parent, parentPos, child, childPos);
                        }

                        parentPos = 0;
                    }
                    else {
                        parent.height = height;
                        writeRecord(parent, parentPos);

                        childPos = parentPos;
                        parentPos = parent.parent;
                        child = readRecord(childPos);
                        parent = readRecord(parentPos);
                    }
                }
                else {
                    otherChild = readRecord(parent.right);
                    if (height - otherChild.height > 2) { // (height - 1) = child.height
                        Record leftSubChild = readRecord(child.left);
                        if (leftSubChild.height + 1 != height) {
                            Record childRight = readRecord(child.right);
                            lr(parent, parentPos, child, childPos, childRight, child.right);
                        }
                        else {
                            ll(parent, parentPos, child, childPos);
                        }

                        parentPos = 0;
                    }
                    else {
                        parent.height = height;
                        writeRecord(parent, parentPos);

                        childPos = parentPos;
                        parentPos = parent.parent;
                        child = readRecord(childPos);
                        parent = readRecord(parentPos);
                    }
                }
            }
            else parentPos = 0;
            ++height;
        }

        writeHeader();
//        Record r = readRecord(rootPos);
//        cout << r << '\n';
    }

    vector<Record> inOrder() {
        if (n == 0) return vector<Record>{};

        stack<int> s;
        s.push(rootPos);

        int i {};
        vector<Record> records;
        vector<bool> vis (n+1);

        while (!s.empty()) {
            int u = s.top();
            Record record = readRecord(u);

            if (vis[u]) {
                records[i++]=record;
                s.pop();
                if (record.right) s.push(record.right);
                continue;
            }

            vis[u]=true;
            if (record.left) s.push(record.left);
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
        cout << avl.find(r.code);
//        assert(avl.find(r.code) == r);
    }
//    vector<Record> v = avl.inOrder();
//    cout << (int)v.size() << '\n';
//    for (const auto& r : v)
//        cout << r;
}

int main() {
    test();
    return 0;
}
