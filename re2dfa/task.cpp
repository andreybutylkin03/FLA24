#include "api.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct Tree
{
    char symb;
    bool nullable;

    set<int> firstpos;
    set<int> lastpos;

    Tree *left;
    Tree *right;

    Tree (char s, bool n=false, int fl = 0) {
        symb = s;
        nullable = n;
        left = nullptr;
        right = nullptr;
        if (fl != 0) {
            firstpos.insert(fl);
            lastpos.insert(fl);
        }
    }
};


void
first_last_pos_complete(Tree *r)
{
    if (r->left == nullptr && r->right == nullptr)
        return;

    first_last_pos_complete(r->left);
    if (r->right != nullptr)
        first_last_pos_complete(r->right);

    if (r->symb == '|') {
        r->nullable = r->left->nullable | r->right->nullable;

        r->firstpos.insert(r->left->firstpos.begin(), r->left->firstpos.end());
        r->firstpos.insert(r->right->firstpos.begin(), r->right->firstpos.end());

        r->lastpos.insert(r->left->lastpos.begin(), r->left->lastpos.end());
        r->lastpos.insert(r->right->lastpos.begin(), r->right->lastpos.end());
    } else if (r->symb == '.') {
        r->nullable = r->left->nullable & r->right->nullable;

        if (r->left->nullable) {
            r->firstpos.insert(r->left->firstpos.begin(), r->left->firstpos.end());
            r->firstpos.insert(r->right->firstpos.begin(), r->right->firstpos.end());
        } else 
            r->firstpos.insert(r->left->firstpos.begin(), r->left->firstpos.end());

        if (r->right->nullable) {
            r->lastpos.insert(r->left->lastpos.begin(), r->left->lastpos.end());
            r->lastpos.insert(r->right->lastpos.begin(), r->right->lastpos.end());
        } else 
            r->lastpos.insert(r->right->lastpos.begin(), r->right->lastpos.end());
    } else if (r->symb == '*') {
        r->nullable = true;

        r->firstpos.insert(r->left->firstpos.begin(), r->left->firstpos.end());
        r->lastpos.insert(r->left->lastpos.begin(), r->left->lastpos.end());
    }
}


void
follow_pos_complete(Tree *r, map<int, set<int>> &m)
{
    if (r->left == nullptr && r->right == nullptr)
        return;

    if (r->symb == '*') {
        for (auto i : r->left->lastpos) 
            m[i].insert(r->left->firstpos.begin(), r->left->firstpos.end());
    } else if (r->symb == '.') {
        for (auto i : r->left->lastpos) 
            m[i].insert(r->right->firstpos.begin(), r->right->firstpos.end());
    }

    follow_pos_complete(r->left, m);
    if (r->right != nullptr)
        follow_pos_complete(r->right, m);
}


