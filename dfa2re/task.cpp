#include "api.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

string dfa2re(DFA &d) {
    int ds = d.size();
    if (ds == 0 || d.is_empty()) {
        return "";
    }
    
    stringstream ss(d.to_string());
    string vr_i;
    getline(ss, vr_i);

    map<string, int> sts;
    map<int, map<int, string>> inm, outm;

    for (int i = 0; i < ds; ++i) {
        string vr_i;
        getline(ss, vr_i);
        string vrr_i = "";
        int vr = 0;
        for (auto x : vr_i) {
            if (x == '[' || x == ']')
                ++vr;
            else
                vrr_i += x;
        }
        
        sts["[" + vrr_i + "]"] = i;
        if (vr == 4) {
            outm[i][ds] = " ";
            inm[ds][i] = " ";
        }
    }

    vector<pair<int, int>> obh(ds, {0, 0});

    while (true) {
        string vr_i;
        getline(ss, vr_i);
        if (vr_i.size() == 0) 
            break;

        stringstream sss(vr_i);

        char symb;
        string fro, t;

        sss >> fro >> symb >> t;

        int from = sts[fro];
        int to = sts[t];
        string ssymb = "";
        ssymb += symb;

        if (inm[to][from].size()) {
            inm[to][from] += "|" + ssymb;
            outm[from][to] += "|" + ssymb;
        } else {
            inm[to][from] = ssymb;
            outm[from][to] = ssymb;
            ++obh[to].first;
            obh[to].second = to;
        }
    }
    ss.clear();

    sort(obh.begin(), obh.end());
    int init = 0;

    set<int> delst;
    /*cout << "inm\n";
    for (auto x :inm) 
        for (auto y :x.second)
            cout << x.first << " " << y.first << " " << y.second << "\n";*/

    /*cout << "outm\n";
    for (auto x :outm) 
        for (auto y :x.second)
            cout << x.first << " " << y.first << " " << y.second << "\n";*/

    
    for (auto x : obh) {
        if (x.second == init || x.second == ds)
            continue;

        //cout << "step to " << x.second << "\n";

        for (auto y : inm[x.second]) {
            if (delst.count(y.first))
                continue;

            string zv = "";

            if (outm[x.second][x.second].size()) {
                zv += "(" + outm[x.second][x.second] + ")";
                zv += "*";
            }

            for (auto z : outm[x.second]) {
                if (z.first == x.second)
                    continue;

                if (delst.count(z.first))
                    continue;

                stringstream ss;
                
                if (zv.size()) {
                    if (y.first != z.first) {
                        if (outm[y.first][z.first].size())
                            ss << "|((" << y.second << ").(" << zv << ").(" << z.second << "))";
                        else 
                            ss << "((" << y.second << ").(" << zv << ").(" << z.second << "))";

                        outm[y.first][z.first] += ss.str();
                        inm[z.first][y.first] += ss.str();

                        //cout << x.second << " " << y.first << "->" << z.first << " " << outm[y.first][z.first] << "\n";
                    } else {
                        if (outm[y.first][y.first].size())
                            ss << "|((" << y.second << ").(" << zv << ").(" << z.second << "))";
                        else 
                            ss << "((" << y.second << ").(" << zv << ").(" << z.second << "))";

                        outm[y.first][y.first] += ss.str();
                        inm[y.first][y.first] += ss.str();

                        //cout << x.second << " " << y.first << "->" << y.first << " " << outm[y.first][y.first] << "\n";
                    }
                } else {
                    if (y.first != z.first) {
                        if (outm[y.first][z.first].size())
                            ss << "|((" << y.second << ").(" << z.second << "))";
                        else 
                            ss << "((" << y.second << ").(" << z.second << "))";

                        outm[y.first][z.first] += ss.str();
                        inm[z.first][y.first] += ss.str();

                        //cout << x.first << " " << y.first << "->" << z.first << " " << outm[y.first][z.first] << "\n";
                    } else {
                        if (outm[y.first][y.first].size())
                            ss << "|((" << y.second << ").(" << z.second << "))";
                        else 
                            ss << "((" << y.second << ").(" << z.second << "))";

                        outm[y.first][y.first] += ss.str();
                        inm[y.first][y.first] += ss.str();

                        //cout << x.first << " " << y.first << "->" << y.first << " " << outm[y.first][y.first] << "\n";
                    }

                }
            }
        }
        delst.insert(x.second);
    }

    string ans = "", sc = outm[init][ds];
    if (inm[init][init].size()) 
        sc += "(" + inm[init][init] + ")*.(" + sc + ")";
    //cout << sc << "\n";
    inm.clear();
    outm.clear();
    delst.clear();
    obh.clear();
    stack<char> st;
    Alphabet alph_s = d.get_alphabet();

    map<char, int> m = {
        {'(', 0}, 
        {'|', 1}, 
        {'.', 2}, 
        {'*', 3}
    };

    for (int i = 0; i < sc.size(); ++i) {
        if (alph_s.has_char(sc[i]) || sc[i] == ' ') {
            ans += sc[i];
        } else if (sc[i] == '(') 
            st.push(sc[i]);
        else if (sc[i] == ')') {
            while (st.size() > 0 && st.top() != '(') {
                ans += st.top();
                st.pop();
            }
            
            st.pop();
        } else {
            while (st.size() > 0 && m[st.top()] >= m[sc[i]]) {
                ans += st.top();
                st.pop();
            }
            
            st.push(sc[i]);
        }
    }

    while (st.size()) {
        ans += st.top();
        st.pop();
    }

    stack<pair<string, int>> stt;

    int sz = ans.size();
    //cout << ans << "\n";

    set<char> oper = {'|', '.', '*'};
    for (int i = 0; i < sz; ++i) {
        if (ans[i] == '*') {
            string op1 = stt.top().first;
            int p1 = stt.top().second;
            stt.pop();

            if (p1 != -1 && m[ans[i]] > p1)
                op1 = "(" + op1 + ")";

            string vr = op1 + ans[i];
            stt.push({vr, m[ans[i]]});

        } else if (oper.count(ans[i])) {
            string op2 = stt.top().first;
            int p2 = stt.top().second;
            stt.pop();

            string op1 = stt.top().first;
            int p1 = stt.top().second;
            stt.pop();
            //cout << op1 << " " << p1 << " " << ans[i] << " " << m[ans[i]] << " " << op2 << " " << p2 << "\n";

            if (p1 != -1 && m[ans[i]] > p1)
                op1 = "(" + op1 + ")";
            if (p2 != -1 && m[ans[i]] > p2)
                op2 = "(" + op2 + ")";

            string vr;
            if (ans[i] == '.') 
                vr = op1 + op2;
            else
                vr = op1 + ans[i] + op2;
            stt.push({vr, m[ans[i]]});
        } else {
            string vr = "";
            if (ans[i] != ' ')
                vr += ans[i];
            stt.push({vr, -1});
        }
    }

    return stt.top().first;
}
