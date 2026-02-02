#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <functional>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <bitset>

class StreamGen {
    std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
    std::mt19937 gen;
    std::uniform_int_distribution<int> char_dist;
    std::uniform_int_distribution<int> len_dist;
    
public:
    StreamGen(int s=42) : gen(s), char_dist(0, chars.size()-1), len_dist(5, 30) {
    }
    
    std::string make_string() {
        int len = len_dist(gen);
        std::string s;
        for(int i=0; i<len; i++){
            s += chars[char_dist(gen)];
        }
        return s;
    }
    
    std::vector<std::string> make_stream(int n) {
        std::vector<std::string> v;
        for(int i=0; i<n; i++){
            v.push_back(make_string());
        }
        return v;
    }
    
    std::vector<std::vector<std::string>> split_stream(std::vector<std::string>& s, std::vector<int> percents) {
        std::vector<std::vector<std::string>> parts;
        for(int p : percents) {
            int n = s.size() * p / 100;
            std::vector<std::string> part(s.begin(), s.begin()+n);
            parts.push_back(part);
        }
        return parts;
    }
};

class HashGen {
    uint32_t seed;
    
public:
    HashGen(uint32_t s=0) : seed(s) {}
    
    uint32_t hash(std::string s) {
        uint32_t h = 0;
        for(char c : s) {
            h = h * 31 +(unsigned char) c;
        }
        return h;
    }
    
    void test(std::vector<std::string>& stream) {
        std::vector<int> counts(100, 0);
        for(std::string& s : stream) {
            counts[hash(s) % 100]++;
        }
        
        double expected = stream.size() / 100.0;
        double chi2 = 0;
        for(int c : counts) {
            chi2 += (c-expected)*(c-expected)/expected;
        }
        std::cout << "Chi2: " << chi2 << '\n';
    }
};

int count_unique(std::vector<std::string>& stream) {
    std::unordered_set<std::string> set;
    for(std::string& s : stream){
         set.insert(s);
    }
    return set.size();
}

class HLL {
    int B;
    int m;
    std::vector<int> regs;
    std::function<uint32_t(std::string)> hash_func;
    
    int count_zeros(uint32_t x) {
        if(x == 0){
             return 32 - B;
        }
        int zeros = 0;
        while((x & 0x80000000) == 0) {
            zeros++;
            x <<= 1;
        }
        return zeros;
    }
    
public:
    HLL(int b, std::function<uint32_t(std::string)> h) : B(b), m(1<<b), regs(m, 0), hash_func(h) {}
    
    void add(std::string s) {
        uint32_t h = hash_func(s);
        int idx = h >> (32 - B);
        uint32_t w = h << B;
        int zeros = count_zeros(w) + 1;
        if(zeros > regs[idx]){
            regs[idx] = zeros;
        } 
    }
    
    double estimate() {
        double sum = 0;
        int zero_regs = 0;
        for(int r : regs) {
            if(r == 0){
                zero_regs++;
            }
            sum += 1.0 / (1ULL << r);
        }
        
        double E = 0.7213 * m * m / sum;
        
        if(E <= 2.5 * m && zero_regs > 0) {
            E = m * std::log((double)m / zero_regs);
        }
        
        return E;
    }
    
    void clear() {
        fill(regs.begin(), regs.end(), 0);
    }
};

class HLL_Improved {
    int B;
    int m;
    std::vector<std::bitset<5>> regs;
    std::function<uint32_t(std::string)> hash_func;
    
    int count_zeros(uint32_t x) {
        if(x == 0){
             return 32 - B;
        }
        int zeros = 0;
        while((x & 0x80000000) == 0) {
            zeros++;
            x <<= 1;
        }
        return zeros;
    }
    
public:
    HLL_Improved(int b, std::function<uint32_t(std::string)> h) 
        : B(b), m(1<<b), regs(m), hash_func(h) {}
    
    void add(std::string s) {
        uint32_t h = hash_func(s);
        int idx = h >> (32 - B);
        uint32_t w = h << B;
        int zeros = count_zeros(w) + 1;
        if(zeros > 31){
            zeros = 31;
        }
        
        if(zeros > (int)regs[idx].to_ulong()) {
            regs[idx] = zeros;
        }
    }
    
    double estimate() {
        double sum = 0;
        int zero_regs = 0;
        
        for(auto& r : regs) {
            int val = r.to_ulong();
            if(val == 0){
                zero_regs++;
            }
            sum += 1.0 / (1ULL << val);
        }
        double E = 0.7213 * m * m / sum;
        if(E <= 2.5 * m && zero_regs > 0) {
            E = m * std::log((double)m / zero_regs);
        }
        return E;
    }
    
    size_t memory_usage() const {
        return m * 5 / 8;
    }
};

