using namespace std;
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <iomanip>



enum side_ {sell = 1, buy};

struct ob_lev{
    unsigned int volume;
    unsigned int ord_num;
    list <unsigned int> ids;
};

struct order{
    unsigned int volume;
    double price;
    side_ side;
};


class instrument{

    public:

        string iname;
        map<double, ob_lev> sell;
        map<double, ob_lev> buy;
        unordered_map<unsigned int, order> ord_ids;
        
        instrument (string name){
            iname = name;
        }

        instrument (){
            iname = "_dummy_";
        }
        
        void add_ord(unsigned int oid, order o);

        void remove_ord(unsigned int oid);

        void mod_ord(unsigned int oid, unsigned int vol= 0, double pr = 0);
        
        void print_ob ();

        void print_full ();

 
    private:

        void match (double pr_lv);  

        map<double, ob_lev> & get_side(side_ s){

            return s == 1? sell : buy;

        }

};

void print_headers(unsigned int p);

// galochka ti ohueesh ot takogo ebanutogo polimorfizma

void print_left(stringstream & st, unsigned int ord, unsigned int vol, double pr, unsigned int p = 7);

void print_left(unsigned int ord, unsigned int vol, double pr, unsigned int p = 7);


void print_left(stringstream & st, unsigned int p = 7);

void print_left(unsigned int p = 7);

void print_right(stringstream & st, unsigned int ord, unsigned int vol, double pr, unsigned int p = 7);

void print_right(unsigned int ord, unsigned int vol, double pr, unsigned int p = 7);


void insert_o (map<double, ob_lev> & m, order & o, unsigned int oid);

void rem_o (map<double, ob_lev> & m, order & o, unsigned int oid);

class MarketSUB{
    
    public:

        list<string> subs_bbo;
        map<string, list<unsigned int>> subs_vwap;
        string filter;

        void add_sub_bbo(string inst);

        void add_sub_vwap(string inst, unsigned int n);

        void uns_sub_bbo(string inst);

        void uns_sub_vwap(string inst, unsigned int n);

        void update(instrument & A);
};

void print_bbo(map<double, ob_lev> sell, map<double, ob_lev> buy, string iname);
void print_vwap(unsigned int n, map<double, ob_lev> sell, map<double, ob_lev> buy, string iname);
