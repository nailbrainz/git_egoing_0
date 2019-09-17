
#include <iostream>

#include <malloc.h>
#define MAX 30000
#include <vector>

using namespace std;


typedef struct _pair{
    int a, b;
} m_pair;

bool operator<(_pair& l, _pair& r){
    if(l.a == r.a)return l.b <r.b;
    return l.a < r.a;
}

template<class V>
struct _vector{
    V* ary;
    int m_size, m_cap;
    _vector(int _cap = 2){
        m_cap = _cap;
        m_size = 0;
        ary = new V[m_cap];
    }

    void push_back(const V& val){
        if(m_size == m_cap){
            m_cap *= 2;
            V* tmp = new V[m_cap];
            for(int i = 0; i<m_size; i++)tmp[i] = ary[i];
            delete[] ary;
            ary = tmp;
        }
        ary[m_size++]=val;
    }

    V back(){
        return ary[m_size-1];
    }

    void pop_back(){
        if(m_size)m_size--;
    }

    int size(){
        return m_size;
    }

    void clear(){
        if(m_size)delete[] ary;
        m_size = 0;
        m_cap = 5;
        ary = new V[m_cap];
    }

    ~_vector(){
        delete[] ary;
    }

    V& operator[](int ind){
        return ary[ind];
    }
};
void swap(int& l, int& r){
    int tmp = l;
    l = r;
    r = tmp;
}

void swap(_pair& l, _pair& r){
    _pair tmp = l;
    l = r;
    r = tmp;
}


void flip(int _bo[4][4]){
    for(int i = 0; i<4; i++){
        for(int j = 0; j<2; j++){
            swap(_bo[i][j], _bo[i][3-j]);
        }
    }
}
int numbering[MAX], ids[MAX], h_orig[MAX];
_pair heights[MAX];
_vector<int> cache[MAX];
bool used[MAX];


extern int makeBlock(int module[][4][4]);


int bo2i(int module[4][4], int ind){
    int ret =0, mult = 1;
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            if(ind == 0){
                ret += mult * (module[i][j]);
            }
            else if(ind == 1)
                ret += mult * (module[3-j][i]);
            else if(ind == 2)
                ret += mult * (module[3-i][3-j]);
            else if(ind == 3)
                ret += mult * (module[j][3-i]);
            mult *= 3;
        }
    }
    return ret;
}

void qsort1(int l, int r, _pair ary[MAX]){
    if(l >= r)return;
    int pi = l, le = l+1, ri = r;
    while(le <= ri){
        while(le <= ri && !(ary[pi] < ary[le]))le++;
        while(le <= ri && !(ary[ri] < ary[pi]))ri--;
        if(le < ri)swap(ary[le], ary[ri]);
    }
    swap(ary[pi], ary[ri]);

    qsort1(l, ri-1, ary);
    qsort1(ri+1, r, ary);
}

void qsort(int l, int r, int ary[MAX]){
    if(l >= r)return;
    int pi = l, le = l+1, ri = r;
    while(le <= ri){
        while(le <= ri && !(ary[pi] < ary[le]))le++;
        while(le <= ri && !(ary[ri] < ary[pi]))ri--;
        if(le < ri)swap(ary[le], ary[ri]);
    }
    swap(ary[pi], ary[ri]);

    qsort(l, ri-1, ary);
    qsort(ri+1, r, ary);
}

int findID(int val){
    int l = 0, r = MAX-1;
    while(l <= r){
        int mid = (l + r)/2;
        if(val < numbering[mid]){
            r = mid-1;
        }else if(val > numbering[mid]){
            l = mid+1;
        }else{
            return mid;
        }
    }
    return -1;
}

int makeBlock(int module[][4][4])
{
    int cnt = 0;

    for(int c = 0; c<MAX; c++){
        used[c] = 0;
        int minn = 98;
        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                minn = minn < module[c][i][j] ? minn : module[c][i][j];
            }
        }
        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                module[c][i][j] -= minn;
            }
        }
        ids[c] = numbering[c] = bo2i(module[c], 0);
        heights[c].a = minn;
        heights[c].b = c;
        cache[c].clear();
    }
    qsort(0, MAX-1, numbering);
    qsort1(0, MAX-1, heights);

    for(int c = 0; c<MAX; c++){ //from smaller heights
        int orig_id = heights[c].b;
        int cache_id = ids[orig_id];
        cache_id = findID(cache_id);
        h_orig[orig_id] = module[orig_id][0][0] + heights[c].a;
        cache[cache_id].push_back(orig_id);
    }
    int ret = 0;

    for(int c = MAX-1; c>=0; c--){
        int id = heights[c].b;
        if(used[id]){
            continue;
        }
        cache[findID(ids[id])].pop_back();
        int flipped[4][4];
        for(int i = 0; i<4; i++)for(int j = 0; j<4; j++)flipped[i][j] = module[id][i][j];


        flip(flipped);
        int maxx = -1;
        for(int i = 0; i<4; i++){
            for(int j= 0; j<4; j++){
                maxx = maxx > flipped[i][j] ? maxx : flipped[i][j];
            }
        }

        for(int i = 0; i<4; i++){
            for(int j= 0; j<4; j++){
                flipped[i][j] = maxx - flipped[i][j];
            }
        }
        int flip_sizes[4] = {flipped[0][0], flipped[3][0], flipped[3][3], flipped[0][3]};

        bool found = false;
        int fi = -1, fv = -1, tcache;
        for(int i = 0; i<4 ; i++){
            int size = maxx - flip_sizes[i] + heights[c].a;
            int cache_val = bo2i(flipped, i);
            cache_val = findID(cache_val);
            if(cache_val != -1){
                if(cache[cache_val].size() > 0){
                    if(fv < h_orig[cache[cache_val].back()]){
                        fv = h_orig[cache[cache_val].back()];
                        fi = i;
                        tcache = cache_val;
                    }
                }
            }
        }
        if(fi != -1){
            int size = maxx - flip_sizes[fi] + heights[c].a;
            ret += h_orig[cache[tcache].back()] + size;
            used[id] = 1;
            used[cache[tcache].back()] = 1;
            cache[tcache].pop_back();
        }

    }
    return ret;
}


int main(void)
{
    static int module[MAX][4][4];
    srand(3); // 3 will be changed

    for (int tc = 1; tc <= 10; tc++)
    {
        for (int c = 0; c < MAX; c++)
        {
            int base = 1 + (rand() % 6);
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					module[c][y][x] = base + (rand() % 3);
				}
			}
        }
        cout << "#" << tc << " " << makeBlock(module) << endl;
    }

	return 0;
}
