// Z under two
// Because what's some maths code without a bad french pun :)
// >>=, <<=, <<, >>, +=, +, *=, *, /, %, &=, &, ==, !=, <, >, = are implemented 
class zut {

unsigned int* number;
int size;
bool irreducible;
    public:

// Initializers -------------------------------------------------

zut(unsigned int* a, int b){
    number=a;
    size=b;
    irreducible=false;
}

zut(initializer_list<unsigned int> a){
    number=new unsigned int[a.size()];
    size=a.size()*32;
    copy(a.begin(), a.end(), number);
    irreducible=false;
}

zut(int b) {
    size=b;
    number = new unsigned int[b/32];
    irreducible=false;
}

zut(const zut &b){
    size=b.size;
    number = new unsigned int[b.size/32];
    (*this)=b;
    irreducible=b.irreducible;
}

void resize(int size){
    this->size=size;
}

// Primitives ---------------------------------------------------

// https://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
void operator<<=(int s){
     for (int i = s; i > 0; i -= (i % 32)) {
            if (!(i % 32)) {
                for (int y = (size/32)-1; y > 0; y--)
                    number[y] = number[y - 1];
                number[0] = 0;
                i -= 32;
            } else {
                for (int y = (size/32)-1; y > 0; y--)
                    number[y] = (number[y] << (i%32)) | ((number[y - 1] >> (32-i%32)) );
                number[0] <<= i%32;
            }
        }
} 

zut operator<<(int s){
    zut res((*this));
    res <<= s;
    return res;
} 


void operator>>=(int s){
    for (int i = s; i > 0; i -= (i % 32)) {
        if (!(i % 32)) {
            for (int y = (size/32)-1; y >0; y--)
                number[y] = number[y - 1];
            number[0] = 0;
            i -= 32;
        } else {
            for (int y = (size / 32) - 1; y > 0; y--)
                number[y] = (number[y] >> (i%32)) | ((number[y - 1] << (32 - (i%32)) & 1));
            number[0] >>= i%32;
        }
    }
} 

zut operator>>(int s){
    zut res((*this));
    res >>= s;
    return res;
}

// i don't think this can be made faster without arch specific code :p
// in a z/2z field 1+1=0 and 1+0 = 1, thus it's a simple xor
// also i guess you know if you are reading this code but substraction =
// addition in Z/2Z so this can be(and is, ie in division) used interchangeably
void operator+=(const zut& b){
    for (int i = 0; i < size / 32; i++){
        if(!(i < b.size / 32))
                break;
        number[i] ^= b.number[i];
    }
}

zut operator+(const zut& b){
    zut res((*this));
    res+=b;
    return res;
}

// https://en.wikipedia.org/wiki/Finite_field_arithmetic#C_programming_example
// Ugh, bit by bit comparison, it really looks like a bottleneck to me, but
// I can't find a way to make it faster...
 void operator*=(zut& b){
    zut tmp(size);
    tmp=(*this);
    (*this)=0;
    for (int i = 0; i < size; i++) {
        if(!(i < b.size))
                break;
        // this verifies if the n-th bit inside b is lit
        if (b.get_bit(i))
            (*this)+=tmp; 
        tmp <<=1;
    }
}

zut operator*(zut& b){
    zut res((*this));
    res *= b;
    return res;
}

// /!\ primitive might fail if 2 values have different size!!
zut operator/(zut& b){
    zut r((*this));
    zut q(size); q=0;

    int deg_a=(*this).degree();
    int deg_div=deg_a-b.degree();
    for (int i = 0; i <= deg_div; i++) {
        q<<=1;
        int y=deg_a-i;
        if (r.get_bit(y)) {
            q++; r+=(b << (deg_div-i));
        }
    }
    return q;
}

zut operator%(zut& b){
    zut r((*this));
    zut q(size); q=0;

    int deg_a=(*this).degree();
    int deg_div=deg_a-b.degree();
    for (int i = 0; i <= deg_div; i++) {
        q<<=1;
        int y=deg_a-i;
        if (r.get_bit(y)) {
            q++; r+=(b << (deg_div-i));
        }
    }
    return r;
}

void operator&=(int b){
    for (int i = 0; i < size / 32; i++)
        number[i] &= b;
}

zut operator&(int b){
    zut res((*this));
    res &= b;
    return res;
}

void operator&=(const zut& b){
    for (int i = 0; i < size / 32; i++){
        if(!(i < b.size / 32))
                break;
        number[i] &= b.number[i];
    }
}

zut operator&(const zut& b){
    zut res((*this));
    res &= b;
    return res;
}