void print(){
    std::cout<<"HLL_OPTIMISED:\n";


    StreamGen gen(2);
    HashGen hgen;
    std::cout << "Hash test:" << '\n';
    std::vector<std::string> test_stream = gen.make_stream(10000);
    hgen.test(test_stream);
    std::cout << '\n';
    std::cout << "Choosing B value:" << '\n';
    std::vector<std::string> stream = gen.make_stream(50000);
    int real = count_unique(stream);
    
    for(int B=4; B<=12; B++) {
        HLL_Improved hll(B, [&](std::string s){ return hgen.hash(s); });
        for(std::string& s : stream) {
            hll.add(s);
        }
        double est = hll.estimate();
        double error = std::abs(est-real)/real*100;
        std::cout << "B=" << B << " m=" << (1<<B) << " Real=" << real << " Est=" << (int)est  << " Error=" << error << "%\n";
    }    
    std::cout << "Save exp" << '\n';
    
    std::ofstream data("experiment_optimized.csv");
    data << "stream_size,percent,real,estimate,error\n";
    
    std::vector<int> sizes = {10000, 50000, 100000};
    std::vector<int> percents = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for(int size : sizes) {
        std::cout << "\nStream size: " << size << '\n';
        
        for(int run=0; run<5; run++) {
            std::vector<std::string> full_stream = gen.make_stream(size);
            auto parts = gen.split_stream(full_stream, percents);
            
            HLL_Improved hll(8, [&](std::string s){ return hgen.hash(s); });
            
            for(int i=0; i<parts.size(); i++) {
                for(std::string& s : parts[i]) hll.add(s);
                
                int real_part = count_unique(parts[i]);
                double est = hll.estimate();
                double err = std::abs(est-real_part)/real_part*100;
                
                data << size << "," << percents[i] << "," << real_part << "," << (int)est << "," << err << "\n";
            }
        }
    }
    
    data.close();
    std::cout << "\nAccuracy(B=8, m=256)\n";
    std::cout << "Theoretical error: sqrt(1.04/256) = " << sqrt(1.04/256)*100 << "%\n";
    std::cout << "Theoretical error: sqrt(1.3/256) = " << sqrt(1.3/256)*100 << "%\n";
}

int main() {
    StreamGen gen(1);
    HashGen hgen;
    std::cout << "Hash test:" << '\n';
    std::vector<std::string> test_stream = gen.make_stream(10000);
    hgen.test(test_stream);
    std::cout << '\n';
    std::cout << "Choosing B value:" << '\n';
    std::vector<std::string> stream = gen.make_stream(50000);
    int real = count_unique(stream);
    
    for(int B=4; B<=12; B++) {
        HLL hll(B, [&](std::string s){ return hgen.hash(s); });
        for(std::string& s : stream) {
            hll.add(s);
        }
        double est = hll.estimate();
        double error = std::abs(est-real)/real*100;
        std::cout << "B=" << B << " m=" << (1<<B) << " Real=" << real << " Est=" << (int)est  << " Error=" << error << "%\n";
    }    
    std::cout << "Save exp" << '\n';
    
    std::ofstream data("experiment.csv");
    data << "stream_size,percent,real,estimate,error\n";
    
    std::vector<int> sizes = {10000, 50000, 100000};
    std::vector<int> percents = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for(int size : sizes) {
        std::cout << "\nStream size: " << size << '\n';
        
        for(int run=0; run<5; run++) {
            std::vector<std::string> full_stream = gen.make_stream(size);
            auto parts = gen.split_stream(full_stream, percents);
            
            HLL hll(8, [&](std::string s){ return hgen.hash(s); });
            
            for(int i=0; i<parts.size(); i++) {
                for(std::string& s : parts[i]) hll.add(s);
                
                int real_part = count_unique(parts[i]);
                double est = hll.estimate();
                double err = std::abs(est-real_part)/real_part*100;
                
                data << size << "," << percents[i] << "," << real_part << "," << (int)est << "," << err << "\n";
            }
        }
    }
    
    data.close();
    std::cout << "\nAccuracy(B=8, m=256)\n";
    std::cout << "Theoretical error: sqrt(1.04/256) = " << sqrt(1.04/256)*100 << "%\n";
    std::cout << "Theoretical error: sqrt(1.3/256) = " << sqrt(1.3/256)*100 << "%\n";
 
    print();


    std::cout << "\n=== Memory Comparison (B=8) ===\n";
    HLL hll_std(8, [&](std::string s){ return hgen.hash(s); });
    HLL_Improved hll_imp(8, [&](std::string s){ return hgen.hash(s); });

    std::cout << "Standard HLL: " << 256 * sizeof(int) << " bytes\n";
    std::cout << "Improved HLL: " << hll_imp.memory_usage() << " bytes\n";
    std::cout << "Memory saved: " << (256*4 - hll_imp.memory_usage())  << " bytes (" << (100 - hll_imp.memory_usage()*100/(256*4)) << "%)\n";
    return 0;
}