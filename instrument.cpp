using namespace std;
#include <string>
#include <map>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <list>
#include <iterator>
#include <algorithm>
#include "instrument.h"
// #include "instrument_side_func.cpp"

void instrument::match(double pr_lev){

    if ( sell.empty() || buy.empty() ) return;

    unsigned int V_buy = 0;
    unsigned int V_sell = 0;
    
    map<double, ob_lev>::const_iterator lb = buy.end(), ls = sell.begin();

    lb--;

    if (lb->first < pr_lev  || ls->first > pr_lev) return;

    lb++;
   
    do{
        lb--;
        if(lb->first < pr_lev){
            lb++;
            break;
        }

        V_buy += lb->second.volume;

    } while(lb != buy.begin());

    for(ls; ls->first <= pr_lev && ls != sell.end(); ++ls)    V_sell += ls->second.volume;
    
    ls--;

    unsigned int V_match = V_buy > V_sell ? V_sell : V_buy;

    int temp = V_match;

    map<double, ob_lev>::const_iterator it = sell.begin();

    while(temp > 0){

        list l = it->second.ids;
        it++;

        for(unsigned int oid : l){

            int vi = ord_ids[oid].volume;
                
            if (temp <= vi){
                    
                mod_ord(oid, vi - temp);
                temp = 0;
                break;
            }

            temp -= vi; 

            remove_ord(oid);
        }
    }

    map<double, ob_lev>::const_iterator it3 = buy.end();
    it3--;
    temp = V_match;
    bool flag = true;

    while(flag && temp > 0){

        list l2 = it3->second.ids;

        if (it3 != buy.begin())  it3--;

        else flag = false; 

        for(unsigned int oid : l2) {

            unsigned int vi = ord_ids[oid].volume;

            if (temp <= vi){

                mod_ord(oid, vi-temp);
                temp = 0;
                break;
            }

            temp -= vi;
            remove_ord(oid);

        }
    }
    
}


void instrument::add_ord(unsigned int oid, order o){

            
    ord_ids[oid] = o;

    insert_o(get_side(o.side), o, oid);
    
    match(o.price);

}


void instrument::remove_ord(unsigned int oid){

    try{
        order o = ord_ids.at(oid);

        rem_o(get_side(o.side), o, oid);
        
        ord_ids.erase(oid);
    }
    catch(const std::exception& e){

        std::cerr << "remove: order not found\n";
    
    }

}


void instrument::mod_ord(unsigned int oid, unsigned int vol, double pr){

    
    if (vol == 0){

        remove_ord(oid);
        return;
    }
    try{

        order o = ord_ids.at(oid);
        if (vol == o.volume && pr == o.price) return;

        order o_new;
        o_new.side = o.side;
        pr == 0 ? o_new.price = o.price : o_new.price = pr;
        o_new.volume = vol;

        map<double, ob_lev> & m = get_side(o.side);

        rem_o (m, o, oid);
        insert_o(m, o_new, oid);

        ord_ids[oid] = o_new;
        match(pr);

    }
    catch(const std::exception& e){

        std::cerr << "no such order \n";
    }

}


void instrument::print_ob (){

    cout << iname << endl;
    
    print_headers(7);

    map<double, ob_lev>::const_iterator it2 = buy.end();

    for (map<double, ob_lev>::const_iterator it = sell.begin(); it != sell.end(); ++it){


        if (it2 != buy.begin()){

            it2--;

            print_left(it2->second.ord_num, it2->second.volume, it2->first);
        }
        else print_left();

        print_right(it->second.ord_num, it->second.volume, it->first);
    }
    if (it2 != buy.begin()){
        do{

            it2--;

            print_left(it2->second.ord_num, it2->second.volume, it2->first);

        }while(it2 != buy.begin());
    }

    cout << "\n";

}


void instrument::print_full ( ){

    cout << iname << endl;

    print_headers(7);
    
    stringstream sst2, sst;

    map<double, ob_lev>::const_iterator it2 = buy.end();

    for (map<double, ob_lev>::const_iterator it = sell.begin(); it != sell.end(); ++it){

    
        if (it2 != buy.begin()){

            it2--;

            for (unsigned int n : it2->second.ids){
            
                order o = ord_ids[n];

                print_left(sst2, n, o.volume, o.price);
            }

        }
        else print_left(sst2);

        for (unsigned int m: it->second.ids){
            
                order o = ord_ids[m];

                print_right(sst, m, o.volume, o.price);

        }

    }
    if (it2 != buy.begin()){
        do{
            it2--;

            for (unsigned int n : it2->second.ids){
            
                order o = ord_ids[n];

                print_left(sst2, n, o.volume, o.price);

            }
        
        }while(it2 != buy.begin());
    }
    
    string s2, s1, s3; 
    getline(sst2,s2);
    
    while(s2!="" or s1 != ""){

        getline(sst, s1);
        
        if (s2 == "" && s1 != "") cout << "|" << setw(31) << "||" << s1 << endl; 
        else cout << s2 << s1 << endl;
        
        getline(sst2,s2);
    
    }
    cout << "\n";

}


// VWAP and BBO

void print_bbo(map<double, ob_lev> sell, map<double, ob_lev> buy, string iname){

    if (sell.empty() or buy.empty()){
        //cout << "BBO can`t be computed :( \n";
        return;
    } 

    cout << "BBO for "<< iname << "\n"; 
    
    print_headers(7);

    map<double, ob_lev>::const_iterator it2 = buy.end();
    map<double, ob_lev>::const_iterator it = sell.begin();

    it2--;
    
    print_left(it2->second.ord_num, it2->second.volume, it2->first, 7);
    
    print_right(it->second.ord_num, it->second.volume, it->first);
}

