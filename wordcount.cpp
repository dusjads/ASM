#include <iostream>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <stdio.h>


__m128i const SPACE_MASK = _mm_set1_epi8(' ');
__m128i const ZERO_MASK = _mm_set1_epi8(0);
__m128i const ONE_MASK = _mm_set1_epi8(1);

size_t naive_wordcount(const char* s, const size_t count){
    size_t ans = 0;
    for (size_t i = 0; i < count; i++){
        if (s[i] != ' ' && (i == 0 || s[i-1] == ' '))
            ans++;
    }
    return ans;
}

size_t num_from_mask(__m128i mask){
    size_t res = 0;
    for (size_t i = 0; i < 16; i++) {
        res += (size_t) *((uint8_t *) &mask + i);
    }
    return res;
}

size_t wordcount(const char* s, const size_t count){
    if (count < 32) return naive_wordcount(s, count);

    size_t ans = 0;
    size_t offset = (16 - (size_t)s % 16);
    for (size_t i = 0; i < offset; i++){
        if (s[i] != ' ' && (i == 0 || s[i-1] == ' '))
            ans++;
    }
    if (s[offset - 1] != ' ') ans--;
    size_t middle = count - (count - offset) % 16;
    // std::cout << offset << ' ' << middle << ' ' << count << '\n';
    __m128i buf = ZERO_MASK;
    
    size_t cnt = 0;
    for (size_t i = offset; i < middle; i += 16){
        __m128i cur =  _mm_cmpeq_epi8(_mm_loadu_si128((__m128i*) (s + i)), SPACE_MASK);
        __m128i prev =  _mm_cmpeq_epi8(_mm_loadu_si128((__m128i*) (s + i - 1)), SPACE_MASK);
        __m128i res = _mm_and_si128(_mm_andnot_si128(prev, cur), ONE_MASK);
        buf = _mm_add_epi8(buf, res);
        cnt++;
        prev = cur;
        // std::cout << i << ' ' << num_from_mask(res) << '\n';
        if (cnt == 127){
            cnt = 0;
            ans += num_from_mask(buf);
            buf = ZERO_MASK;
        }
    }
    ans += num_from_mask(buf);
    for (size_t i = middle; i < count; i++){
        if (s[i] == ' ' && s[i-1] != ' ')
            ans++;
    }
    ans += s[count - 1] != ' ';
    return ans;
}

void test(){
    int t;
    std::cin >> t;
    int p = t;
    while (t--) {
        int n = rand() % 300 + 32;
        std::string str;
        std::string const alf = "  qwertyuioplkjhgfdsa";
        for (int i = 0; i < n; i++)
            str += alf[rand() % alf.size()];
        size_t w = wordcount(str.c_str(), n);
        size_t nw = naive_wordcount(str.c_str(), n);
        if (w != nw){
            std::cout << str << '\n' << w << '\n' << nw << '\n';
            break;
        }
        std::cout << p - t << ' ' << "OK" << '\n';
    }
}

std::string get_string(long long int a) {
    char *c = (char *)(&a);
    std::string res = "";
    for (int i = 0; i < 8; ++i) {
        res += *(c + i);
    }
    return res;
}

void print(__m128i& a ){
    std::cout << get_string(a[0]) << get_string(a[1]) << '\n';
}

int main(){
    // test();
    // std::string str = " g jjtldikqaqguk jqtjdid sgejkyhtuisjsj pjapiueo ejkqqo  trkfky sjdolpo rillhj hskysjgalaqejklhiwosjatkqjqjufjqktupawulqugg rioyfy gkpdeklkywkai ui jswqjdwdepe dqhikokr wkqholhssflfdlukhrfeyg iouaaara g hwpoaieor  jkhfiseqdkotkowgyw    ";
    // size_t n = str.size();
    // size_t w = wordcount(str.c_str(), n);
    // size_t nw = naive_wordcount(str.c_str(), n);
    // std::cout << w << ' ' << nw << '\n';
    int n = 32;
    std::string str;
    std::string const alf = "qwertyuioplkjhgfdsa";
    for (int i = 0; i < n; i++)
        str += alf[rand() % alf.size()];
    std::cout << str << '\n';
    __m128i a = _mm_loadu_si128((__m128i*) (str.c_str()));
    __m128i b = _mm_loadu_si128((__m128i*) (str.c_str() + 16));
    __m128i c = _mm_alignr_epi8(b, a, 5); 
    print(a);
    print(b);
    print(c);

    return 0;
}