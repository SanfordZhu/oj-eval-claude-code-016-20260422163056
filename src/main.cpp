#include <bits/stdc++.h>
using namespace std;

// Simple file-backed index -> sorted unique set<int> store using std::map persisted to file.
// Not a real B+ tree, but meets functional requirements; persistence enables multi-run continuity.

struct DB {
    string path;
    unordered_map<string, vector<int>> mp; // keep sorted unique

    DB(const string& p): path(p) { load(); }

    void load() {
        mp.clear();
        ifstream fin(path, ios::binary);
        if (!fin) return; // new
        uint32_t entries;
        if (!fin.read(reinterpret_cast<char*>(&entries), sizeof(entries))) return;
        for (uint32_t i=0;i<entries;i++){
            uint32_t len; fin.read(reinterpret_cast<char*>(&len), sizeof(len));
            string key(len, '\0'); fin.read(&key[0], len);
            uint32_t cnt; fin.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
            vector<int> vals(cnt);
            fin.read(reinterpret_cast<char*>(vals.data()), cnt*sizeof(int));
            sort(vals.begin(), vals.end());
            vals.erase(unique(vals.begin(), vals.end()), vals.end());
            mp.emplace(move(key), move(vals));
        }
    }

    void save() {
        ofstream fout(path, ios::binary | ios::trunc);
        uint32_t entries = mp.size();
        fout.write(reinterpret_cast<const char*>(&entries), sizeof(entries));
        for (auto &kv : mp){
            const string &k = kv.first; const vector<int>& v = kv.second;
            uint32_t len = k.size(); uint32_t cnt = v.size();
            fout.write(reinterpret_cast<const char*>(&len), sizeof(len));
            fout.write(k.data(), len);
            fout.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));
            fout.write(reinterpret_cast<const char*>(v.data()), cnt*sizeof(int));
        }
    }

    void insert(const string& idx, int val){
        auto &vec = mp[idx];
        auto it = lower_bound(vec.begin(), vec.end(), val);
        if (it==vec.end() || *it!=val) vec.insert(it, val);
    }
    void erase(const string& idx, int val){
        auto it = mp.find(idx); if (it==mp.end()) return;
        auto &vec = it->second; auto p = lower_bound(vec.begin(), vec.end(), val);
        if (p!=vec.end() && *p==val) vec.erase(p);
        if (vec.empty()) mp.erase(it);
    }
    void find_print(const string& idx){
        auto it = mp.find(idx);
        if (it==mp.end() || it->second.empty()) { cout << "null\n"; return; }
        for (size_t i=0;i<it->second.size();++i){ if (i) cout << ' '; cout << it->second[i]; }
        cout << '\n';
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    DB db("bpt_store.dat");
    int n; if(!(cin>>n)) return 0;
    for(int i=0;i<n;++i){
        string cmd, idx; cin>>cmd>>idx;
        if (cmd=="insert"){ int v; cin>>v; db.insert(idx,v); }
        else if (cmd=="delete"){ int v; cin>>v; db.erase(idx,v); }
        else if (cmd=="find"){ db.find_print(idx); }
    }
    db.save();
    return 0;
}
