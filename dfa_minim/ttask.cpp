#include "api.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

DFA dfa_minim(DFA &d) {
    Alphabet alph = d.get_alphabet();

    DFA ans(alph);
    vector<set<long long>> G(3000);

    long long ds = d.size();
    if (ds == 0) {
        return d;
    }

    G[1].insert(ds);

    stringstream ss(d.to_string());
    string vr_i;
    getline(ss, vr_i);

    map<string, long long> sts;

    for (long long i = 0; i < ds; ++i) {
        string vr_i;
        getline(ss, vr_i);
        string vrr_i = "";
        long long vr = 0;

        for (auto x : vr_i) {
            if (x == '[' || x == ']')
                ++vr;
            else
                vrr_i += x;
        }

        sts["[" + vrr_i + "]"] = i;
        if (vr == 4) {
            G[0].insert(i);
        } else {
            G[1].insert(i);
        }
    }

    map<pair<long long, char>, long long> tr;
    set<long long> dos;

    for (long long i = 0; i < ds; ++i) 
        dos.insert(i);

    while (true) {
        string vr_i;
        getline(ss, vr_i);
        if (vr_i.size() == 0)
            break;

        stringstream sss(vr_i);

        char symb;
        string fro, t;

        sss >> fro >> symb >> t;

        long long from = sts[fro];
        long long to = sts[t];

        tr[{from, symb}] = to;
        dos.erase(to);
    }
    dos.erase(0);

    for (long long i = 0; i < ds; ++i) {
        if (dos.count(i))
            continue;
        for (auto x : alph) {
            if (tr.count({i, x}) == 0) 
                tr[{i, x}] = ds;

        }
    }
    for (auto x : alph) 
        tr[{ds, x}] = ds;

    for(auto x : dos) {
        if (x != 0) {
            G[0].erase(x);
            G[1].erase(x);
        }
    }

    set<long long> act = {0, 1};
    long long f_f = 2;

    while (true) {
        set<long long> new_act;
        for (auto x : act) {
            map<vector<long long>, long long> n_act;

            for (auto from : G[x]) {
                vector<long long> vec;

                for (auto el : alph) {
                    long long to = tr[{from, el}];
                    for (auto tt : act) {
                        if (G[tt].count(to)) {
                            vec.push_back(tt);
                            break;
                        }
                    }
                } 

                if (!n_act.count(vec)) {
                    n_act[vec] = f_f++;
                }
                
                G[n_act[vec]].insert(from);
            }

            for (auto y : n_act) 
                new_act.insert(y.second);
        }

        if (new_act.size() == act.size()) 
            break;
        act = new_act;
    }


    long long itm = 0, era = 0;
    set<long long> dsc;

    map<long long, long long> nnew;
    long long nf = 0;

    for (auto x : act) {
        bool flag = false, flf = false;

        if (G[x].count(0))
            flag = true;
        for (auto y : G[0]) {
            if (G[x].count(y)) {
                flf = true;
                break;
            }
        }

        if (G[x].count(ds)) {
            dsc.insert(G[x].begin(), G[x].end());
            era = x;
        } else {
            nnew[x] = nf++;
            ans.create_state(to_string(nnew[x]), flf);
            if (flag)
                ans.set_initial(to_string(nnew[x]));
        }
    }
    act.erase(era);

    for (auto x : act) {
        for (auto y : G[x]) {
            for (auto z : alph) {
                if (dsc.count(tr[{y, z}]) == 0) {
                    for (auto tt : act) {
                        if (G[tt].count(tr[{y, z}])) {
                            ans.set_trans(to_string(nnew[x]), z, to_string(nnew[tt]));
                        }
                    }
                }
            }
            break;
        }
    }
    return ans;
}