 void operator++(int a){
        number[0]^=1;
}


// Logical operators --------------------------------------------
bool operator==(const zut& b){
    for (int i = 0; i < size / 32; i++){
        if(!(i < b.size / 32))
                break;
        if(number[i]!=b.number[i])
            return false;
    }
    return true;
}

bool operator==(int b){
    if(number[0]!=b)
        return false;
    for (int i = 1; i < size / 32; i++)
        if(number[i]!=0)
            return false;
    return true;
}

bool operator!=(const zut& b){
    return !((*this)==b);
}

bool operator!=(int b){
    return !((*this)==b);
}

bool operator<(int a){
    return number[0]<a;
}

bool operator>(int a){
    return number[0]>a;
}
void operator=(const zut& b){
    for (int i = 0; i < b.size / 32; i++)
        number[i] = b.number[i];
    for (int i = b.size / 32; i < size / 32; i++)
        number[i] = 0;
}



void operator=(int b){
    number[0] = b;
    for (int i = 1; i < size / 32; i++)
        number[i] = 0;
}

// Algorithms ---------------------------------------------------

// should be able to do this faster...
zut get_bits(int start, int end) {
    zut res(size); res=0;
    for (int i = start; i < end; i++) {
        if ((*this).get_bit(i))
            res.number[(i-start)/32] |= (1 << (i-start)%32);
    }
    return res;
}

bool get_bit(int bit){
    if (((number[(bit / 32)] & (1 << bit % 32)) >> bit % 32) & 1)
        return true;
    return false;
}


/* So, how does this work?
 * Well to begin with we are dealing with a Z/2Z field and as such have an
 * equation in the form of sigma(ax^n), for n up to the maximum bit size of
 * our current number. Knowing a is either 0 or 1 in this field, it can also
 * be disregarded as d0=0. This means the derivative is in
 * the form of sigma(nx^(n-1)). This also means that any odd power is
 * completely disregarded as they would be, by definition, 0.
 * Thus we only need to lower the power of every power by one and to
 * drop x^n where n is odd.
 */
zut derivative() {
    return ((*this)&0xaaaaaaaa)>>1; // -> 0b101010... -> 1+3 aka mask odd numbers
}

// https://stackoverflow.com/questions/671815/what-is-the-fastest-most-efficient-way-to-find-the-highest-set-bit-msb-in-an-i
int degree() {
    for (int i = (size / 32) - 1; i >= 0; i--)
        if (number[i])
            return (i * 32) + (31 - __builtin_clz(number[i]));
    return 0;
}

// mostly same as derivative: sqrt(x^n) = x^(n/2) (except 1)thus we just shift everything
zut sqrt(){
    zut sqrt(size);
    zut bit(size); bit=1;

    for (int i=0;i<size;i++,bit<<=1)
        sqrt+=((bit&(*this))>>(i>>1));
    return sqrt;
}

// aren't you supposed to learn what this is in middle school? ;)
zut gcd(zut b) {
    zut a((*this));
    zut mod=a%b;
    while(mod>2){
        a=b;
        b=mod;
        mod=a%b;
    }
    if(mod==0)
        return b;
    return mod;
}

// https://en.wikipedia.org/wiki/Factorization_of_polynomials_over_finite_fields#Square-free_factorization
// ^same as berlekamp, explains it better than i ever could
vector<zut> sff(vector<zut> factors={}) {
    zut d=(*this).derivative();
    // step 2 of sff(wikipedia)
    if(d==0){
        factors=(*this).sqrt().sff();
        factors.insert(factors.end(), factors.begin(), factors.end());
    }
    else{
        zut gcd=(*this).gcd(d);
        if(gcd==1){
            factors.push_back((*this));
            return factors;
        }
        factors=gcd.sff(factors);
        factors=((*this)/gcd).sff(factors);
    }
    return factors;
}

/*
 * Berlekamp algorithm in all its glory, along with a baked in matrix reduced
 * echelon form transformation.
 */
vector<zut> bk(){
    // first create the matrix Q-I
    vector<zut> subalgebra;
    vector<zut> factors;
    int deg = (*this).degree();
    zut x_2n(size*2);
    zut fac(size*2); fac=(*this);
    zut di(size); di=1;
    vector<zut> mat; mat.reserve(deg);
    for(int i=0; i<deg;i++){
        x_2n=1; x_2n <<=i*2;
        zut row(size*2); 
        // Q = (x_2n%fac)+di), I=di
        row=(((x_2n%fac)+di)<<(deg+1))+di; mat.push_back(row); 
        di<<=1;
    }

    // then compute the row reduced echelon form of the matrix     
    int row=0;
    for (int j = deg*2; j>=0; j--) {
        int pivrow = -1;
        for (int i = row; i < mat.size(); i++) {
            if (mat[i].get_bit(j)) {
                pivrow = i;
                break;
            }
        }
        if (pivrow != -1 ) {
            swap(mat[row], mat[pivrow]);
            for (int i = 0; i < mat.size(); i++) {
                if (i != row) {
                    if (mat[i].get_bit(j)) {
                        mat[i]+= mat[row];
                    }
                }
            }
        row++;
        }
    }

    // then read off berlekamp subalgebra from the null space basis 
    bool empty=true;
    for(zut  &row: mat) {
        zut row_left = row.get_bits(deg+2, (deg*2)+2);
        if(row_left==0){
            zut row_right = row.get_bits(0, deg);
            if(row_right!=1){
                // bk subalgebra takes half of the matrix thus size/2
                row_right.resize(size);
                subalgebra.push_back(row_right);
                empty=false;
        }
    }

    }
    // if we had no correct null basis it is irreductible
    if(empty){
        (*this).irreducible=true; factors.push_back((*this)); return factors;
    }

    // otherwise calculates irreducible factor from berlekamp subalgebra
    factors.push_back((*this));
    for(zut  &vector: subalgebra) {
       zut test((*this).gcd(vector)); 
       test=(*this)/test;
       for(int i=0; i<factors.size(); i++){
           if((factors[i]%test)==0){
            if((factors[i]/test)!=1){
               zut help((factors[i]/test));
               factors[i]=test;
               factors.push_back(help);
               empty=false;
               break;
            }
           }
       }
    }
    if(empty)
        factors[0].irreducible=true;
    return factors;
}

// we get square free factors, then recurse with berlekamp to get irreducible
// forms of all possible factors
vector<zut> factorize() {
    vector<zut> factors=(*this).sff();

    bool done=false;
    while(!done){

        for(int i=0; i< factors.size(); i++) {
            if(!factors[i].irreducible) {
                vector<zut> factored = factors[i].bk();
                factors.erase(factors.begin()+i);
                factors.insert(factors.end(), factored.begin(), factored.end());
                break;
            }
            if(i==factors.size()-1){
                done=true;
            }
        }
        if(factors.size()==0)
            done=true;
    }
    return factors;
}

}