DFA 
re2dfa(const string &s) 
{
    if (s.size() == 0) {
        Alphabet alph_s = Alphabet("a");
        DFA res = DFA(alph_s);
        res.create_state("0", true);
        res.set_initial("0");    
        return res;
    }

    string sc = "", scc = "";
    int sz = s.size();
    set<char> no_alph = {'*', '|', '(', ')'};

    for (int i = 0; i < sz; ++i) {
        if (i > 0) {
            if (s[i] == '|' && s[i - 1] == '|') 
                scc += ' ';
            if (s[i] == '*' && s[i - 1] == '(') 
                scc += ' ';
            if (s[i] == '*' && s[i - 1] == '|') 
                scc += ' ';
            if (s[i] == '|' && s[i - 1] == '(') 
                scc += ' ';
            if (s[i] == ')' && s[i - 1] == '|')
                scc += ' ';
            if (s[i] == ')' && s[i - 1] == '(')
                scc += ' ';
        }

        if (i == 0 && (s[i] == '*' || s[i] == '|'))
            scc += ' ';

        scc += s[i];
    }

    if (s[sz - 1] == '|') 
        scc += ' ';

    sc += scc[0];
    for (int i = 1; i < scc.size(); ++i) {
        if (no_alph.count(scc[i]) == 0 && no_alph.count(scc[i - 1]) == 0) 
            sc += '.';
        if (scc[i] == '(' && scc[i - 1] == ')')
            sc += '.';
        if (no_alph.count(scc[i]) == 0 && scc[i-1] == '*')
            sc += '.';
        if (no_alph.count(scc[i]) == 0 && scc[i-1] == ')')
            sc += '.';
        if (scc[i] == '(' && no_alph.count(scc[i - 1]) == 0)
            sc += '.';
        if (scc[i] == '(' && scc[i-1] == '*')
            sc += '.';


        sc += scc[i];
    }

    Alphabet alph_s = Alphabet(s);
	DFA res = DFA(alph_s);

    map<char, set<int>> pos_ch;

    string ans = "";
    stack<char> st;
    stack<Tree *> st_t;

    map<char, int> m = {
        {'(', 0}, 
        {'|', 1}, 
        {'.', 2}, 
        {'*', 3}
    };

    int i_fl = 0;

    for (int i = 0; i < sc.size(); ++i) {
        if (alph_s.has_char(sc[i]) || sc[i] == ' ' || sc[i] == '#') {
            ans += sc[i];

            if (sc[i] == ' ') {
                st_t.push(new Tree(' ', true));
            } else {
                st_t.push(new Tree(sc[i], false, ++i_fl));
                pos_ch[sc[i]].insert(i_fl);
            }

        } else if (sc[i] == '(') 
            st.push(sc[i]);
        else if (sc[i] == ')') {
            while (st.size() > 0 && st.top() != '(') {
                ans += st.top();

                Tree *r = new Tree(st.top());

                if (st.top() == '*') {
                    Tree *pl = st_t.top();
                    st_t.pop();
                    r->left = pl;

                    st_t.push(r);
                } else {                    
                    Tree *pr = st_t.top();
                    st_t.pop();
                    Tree *pl = st_t.top();
                    st_t.pop();

                    r->left = pl;
                    r->right = pr;

                    st_t.push(r);
                }

                st.pop();
            }
            
            st.pop();
        } else {
            while (st.size() > 0 && m[st.top()] >= m[sc[i]]) {
                ans += st.top();

                Tree *r = new Tree(st.top());

                if (st.top() == '*') {
                    Tree *pl = st_t.top();
                    st_t.pop();
                    r->left = pl;

                    st_t.push(r);
                } else {
                    Tree *pr = st_t.top();
                    st_t.pop();
                    Tree *pl = st_t.top();
                    st_t.pop();

                    r->left = pl;
                    r->right = pr;

                    st_t.push(r);
                }

                st.pop();
            }
            
            st.push(sc[i]);
        }
    }

    while (st.size()) {
        ans += st.top();
        Tree *r = new Tree(st.top());

        if (st.top() == '*') {
            Tree *pl = st_t.top();
            st_t.pop();
            r->left = pl;

            st_t.push(r);
        } else {
            Tree *pr = st_t.top();
            st_t.pop();
            Tree *pl = st_t.top();
            st_t.pop();


            r->left = pl;
            r->right = pr;

            st_t.push(r);
        }

        st.pop();
    }

    sc = ans;

    Tree *ht = new Tree('#', false, ++i_fl);
    Tree *r = new Tree('.');

    r->left = st_t.top();
    st_t.pop();
    r->right = ht;

    first_last_pos_complete(r);

    map<int, set<int>> followpos;

    follow_pos_complete(r, followpos);

    set<set<int>> Q;
    set<int> q0 = r->firstpos;
    Q.insert(q0);
    int count_a = 0;
    map<set<int>, string> set2s;
    set2s[q0] = to_string(count_a);
    res.create_state(to_string(count_a));
    res.set_initial(to_string(count_a));
    ++count_a;

    set<set<int>> marked;

    bool flag_c = true;

    while (flag_c) {
        flag_c = false;
        for (auto R : Q) {
            if (marked.count(R) == 1)
                continue;

            marked.insert(R);

            for (auto a : alph_s) {
                //cout << "SYMB -- " << a << "\n";
                set<int> S;

                for (auto p : pos_ch[a]) {
                    if (R.count(p) == 0)
                        continue;
                    S.insert(followpos[p].begin(), followpos[p].end());
                }

                if (S.size()) {
                    if (Q.count(S) == 0) {

                        Q.insert(S);
                        flag_c = true;

                        set2s[S] = to_string(count_a);
                        res.create_state(to_string(count_a));
                        ++count_a;
                    }

                    res.set_trans(set2s[R], a, set2s[S]);
                }
            }
        }
    }

    for (auto R : Q) {
        if (R.count(i_fl)) {
            res.make_final(set2s[R]);
        }
    }

	return res;
}
