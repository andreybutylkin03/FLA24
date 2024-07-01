#include "api.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

using namespace std;


void 
deep(set<long long> &dos, Alphabet &alph, map<pair<long long, char>, long long> &tr, long long ds, long long cur)
{
    for (auto x : alph) {
        if (tr.count({cur, x})) {
            long long i = tr[{cur, x}];
            if (dos.count(i))
                continue;
            dos.insert(i);
            deep(dos, alph, tr, ds, i);
        }
    }
}

/*DFA pre_dfa_minim(DFA &d) {
    Alphabet alph = d.get_alphabet();

    DFA ans(alph);
    map<long long, set<long long>> G;

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
    set<long long> doss;

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
    }

    for (long long i = 0; i < ds; ++i) {
        for (auto x : alph) {
            if (tr.count({i, x}) == 0)
                tr[{i, x}] = ds;

        }
    }
    for (auto x : alph)
        tr[{ds, x}] = ds;
    doss.insert(0);
    deep(doss, alph, tr, ds, 0);
    set<long long> dos;
    for (long long i = 0; i <= ds; ++i) {
        if (doss.count(i) == 0)
            dos.insert(i);
    }


}*/

DFA dfa_minim(DFA &d) {
    Alphabet alph = d.get_alphabet();

    DFA ans(alph);
    map<long long, set<long long>> G;

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
    set<long long> doss;

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
    }

    for (long long i = 0; i < ds; ++i) {
        for (auto x : alph) {
            if (tr.count({i, x}) == 0)
                tr[{i, x}] = ds;

        }
    }
    for (auto x : alph)
        tr[{ds, x}] = ds;
    doss.insert(0);
    deep(doss, alph, tr, ds, 0);
    set<long long> dos;
    for (long long i = 0; i <= ds; ++i) {
        if (doss.count(i) == 0)
            dos.insert(i);
    }
    set<long long> act = {0, 1};
    long long f_f = 2;

    while (true) {
        set<long long> new_act;
        bool falg = false;
        for (auto x : act) {
            if (falg) {
                new_act.insert(x);
                continue;
            }

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
                    new_act.insert(f_f);
                    n_act[vec] = f_f++;
                }
                
                G[n_act[vec]].insert(from);
            }

            if (n_act.size() > 1)
                falg = true;
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
            if (dos.count(y))
                continue;
            if (G[x].count(y)) {
                flf = true;
                break;
            }
        }

        nnew[x] = nf++;
        ans.create_state(to_string(nnew[x]), flf);
        if (flag)
            ans.set_initial(to_string(nnew[x]));
    }

    for (auto x : act) {
        for (auto y : G[x]) {
            for (auto z : alph) {
                for (auto tt : act) {
                    if (G[tt].count(tr[{y, z}])) {
                        ans.set_trans(to_string(nnew[x]), z, to_string(nnew[tt]));
                        break;
                    }
                }
            }
            break;
        }
    }
    for (auto x : act) {
        if (G[x].count(ds))
            ans.delete_state(to_string(nnew[x]));
        bool vrf = true;

        for (auto y : G[x]) {
            if (dos.count(y) == 0)
                vrf = false;
        }
        if (vrf) {
            ans.delete_state(to_string(nnew[x]));
        }
    }
    return ans;
}