void print_vwap(unsigned int n, map<double, ob_lev> sell, map<double, ob_lev> buy, string iname){

    if (n == 0) return;
    if (sell.empty() or buy.empty()){
        //cout << "VWAP can`t be computed :( \n";
        return;
    } 

    unsigned int v1 = 0;
    unsigned int n1 = n;
    unsigned int s_s = 0;

    for (map<double, ob_lev>::const_iterator it = sell.begin(); it != sell.end() && n1 != 0; ++it){
        v1 = it->second.volume;
        if (v1 < n1){
            s_s += v1*(it->first);
            n1 = n1 - v1;
        }
        else {
            s_s += n1*(it->first);
            n1 = 0;
        }
    }

    if (n1 != 0){
        //cout << "VWAP does not exist \n";
        return;
    }

    unsigned int v2 = 0;
    unsigned int n2 = n;
    unsigned int s_b = 0;

    map<double, ob_lev>::const_iterator it = buy.end();
    do{
        it--;
        v2 = it->second.volume;
        if (v2 < n2){
            s_b += v2*(it->first);
            n2 = n2 - v2;
        }
        else {
            s_b += n2*(it->first);
            n2 = 0;
        }


    } while(it != buy.begin() && n2 !=0);

    if (n2 != 0){
        //cout << "VWAP does not exist \n";
        return;
    }
    
        
    cout << setprecision(5) << "VWAP for " << iname <<", n =  " << n << ": ("<< 1.0*s_s/n << ',' << 1.0*s_b/n << ")\n";  

}

// Subscript

void MarketSUB::add_sub_bbo(string inst){

    if(filter == "" || inst == filter) subs_bbo.push_back(inst);
}

void MarketSUB::add_sub_vwap(string inst, unsigned int n){
    
    if (filter != "" && inst != filter) return;
    try{
        subs_vwap.at(inst).push_back(n);
    }
    catch(const out_of_range& ex){
        subs_vwap[inst] = {n};
    };
}

void MarketSUB::uns_sub_bbo(string inst){

    if (filter != "" && inst != filter) return;

    auto it = find(subs_bbo.begin(), subs_bbo.end(), inst);

    if(it != subs_bbo.end()) subs_bbo.erase(it);
}

void MarketSUB::uns_sub_vwap(string inst, unsigned int n){

    if (filter != "" && inst != filter) return;

    try{

        list<unsigned int> l = subs_vwap.at(inst);

        auto it = find(l.begin(), l.end(), n);

        if(it != l.end()) l.erase(it);            
    }
    catch(const out_of_range& ex){

        cout << "no such subscription \n";
    };
}

void MarketSUB::update(instrument & A){
    
    string inam = A.iname;

    if(filter != "" && inam != filter) return;  

    if( auto it = subs_vwap.find(inam); it != subs_vwap.end())

    for(unsigned int n : it->second) print_vwap(n, A.sell,  A.buy, inam);

    if(auto it = find(subs_bbo.begin(), subs_bbo.end(), inam); it != subs_bbo.end()) print_bbo(A.sell, A.buy, inam);

}

// side funcs

void print_headers(unsigned int p){

    cout   << "| " 
    << setw(p) << "orders"  << " | " 
    << setw(p) << "volume"  << " | " 
    << setw(p) << "bid"  << " || ";
    cout  << setw(p)  << "ask"  << " | "  
    << setw(p) << "volume"  << " | " 
    << setw(p) << "orders" << "| \n";

}

// galochka ti ohueesh ot takogo ebanutogo polimorfizma

void print_left(stringstream & st, unsigned int ord, unsigned int vol, double pr, unsigned int p){

    st << "| " 
    << setw(p) << ord << " | " 
    << setw(p) << vol << " | " 
    << setw(p) << pr  << " ||\n";

}

void print_left(unsigned int ord, unsigned int vol, double pr, unsigned int p){

    cout << "| " 
    << setw(p) << ord << " | " 
    << setw(p) << vol << " | " 
    << setw(p) << pr  << " || ";

}


void print_left(stringstream & st, unsigned int p){

    st << "| " << setw(p*3 + 10) << " ||\n";

}

void print_left(unsigned int p){

    cout << "| " << setw(p*3 + 10) << " || ";

}

void print_right(stringstream & st, unsigned int ord, unsigned int vol, double pr, unsigned int p){

    st << setw(p+1) << pr << " | "  
    << setw(p) << vol  << " | "
    << setw(p) << ord << "|\n"; 

}

void print_right(unsigned int ord, unsigned int vol, double pr, unsigned int p){

    cout << setw(p) << pr << " | "  
    << setw(p) << vol  << " | "
    << setw(p) << ord << "|\n"; 

}

void insert_o (map<double, ob_lev> & m, order & o, unsigned int oid){

    if (m.count(o.price) > 0) {
        m[o.price].ord_num ++;
        m[o.price].volume += o.volume;
        m[o.price].ids.push_back(oid);
    }
    else {
        m.insert({o.price, ob_lev{o.volume, 1, {oid}}});
    }  
}

void rem_o (map<double, ob_lev> & m, order & o, unsigned int oid){

    if (m.count(o.price) > 0) 
        if (m[o.price].ord_num == 1)
            m.erase(o.price);
        else {

            m[o.price].ord_num --;
            m[o.price].volume -= o.volume;
            m[o.price].ids.remove(oid);

        }
    else {

        cout << "order not found" << endl;
    }  
}
